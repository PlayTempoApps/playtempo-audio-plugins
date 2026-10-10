#define PLUG_NAME "DualBassPreamp"
#define PLUG_MFR "Play Tempo"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID 'DbP1'
#define PLUG_MFR_ID 'FgAu'
#define PLUG_URL_STR ""
#define PLUG_EMAIL_STR ""
#define PLUG_COPYRIGHT_STR "Copyright 2026 Play Tempo"
#define PLUG_CLASS_NAME DualBassPreamp

#define BUNDLE_NAME "DualBassPreamp"
#define BUNDLE_MFR "PlayTempo"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "DualBassPreamp"

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 960
#define PLUG_HEIGHT 600
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0
#define PLUG_MIN_WIDTH 960
#define PLUG_MIN_HEIGHT 600
#define PLUG_MAX_WIDTH 960
#define PLUG_MAX_HEIGHT 600

#define AUV2_ENTRY DualBassPreamp_Entry
#define AUV2_ENTRY_STR "DualBassPreamp_Entry"
#define AUV2_FACTORY DualBassPreamp_Factory
#define AUV2_VIEW_CLASS DualBassPreamp_View
#define AUV2_VIEW_CLASS_STR "DualBassPreamp_View"

#define AAX_TYPE_IDS 'ITP1'
#define AAX_TYPE_IDS_AUDIOSUITE 'ITA1'
#define AAX_PLUG_MFR_STR "FgAu"
#define AAX_PLUG_NAME_STR "DualBassPreamp\nIPEF"
#define AAX_PLUG_CATEGORY_STR "Effect"
#define AAX_DOES_AUDIOSUITE 1

#define VST3_SUBCATEGORY "Fx"

#define CLAP_MANUAL_URL "https://iplug2.github.io/manuals/example_manual.pdf"
#define CLAP_SUPPORT_URL "https://github.com/iPlug2/iPlug2/wiki"
#define CLAP_DESCRIPTION "A dual-channel bass preamp and drive effect"
#define CLAP_FEATURES "audio-effect"//, "utility"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define ROBOTO_FN "Roboto-Regular.ttf"
#define BLUE_STEEL_PANEL_FN "blue-steel-panel-v2.png"
#define METAL_KNOB_FN "metal-knob-v1.png"
