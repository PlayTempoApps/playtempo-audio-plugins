# Modern Metal Bass 0.4.1 — User Guide

Modern Metal Bass is an original multiband processor for tight lows, dense
distorted mids, and controlled pick attack in modern metal productions.

## Plug-in formats

- macOS: Audio Unit v2 and VST3, Universal Binary for Intel and Apple Silicon
- Windows: 64-bit VST3 build configuration

## Controls

- **Input:** Sets the level feeding the processor. Higher settings create more
  saturation; lower settings preserve additional dynamics.
- **Input Shape:** Prepares dark or bright instruments for the drive stages.
  Raise it for more upper-mid definition before distortion.
- **Low Cross:** Sets the upper edge of the clean low band.
- **High Cross:** Sets where the upper drive band begins.
- **Low Tight:** Compresses and stabilizes the low band without distorting it.
- **Mid Drive:** Creates the dense growl at the center of the metal tone.
- **High Drive:** Adds controlled upper-band edge.
- **Attack:** Emphasizes the fast pick transient of each note.
- **Body:** Adjusts weight around 180 Hz.
- **Growl:** Adjusts definition around 850 Hz.
- **Clank:** Adjusts string detail around 2.4 kHz.
- **Exciter:** Derives extra harmonics from the mid band.
- **Glue:** Applies musical compression after the bands are recombined.
- **Cabinet:** Blends an original speaker-like filter curve.
- **Mix:** Blends the clean bass with the complete processed tone.
- **Output:** Sets final level before the smooth safety ceiling.
- **OD Drive:** Sets the amount of soft, touch-sensitive asymmetric clipping.
- **OD Tone:** Moves the overdrive low-pass filter from warm to bright.
- **OD Blend:** Mixes the overdrive branch with the core bass signal. Zero
  bypasses the overdrive stage.
- **OD Level:** Sets the level of the overdrive branch before blending.
- **Dist Gain:** Sets the amount of dense, harder symmetric clipping.
- **Dist Tone:** Moves the distortion low-pass filter from dark to biting.
- **Dist Blend:** Mixes the distortion branch with its input. Zero bypasses the
  distortion stage.
- **Dist Level:** Sets the level of the distortion branch before blending.

Hover over any control for a short explanation. Double-click a displayed value
to enter it precisely. Use the DAW's own plug-in power button for bypass.

## Starting settings

Load **Modern Metal** first. Use **Deep Grind** for a heavier low-mid sound,
**Pick Attack** for fast articulate parts, **Tight Clean** for a restrained
foundation, or **Wide Bite** for a more open parallel tone.

The five presets prefixed **OD:** cover tight, warm, pick-forward, low-tuned,
and open overdrive tones. The five presets prefixed **DIST:** cover modern wall,
tight razor, low-tuned mass, pick grind, and maximum-gain controlled distortion.

As a neutral starting point, use full instrument volume, center-detented bass
and treble controls, and a centered pickup blend. For more pick definition,
move the blend slightly toward the bridge pickup and raise Input Shape.

Use a firm pick attack between the pickup midpoint and the bridge pickup. Avoid
clipping the audio interface; recorded peaks around -12 to -6 dBFS are suitable.

## Installation

On macOS, run the supplied installer and select Audio Unit, VST3, or both.
GarageBand and Logic Pro use Audio Unit. Restart the DAW after installation.

On Windows, install the VST3 in
`C:\Program Files\Common Files\VST3`, restart Ableton Live, and rescan plug-ins.

## Legal

The name, interface, DSP, cabinet curve, and presets are original. No third-party
plug-in code, artwork, presets, neural models, or impulse responses are included.
This product is not affiliated with or endorsed by any artist or manufacturer.
