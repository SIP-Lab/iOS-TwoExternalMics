//
//  IOSAudioController.m
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/12/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#import "IOSAudioController.h"

#define kOutputBus              0
#define kInputBus               1
#define SHORT2FLOAT             1/32768.0
#define FLOAT2SHORT             32768.0
#define NUMCHANNELS             2
#define BUFFER                  64
#define FRAMESIZE               600
#define FS                      48000
#define DECISIONBUFFERLENGTH    25
#define FAC                     3.051757812500000e-05f


NoiseReduction* noiseReduction;
long                **memoryPointers;
long *nlms;
float               **bufs, **outBufs;
TPCircularBuffer    *inputBuffer, *outputBuffer;
AudioBufferList     *inputBufferList;
IOSAudioController  *audioController;
NSDate              *start;
NSTimeInterval      timeElapsed = 0;
int                 allocFrameSize, count;

AudioStreamBasicDescription format;
ExtAudioFileRef             cfref;
NSString*                   str;


static OSStatus playbackCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData) {
    
    if (outputBuffer->fillCount >= ioData->mBuffers[0].mDataByteSize) {
        AudioBuffer buffer = ioData->mBuffers[0];
        UInt32 size = buffer.mDataByteSize;
        int32_t availableBytes;
        short* tail = TPCircularBufferTail(outputBuffer, &availableBytes);
        memcpy(buffer.mData, tail, size);
        TPCircularBufferConsume(outputBuffer, size);
       if (cfref!=NULL) {
            ExtAudioFileWriteAsync(cfref, inNumberFrames, ioData);
        }
    }
    
    return noErr;
}

static OSStatus recordingCallback(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    
    //Create Audio Buffer List
    inputBufferList->mNumberBuffers = 1;//NUMCHANNELS;
    
    inputBufferList->mBuffers[0].mDataByteSize = NUMCHANNELS * inNumberFrames * sizeof(short);
    inputBufferList->mBuffers[0].mNumberChannels = NUMCHANNELS;
    inputBufferList->mBuffers[0].mData = malloc(inputBufferList->mBuffers[0].mDataByteSize);
    
    
    
    
    // Render audio into the input buffer list
    AudioUnitRender(audioController.au,
                    ioActionFlags,
                    inTimeStamp,
                    inBusNumber,
                    inNumberFrames,
                    inputBufferList);
    
    TPCircularBufferProduceBytes(inputBuffer, (void*)inputBufferList->mBuffers[0].mData, inputBufferList->mBuffers[0].mDataByteSize);
    
    if (inputBuffer->fillCount >= FRAMESIZE*sizeof(short)*NUMCHANNELS) {
        start = [NSDate date];
        [audioController processAudio];
        [audioController.timeBuffer addDatum:[NSNumber numberWithFloat:[[NSDate date] timeIntervalSinceDate:start]]];
        audioController.allocFrameSize = inNumberFrames;
    }
    
//    if (cfref != NULL) {
//        ExtAudioFileWriteAsync(cfref, inNumberFrames, inputBufferList);
//    }
    free(inputBufferList->mBuffers[0].mData);
    inputBufferList->mBuffers[0].mData = NULL;
    
    
    
    return noErr;
}

void deinterleave(const short* interleavedAudio, float* leftChannel, float* rightChannel, int nSamples){
    
    for (int i = 0, j = 0; i < nSamples; i++, j += 2) {
        leftChannel[i]  = interleavedAudio[j] * FAC;
        rightChannel[i] = interleavedAudio[j + 1] * FAC;
    }
    
}

void interleave(const float* leftChannel, const float* rightChannel, short* interleavedAudio, int nSamples){
    for (int i = 0, j = 0; i < nSamples; i++, j += 2) {
        interleavedAudio[j]     = (short)(FLOAT2SHORT * leftChannel[i]);
        interleavedAudio[j + 1] = (short)(FLOAT2SHORT * rightChannel[i]);
    }
}


@implementation IOSAudioController

@synthesize au, timeBuffer, info;

