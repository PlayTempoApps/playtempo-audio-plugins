# Open-source distribution

The Play Tempo-authored code, documentation and original interface assets in
this project are released under the MIT license in LICENSE. Third-party code
and assets retain their original licenses; MIT does not replace those notices.
The license does not grant trademark rights or imply endorsement of forks.

## Dependencies

- iPlug2: zlib-style license; preserve THIRD_PARTY-iPlug2-LICENSE.txt and the
  original licenses of WDL, NanoVG, NanoSVG and other enabled components.
- Roboto-Regular.ttf: Google Roboto font, Apache-2.0. See
  resources/fonts/LICENSE-Roboto.txt. The font is unchanged.
- VST3 SDK: new open-source Windows builds must use a pinned, MIT-licensed SDK
  revision, rather than redistributing the old local SDK with missing notices.
  Candidate revision: 3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96.
  Compatibility with that revision still requires a Windows CI build and test.
- macOS Audio Unit: supplied by the operating system / Xcode SDK; do not
  redistribute Apple's SDK. Proprietary AAX and VST2 SDKs are not included.

## Publication and signing status

Open-source preparation does not mean that SignPath has accepted this project.
Do not describe any Windows installer as signed until the plugin binary and
installer have verified, timestamped Authenticode signatures.

SignPath requires public sources, a verifiable build, maintainer MFA, signing
roles, manual release approval, installation/uninstallation documentation and
a public code-signing policy. Publish a sponsorship attribution only once the
provider confirms acceptance. Do not imply that signing guarantees the absence
of Microsoft SmartScreen prompts.

The plugin implementation does not upload audio, presets or telemetry.
Build tooling downloads dependencies; DAWs can have separate network behavior.

Never publish credentials, certificates, private keys, reference recordings,
DAW sessions, user-specific IDE files, build caches or old private Git history.
