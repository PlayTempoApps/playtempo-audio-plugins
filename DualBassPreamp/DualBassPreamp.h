#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Oversampler.h"
#include "Smoothers.h"

#include <array>
#include <cmath>

const int kNumPresets = 6;

enum EParams
{
  kParamChannel = 0,
  kParamGain1,
  kParamLevel1,
  kParamGain2,
  kParamLevel2,
  kParamLow,
  kParamMid1,
  kParamMid2,
  kParamHigh,
  kParamBlend,
  kParamBite,
  kParamCabSim,
  kParamBypass,
  // New parameters must be appended so v0.1 DAW automation IDs remain valid.
  kParamInput,
  // v0.3 replaces the old on/off speaker switch with a continuous cabinet amount.
  kParamCabinet,
  kNumParams
};

enum ECtrlTags
{
  kCtrlTagBackground = 0,
  kCtrlTagGain1,
  kCtrlTagLevel1,
  kCtrlTagGain2,
  kCtrlTagLevel2,
  kCtrlTagLow,
  kCtrlTagMid1,
  kCtrlTagMid2,
  kCtrlTagHigh,
  kCtrlTagBlend,
  kCtrlTagChannel,
  kCtrlTagBite,
  kCtrlTagCabinet,
  kCtrlTagInput
};

using namespace iplug;
using namespace igraphics;

class DualBassPreamp final : public Plugin
{
public:
  DualBassPreamp(const InstanceInfo& info);

#if IPLUG_EDITOR
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override
  {
    return width == PLUG_WIDTH && height == PLUG_HEIGHT;
  }
#endif

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
#endif

private:
#if IPLUG_DSP
  struct Biquad
  {
    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;
    double z1 = 0.0;
    double z2 = 0.0;

    sample Process(sample x);
    void Reset();
    void SetHighPass(double sampleRate, double frequency, double q);
    void SetLowPass(double sampleRate, double frequency, double q);
    void SetPeak(double sampleRate, double frequency, double q, double gainDB);
    void SetLowShelf(double sampleRate, double frequency, double gainDB);
    void SetHighShelf(double sampleRate, double frequency, double gainDB);

  private:
    void SetCoefficients(double nb0, double nb1, double nb2, double na0, double na1, double na2);
  };

  struct ChannelState
  {
    Biquad ampHighPass;
    Biquad attackShelf;
    Biquad attackPeak;
    Biquad distortionHighPass;
    Biquad speakerBody;
    Biquad speakerTilt;
    Biquad speakerLowPass;
    Biquad lowEQ;
    Biquad mid1EQ;
    Biquad mid2EQ;
    Biquad highEQ;

    void Reset();
  };

  struct OversampledParams
  {
    double driveAmount = 0.0;
    double characterMix = 1.0;
    double attackMix = 0.0;
    double speakerMix = 1.0;
  };

  sample ProcessSignal(ChannelState& state, OverSampler<sample>& oversampler, sample input,
                       const OversampledParams& params);
  static sample Saturate(sample input, double driveAmount);
  void UpdateFilterCoefficients(ChannelState& state, double lowDB, double mid1DB, double mid2DB,
                                double highDB);

  std::array<ChannelState, 2> mChannelStates;
  OverSampler<sample> mOversamplerLeft {k4x, false};
  OverSampler<sample> mOversamplerRight {k4x, false};
  LogParamSmooth<sample, 1> mInputSmoother {20.0, 1.0};
  LogParamSmooth<sample, 1> mDriveSmoother {20.0, 1.0};
  LogParamSmooth<sample, 1> mLevelSmoother {20.0, 1.0};
  LogParamSmooth<sample, 1> mBlendSmoother {15.0, 1.0};
  LogParamSmooth<sample, 1> mAttackSmoother {8.0, 0.0};
  LogParamSmooth<sample, 1> mSpeakerSmoother {8.0, 1.0};
#endif
};
