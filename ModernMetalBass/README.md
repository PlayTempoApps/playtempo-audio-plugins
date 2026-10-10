# Modern Metal Bass

Modern Metal Bass is an original multiband processor for tight, mix-ready metal
bass tones. Version 0.4.1 combines a clean controlled foundation with driven
mids, pick definition, dedicated overdrive and distortion stages, tone shaping,
and speaker-style filtering.

## Signal path

- Input gain and adjustable instrument voicing
- Clean, dynamically controlled low band
- Independently driven mid and high bands
- A harmonic-definition layer derived from the fundamental notes
- Body, Growl, and Clank tone controls
- Transient shaping, full-band glue compression, and harmonic excitation
- Original cabinet-style filter curve without copied impulse responses
- Parallel bass overdrive with Drive, Tone, Blend, and Level controls
- Parallel modern distortion with Gain, Tone, Blend, and Level controls
- Wet/dry mix and a smooth safety ceiling near -1 dBFS
- Fifteen starting presets, including five overdrive and five distortion tones
- Explanatory tooltips for all 24 controls

The fixed 1200x760 interface and DSP are original. The project does not copy
third-party plug-in code, artwork, presets, neural models, or impulse responses.

## Formats

- macOS AUv2 and VST3 Universal Binary (`x86_64` and `arm64`)
- Windows 64-bit VST3 build configuration

GarageBand and Logic Pro use AUv2. Ableton Live on Windows uses VST3.

## Build

On macOS, open `ModernMetalBass.xcworkspace` in Xcode and select the
`macOS-AUv2` or `macOS-VST3` scheme. Use Release with both `x86_64` and
`arm64` for distribution.

On Windows, open `ModernMetalBass.sln` in Visual Studio 2022 or run
`scripts\build-release-windows.bat` from Command Prompt after installing the
required C++ workload and Inno Setup 6.

## Attribution

This source uses the iPlug2 framework. See the included iPlug2 and third-party
licenses before redistribution.

The product name, interface, and signal-processing design are original. This
project is not affiliated with, endorsed by, or an official model of any
artist, pedal, plug-in, or instrument manufacturer.
