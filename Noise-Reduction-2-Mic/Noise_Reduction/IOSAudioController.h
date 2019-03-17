//
//  IOSAudioController.h
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/12/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TPCircularBuffer.h"
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import "MovingAverageBuffer.h"
#import "NoiseReduction.h"
//#import "SpeechProcessing.h"
//#import "NLMS.h"

@interface IOSAudioController : NSObject {
    AudioUnit au;
}

@property AudioUnit au;
@property MovingAverageBuffer *timeBuffer;
@property int allocFrameSize;
@property bool audioOutput;
@property int noiseReductionSelect;
@property NSString* info;

- (void) processAudio;
- (void) start: (BOOL) store;
- (void) stop;

@end

extern IOSAudioController* audioController;
