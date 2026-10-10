#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Smoothers.h"

#include <array>

const int kNumPresets = 15;

enum EParams
{
  kParamInput = 0,
  kParamSourceMatch,
  kParamLowCrossover,
  kParamHighCrossover,
  kParamLowTight,
  kParamMidDrive,
  kParamHighDrive,
  kParamBody,
  kParamGrowl,
  kParamClank,
  kParamAttack,
  kParamGlue,
  kParamExciter,
  kParamCabinet,
  kParamMix,
  kParamOutput,
  // Append new parameters to preserve automation/state compatibility with v0.3.0.
  kParamOverdriveDrive,
  kParamOverdriveTone,
  kParamOverdriveBlend,
  kParamOverdriveLevel,
  kParamDistortionGain,
  kParamDistortionTone,
  kParamDistortionBlend,
  kParamDistortionLevel,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class ModernMetalBass final : public Plugin
{
public:
  ModernMetalBass(const InstanceInfo& info);

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
    void SetHighShelf(double sampleRate, double frequency, double gainDB);

  private:
    void SetCoefficients(double nb0, double nb1, double nb2,
                         double na0, double na1, double na2);
  };

  struct OnePoleLowPass
  {
    double coefficient = 0.0;
    double state = 0.0;

    void SetFrequency(double sampleRate, double frequency);
    sample Process(sample x);
    void Reset() { state = 0.0; }
  };

  struct ChannelState
  {
    Biquad inputHighPass;
    Biquad sourceBoom;
    Biquad sourceDefinition;
    Biquad sourcePresence;
    OnePoleLowPass lowSplit;
    OnePoleLowPass lowSplitSecond;
    OnePoleLowPass highSplit;
    OnePoleLowPass highSplitSecond;
    OnePoleLowPass harmonicLowPass;
    Biquad definitionGrowl;
    Biquad definitionClank;
    Biquad definitionShelf;
    Biquad bodyEQ;
    Biquad growlEQ;
    Biquad clankEQ;
    Biquad cabinetBody;
    Biquad cabinetNotch;
    Biquad cabinetClank;
    Biquad cabinetTilt;
    Biquad cabinetLowPass;
    Biquad overdriveHighPass;
    Biquad overdriveTone;
    Biquad distortionHighPass;
    Biquad distortionTone;
    double lowEnvelope = 0.0;
    double fastEnvelope = 0.0;
    double slowEnvelope = 0.0;
    double glueEnvelope = 0.0;

    void Reset();
  };

  struct BlockParameters
  {
    double sourceMatch;
    double lowTight;
    double midDrive;
    double highDrive;
    double attack;
    double glue;
    double exciter;
    double cabinet;
    double overdriveDrive;
    double overdriveTone;
    double overdriveBlend;
    double overdriveLevel;
    double distortionGain;
    double distortionTone;
    double distortionBlend;
    double distortionLevel;
  };

  void UpdateCoefficients(ChannelState& state);
  sample ProcessWet(ChannelState& state, sample input, const BlockParameters& parameters);
  static sample ProcessOverdrive(ChannelState& state, sample input,
                                 const BlockParameters& parameters);
  static sample ProcessDistortion(ChannelState& state, sample input,
                                  const BlockParameters& parameters);
  static sample Saturate(sample input, double amount, double bias);
  static sample SafeLimit(sample input);

  std::array<ChannelState, 2> mChannelStates;
  LogParamSmooth<sample, 1> mInputSmoother {20.0, 1.0};
  LogParamSmooth<sample, 1> mMixSmoother {15.0, 1.0};
  LogParamSmooth<sample, 1> mOutputSmoother {20.0, 1.0};
#endif
};
