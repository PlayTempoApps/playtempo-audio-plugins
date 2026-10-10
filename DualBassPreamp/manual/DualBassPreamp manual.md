# DualBassPreamp 1.0.0 — User Manual

DualBassPreamp is a two-voice bass preamp and drive effect for macOS and
Windows DAWs.

## Plug-in formats

- macOS: Audio Unit v2 and VST3, Universal Binary for Intel and Apple Silicon
- Windows: 64-bit VST3

## Controls

- **Active Voice:** Selects Voice A or Voice B. The selected side remains lit.
- **Drive A / Drive B:** Sets the saturation amount stored for each voice.
- **Output A / Output B:** Matches the final level of each voice.
- **Bass 50:** Adjusts the deepest bass weight.
- **Body 200:** Adjusts warmth and low-mid body.
- **Punch 500:** Shapes note impact and midrange definition.
- **Presence 3K:** Adjusts upper-mid clarity and string detail.
- **Input:** Matches the instrument level entering the drive circuit.
- **Character Mix:** Blends clean bass with the selected driven voice.
- **Attack:** Adds pick definition and upper-mid presence before the drive.
- **Cabinet:** Progressively smooths harsh highs and adds speaker-like body.
  Set it to 0% to disable this filtering.

The four EQ bands are shared by both channels. Gain and level are independent
for each voice. Hover over any visible control to read its short explanation.

The plug-in does not duplicate the DAW's bypass control and does not run an
internal tuner. Use the host's plug-in power button and tuner when needed.

## Installation

On macOS, run the supplied installer package and select Audio Unit, VST3, or
both. GarageBand and Logic Pro use the Audio Unit version. Other macOS DAWs may
use either format.

On Windows, run the supplied installer executable. The VST3 plug-in is
installed in the standard system VST3 directory:
`C:\Program Files\Common Files\VST3`.

Restart the DAW after installation. If the plug-in is not listed immediately,
run the DAW's plug-in rescan command.

## Compatibility

The interface has a fixed size of 960x600 pixels. The macOS build supports
Intel and Apple Silicon processors. The Windows release is a native 64-bit
VST3 plug-in.

Keep monitoring volume low when first loading an experimental audio plug-in.

## Legal

The product name, interface, and signal-processing design are original. This
project is not affiliated with, endorsed by, or an official model of any pedal
manufacturer or artist.
