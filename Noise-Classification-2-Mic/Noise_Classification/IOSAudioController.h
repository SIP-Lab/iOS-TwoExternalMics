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
#import "SpeechProcessing.h"

@interface IOSAudioController : NSObject {
    AudioUnit au;
}

@property AudioUnit au;
@property MovingAverageBuffer *timeBuffer;
@property int allocFrameSize;
@property NSString* info;
@property int classLabel;
@property MovingAverageBuffer *classBuffer;
- (void) processAudio;
- (void) start: (BOOL) store;
- (void) stop;

@end

extern IOSAudioController* audioController;
