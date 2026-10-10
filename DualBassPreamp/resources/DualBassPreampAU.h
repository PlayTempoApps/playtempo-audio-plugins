
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1 || TARGET_OS_VISION == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vDualBassPreamp
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vDualBassPreamp
#import <DualBassPreampAU/IPlugAUViewController.h>
#import <DualBassPreampAU/IPlugAUAudioUnit.h>

//! Project version number for DualBassPreampAU.
FOUNDATION_EXPORT double DualBassPreampAUVersionNumber;

//! Project version string for DualBassPreampAU.
FOUNDATION_EXPORT const unsigned char DualBassPreampAUVersionString[];

@class IPlugAUViewController_vDualBassPreamp;