- (id) init {
    
    self = [super init];
    
    // Create the audio session
    [[AVAudioSession sharedInstance] setCategory: AVAudioSessionCategoryPlayAndRecord
                                           error: NULL];
    [[AVAudioSession sharedInstance] setMode: AVAudioSessionModeMeasurement
                                       error:NULL];
    [[AVAudioSession sharedInstance] setPreferredSampleRate:FS
                                                      error:NULL];
    [[AVAudioSession sharedInstance] setPreferredIOBufferDuration:(float)BUFFER/(float)FS
                                                            error:NULL];
    
    
    // Setup Audio Component Description
    AudioComponentDescription desc;
    desc.componentType          = kAudioUnitType_Output;
    desc.componentSubType       = kAudioUnitSubType_RemoteIO;
    desc.componentFlags         = 0;
    desc.componentFlagsMask     = 0;
    desc.componentManufacturer  = kAudioUnitManufacturer_Apple;
    AudioComponent component    = AudioComponentFindNext(NULL, &desc);
    if (AudioComponentInstanceNew(component, &au) != 0) abort();
    
    
    UInt32 value = 1;
    if (AudioUnitSetProperty(au, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &value, sizeof(value))) abort();
    value = 1;
    if (AudioUnitSetProperty(au, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &value, sizeof(value))) abort();
    
    // Setup Audio Stream Basic Description
    //AudioStreamBasicDescription format;
    format.mSampleRate          = FS;
    format.mFormatID            = kAudioFormatLinearPCM;
    format.mFormatFlags         = kAudioFormatFlagIsSignedInteger;
    format.mFramesPerPacket     = 1;
    format.mChannelsPerFrame	= NUMCHANNELS;
    format.mBitsPerChannel      = 16;
    format.mBytesPerPacket      = 4;
    format.mBytesPerFrame       = 4;
    if (AudioUnitSetProperty(au, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &format, sizeof(format))) abort();
    if (AudioUnitSetProperty(au, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &format, sizeof(format))) abort();
    
    // Set input callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = recordingCallback;
    callbackStruct.inputProcRefCon = (__bridge void *)(self);
    AudioUnitSetProperty(au,
                         kAudioOutputUnitProperty_SetInputCallback,
                         kAudioUnitScope_Global,
                         kInputBus,
                         &callbackStruct,
                         sizeof(callbackStruct));
    
    
    
    // Set output callback
    callbackStruct.inputProc = playbackCallback;
    callbackStruct.inputProcRefCon = (__bridge void *)(self);
    AudioUnitSetProperty(au,
                         kAudioUnitProperty_SetRenderCallback,
                         kAudioUnitScope_Global,
                         kOutputBus,
                         &callbackStruct,
                         sizeof(callbackStruct));
    
    AudioUnitInitialize(au);
    
    timeBuffer = [[MovingAverageBuffer alloc] initWithPeriod:round(FS/FRAMESIZE)];
    NSLog(@"Sampling Frequency of Device:\t%.0f", [AVAudioSession sharedInstance].sampleRate);
    NSLog(@"Input Buffer Size for Callback:\t%.0f", [AVAudioSession sharedInstance].IOBufferDuration*[AVAudioSession sharedInstance].sampleRate);
    NSLog(@"Specified Overlap Frame Size:\t%d",FRAMESIZE);
    
    /* Getting all the available inputs */
    
    AVAudioSession* myAudioSession = [AVAudioSession sharedInstance];
    NSArray* inputs = myAudioSession.currentRoute.inputs;
    for (AVAudioSessionPortDescription* port in inputs) {
        str = [NSString stringWithFormat:@"%@", port.portType];        
        info = [NSString stringWithFormat:@"Microphone Specifications\n\nPort: %@\nUID: %@\n\nAudio I/O Setup\n\nInput Microphones: \t%lu\nOutput Speakers: \t%lu\n\n",
                port.portName,
                port.UID,
                (long)myAudioSession.maximumInputNumberOfChannels,
                (long)myAudioSession.maximumOutputNumberOfChannels];
        
    }
    
    return self;
    
}

