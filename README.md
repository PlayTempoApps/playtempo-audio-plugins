# Play Tempo audio plugins

Free, advertisement-free bass processing plugins by Play Tempo.

- Modern Metal Bass 0.4.1: multiband metal bass processor, with dedicated
  overdrive and distortion controls and presets.
- DualBassPreamp (Double Bass): two-voice bass preamp with EQ and drive.

Product information and released downloads: https://playtempo.net/plugins

## License

Play Tempo-authored code, documentation and original interface assets are MIT
licensed. The per-product LICENSE files include the full terms. Third-party
components retain their own licenses. The MIT license allows modification and
redistribution, including commercial use; it does not grant trademark rights.

## Build

Clone iPlug2 into an `iPlug2` directory alongside the two product directories:

```sh
git clone https://github.com/iPlug2/iPlug2.git iPlug2
git -C iPlug2 checkout 69c1d4e787267f20ca60bde7824e710924444800
```

Download the required prebuilt dependencies using the framework's instructions.
For a new Windows VST3 build, use the MIT-licensed VST3 SDK revision recorded in
the workflow rather than copying the old local SDK. The Modern Metal Bass
Windows workflow documents all CI dependencies and packaging steps. Builds
require Visual Studio 2022 C++, CMake and Inno Setup on Windows, or Xcode on Mac.
Product README files describe the individual targets.

The new MIT-SDK Windows build must pass CI validation before distribution.
DualBassPreamp Windows packaging still requires its own release validation.
Do not confuse source publication with completed Windows signing.

## Code signing policy

Windows releases are currently unsigned. A SignPath Foundation application is
planned, not yet accepted. There is no active SignPath sponsorship to announce.
The project maintainer and release approver is the PlayTempoApps GitHub account.
Third-party contributions must be reviewed before release. Maintainer MFA and
manual signing approval will be required by the signing provider.

The plugin implementations do not send audio, presets or telemetry over the
network. Build tools fetch dependencies and a DAW may have its own independent
network behavior. Installation and uninstallation instructions are included
with each product. Please report problems using this repository's issues.

No private keys, account credentials, user recordings, DAW sessions, generated
build caches or private Git history are included in this repository.
