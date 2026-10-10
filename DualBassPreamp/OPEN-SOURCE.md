# Open-source distribution

The Play Tempo-authored code, documentation and original interface assets in
this project are released under the MIT license in LICENSE. Third-party code
and assets retain their original licenses. The license does not grant trademark
rights or imply endorsement of forks.

Dependencies include iPlug2 (zlib-style license and its bundled components),
the unchanged Google Roboto font (Apache-2.0; see
resources/fonts/LICENSE-Roboto.txt), and the platform plugin SDKs.
Preserve THIRD_PARTY-iPlug2-LICENSE.txt and all enabled dependency notices.
Do not redistribute Apple's SDK or proprietary AAX/VST2 SDKs.

Windows open-source builds must use a pinned MIT-licensed VST3 SDK. The candidate
revision is 3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96; a Windows compatibility
build is still required. The existing local SDK lacks complete license notices
and is deliberately excluded from the public-source export.

SignPath acceptance and Windows signing are pending. A code-signing policy,
maintainer MFA, verifiable CI build, signing roles and manual release approvals
are required. Never claim sponsorship before acceptance, or promise that an
Authenticode signature eliminates all SmartScreen warnings.

The plugin implementation does not upload audio, presets or telemetry.
Never publish private keys, account credentials, reference recordings, DAW
sessions, user-specific IDE files, build caches or old private Git history.
