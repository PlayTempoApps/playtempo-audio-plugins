# DualBassPreamp

DualBassPreamp is an original two-voice bass preamp and drive effect built with
iPlug2. Version 1.0.0 is the first public release.

## Current feature set

- Two switchable voices with independent drive and output level
- Shared active EQ at 50 Hz, 200 Hz, 500 Hz, and 3 kHz
- Input trim and phase-coherent Character Mix
- Broad Attack voicing and continuously variable Cabinet filtering
- 4x oversampled nonlinear drive path
- Six original starting presets
- Fixed-size 960x600 photorealistic Blue Steel panel with layered metal knobs,
  live pointers, a hardware-style Attack switch, and persistent voice indication
- Explanatory tooltips for every visible control
- Universal macOS AUv2 and VST3 binaries (`x86_64` and `arm64`)
- 64-bit Windows VST3 build configuration

The plug-in is published under the **Play Tempo** manufacturer name. Host
bypass is used instead of a duplicate on-panel bypass control. A tuner is
deliberately omitted because supported DAWs already provide one.

## Build target

On macOS, open `DualBassPreamp.xcworkspace` in Xcode and select the
`macOS-AUv2` or `macOS-VST3` scheme. For a distributable build, use the Release
configuration with both `x86_64` and `arm64` architectures.

On Windows, open `DualBassPreamp.sln` in Visual Studio 2022 and build the
`DualBassPreamp-vst3` project in the Release/x64 configuration. The
`scripts/build-release-windows.bat` script builds the plug-in and creates the
Inno Setup installer when the required tools are installed.

## Status and attribution

This source is based on the iPlug2 template and framework. See the iPlug2
license and the licenses of its bundled dependencies before redistribution.

The product name, interface, and signal-processing design are original. This
project is not affiliated with, endorsed by, or an official model of any pedal
manufacturer or artist.
