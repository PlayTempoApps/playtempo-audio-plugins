
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1 || TARGET_OS_VISION == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vModernMetalBass
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vModernMetalBass
#import <ModernMetalBassAU/IPlugAUViewController.h>
#import <ModernMetalBassAU/IPlugAUAudioUnit.h>

//! Project version number for ModernMetalBassAU.
FOUNDATION_EXPORT double ModernMetalBassAUVersionNumber;

//! Project version string for ModernMetalBassAU.
FOUNDATION_EXPORT const unsigned char ModernMetalBassAUVersionString[];

@class IPlugAUViewController_vModernMetalBass;