- (void) start: (BOOL) store {
    
    count = 0;
    
    // Setup the circular buffers, float buffers and algorithm memory pointers
//    nlms = initializeNLMS(FRAMESIZE, 2*FRAMESIZE, 64, 0.1);
//    memoryPointers = (long**)malloc(sizeof(long*) * NUMCHANNELS);
//    for (size_t n = 0; n < NUMCHANNELS; n++) {
//        memoryPointers[n] = initialize(FRAMESIZE, FS, DECISIONBUFFERLENGTH);
//    }
    
    noiseReduction = initNoiseReduction(FRAMESIZE);
    inputBuffer  = (TPCircularBuffer*)malloc(sizeof(TPCircularBuffer));
    outputBuffer = (TPCircularBuffer*)malloc(sizeof(TPCircularBuffer));
    TPCircularBufferInit(inputBuffer,  2048*16);
    TPCircularBufferInit(outputBuffer, 2048*16);
    
    
    bufs = (float**)malloc(sizeof(float*)*NUMCHANNELS);
    outBufs = (float**)malloc(sizeof(float*)*NUMCHANNELS);
    for (size_t n = 0; n < NUMCHANNELS; n++) {
        bufs[n]    = (float*)calloc(FRAMESIZE, sizeof(float));
        outBufs[n] = (float*)calloc(FRAMESIZE, sizeof(float));
    };
    
    // define input buffer list
    inputBufferList = (AudioBufferList *)malloc(sizeof(AudioBufferList)
                                                + sizeof(AudioBuffer) * 2);
    
    if (store) {
        
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"MM_dd_yyyy_HH_mm_ss"];
        NSString* dateString = [formatter stringFromDate:[NSDate date]];
        
        NSArray  *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString* destinationFilePath = [[NSString alloc] initWithFormat: @"%@/%@_%@.caf", documentsDirectory, str, dateString];
        CFURLRef destinationURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)destinationFilePath, kCFURLPOSIXPathStyle, false);
        
        OSStatus status;
        
        
        
        status = ExtAudioFileCreateWithURL(destinationURL, kAudioFileCAFType,
                                           &format, NULL, kAudioFileFlags_EraseFile,
                                           &cfref);
    }
    
    // Start the audio
    AudioOutputUnitStart(au);
}

- (void) stop {
    
    // Stop the audio
    AudioOutputUnitStop(au);
    
    if (cfref != NULL) {
        ExtAudioFileDispose(cfref);
    }
    
    
    // Clean up all initialized memory
//    for (size_t n = 0; n < NUMCHANNELS; n++) {
//        destroy(memoryPointers[n]);
//    }
    free(memoryPointers);
    memoryPointers = NULL;
    
    free(inputBuffer);
    inputBuffer = NULL;
    free(outputBuffer);
    outputBuffer = NULL;
    
    for (size_t n = 0; n < NUMCHANNELS; n++) {
        free(bufs[n]);
        bufs[n] = NULL;
        free(outBufs[n]);
        outBufs[n] = NULL;
    }
    free(bufs);
    bufs = NULL;
    free(outBufs);
    outBufs = NULL;
    free(inputBufferList);
    inputBufferList = NULL;
    
}

- (void) processAudio {
    
    uint32_t frameSize = FRAMESIZE * sizeof(short) * NUMCHANNELS;
    int32_t availableBytes;
    
    short* tail = TPCircularBufferTail(inputBuffer, &availableBytes);
    
    if (availableBytes >= frameSize) {
        // Deinterleave to float
        //deinterleave(tail, bufs[0], bufs[1], FRAMESIZE);
        
        // Run the algorithms
//        for (int i = 0; i < NUMCHANNELS; i++) {
//            compute(memoryPointers[i], bufs[i], outBufs[i], i);
//        }
//        if (count > 120) {
//            processAudio(nlms, tail, outBufs[0], outBufs[1]);
//        }
//        else{
//            count++;
//        }
        short* head = TPCircularBufferHead(outputBuffer, &availableBytes);
        doNoiseReduction(noiseReduction, tail, head, audioController.noiseReductionSelect);
        
        // Interleave data into short
        
        //interleave(bufs[0], bufs[1], head, FRAMESIZE);
        
        
        TPCircularBufferProduce(outputBuffer, frameSize);
        TPCircularBufferConsume(inputBuffer, frameSize);
        
    }
}

-(void) dealloc{
    
    
}

@end

