#include "ModernMetalBass.h"
#include "IPlug_include_in_plug_src.h"

#include <algorithm>
#include <cmath>

#if IPLUG_EDITOR
#include "IControls.h"
#endif

namespace
{
constexpr double kPi = 3.1415926535897932384626433832795;

double ClampFrequency(double sampleRate, double frequency)
{
  return std::clamp(frequency, 10.0, sampleRate * 0.45);
}

double DecibelsToGain(double decibels)
{
  return std::pow(10.0, decibels / 20.0);
}

double EnvelopeCoefficient(double sampleRate, double milliseconds)
{
  return std::exp(-1.0 / (sampleRate * milliseconds * 0.001));
}

#if IPLUG_EDITOR
class MidnightPanelControl final : public IControl
{
public:
  MidnightPanelControl(const IRECT& bounds, const IBitmap& bitmap)
  : IControl(bounds)
  , mBitmap(bitmap)
  {
    SetIgnoreMouse(true);
  }

  void Draw(IGraphics& g) override
  {
    g.DrawFittedBitmap(mBitmap, mRECT);

    // Preserve the approved header while redrawing its supporting copy at a
    // size that remains readable in the larger fixed window.
    g.FillRect(IColor(250, 5, 12, 19), IRECT(46.f, 103.f, 565.f, 140.f));
    g.DrawText(IText(12.f, IColor(255, 224, 204, 169), "Roboto-Regular", EAlign::Near),
               "TIGHT LOWS  •  DISTORTED MIDS  •  PICK ATTACK",
               IRECT(50.f, 103.f, 565.f, 121.f));
    g.DrawText(IText(10.f, IColor(255, 151, 184, 211), "Roboto-Regular", EAlign::Near),
               "HOVER OVER ANY CONTROL FOR SOUND-SHAPING TIPS",
               IRECT(50.f, 121.f, 565.f, 138.f));

    // Cover the concept's original control field, then present the 24 controls
    // as six clearly named sections with four controls each.
    g.FillRect(IColor(253, 3, 9, 16), IRECT(20.f, 143.f, 1180.f, 724.f));
    g.DrawRect(IColor(155, 53, 105, 155), IRECT(21.f, 144.f, 1179.f, 723.f), nullptr, 1.f);

    const char* sectionNames[6] = {
      "SOURCE", "BAND ENGINE", "TONE SHAPE", "FINISH", "OVERDRIVE", "DISTORTION"
    };
    constexpr float left = 28.f;
    constexpr float top = 151.f;
    constexpr float panelWidth = 564.f;
    constexpr float panelHeight = 177.f;
    constexpr float columnGap = 16.f;
    constexpr float rowGap = 13.f;
    for (int section = 0; section < 6; ++section)
    {
      const int row = section / 2;
      const int column = section % 2;
      const float x = left + column * (panelWidth + columnGap);
      const float y = top + row * (panelHeight + rowGap);
      const IRECT panel(x, y, x + panelWidth, y + panelHeight);
      g.FillRect(IColor(238, 5, 15, 25), panel);
      g.DrawRect(IColor(185, 31, 91, 151), panel, nullptr, 1.1f);
      g.FillRect(IColor(245, 7, 24, 39), IRECT(x + 1.f, y + 1.f, x + panelWidth - 1.f, y + 30.f));
      g.FillRect(IColor(255, 31, 121, 255), IRECT(x + 1.f, y + 29.f, x + panelWidth - 1.f, y + 31.f));
      g.DrawText(IText(13.f, IColor(255, 182, 216, 243), "Roboto-Regular", EAlign::Near),
                 sectionNames[section], IRECT(x + 15.f, y + 4.f, x + panelWidth - 15.f, y + 27.f));
    }

    g.DrawText(IText(10.f, IColor(255, 128, 164, 201), "Roboto-Regular", EAlign::Far),
               "v0.4.1", IRECT(1110.f, 738.f, 1184.f, 756.f));
  }

private:
  IBitmap mBitmap;
};

class MidnightKnobControl final : public IKnobControlBase
{
public:
  MidnightKnobControl(const IRECT& bounds, int parameter, const char* label)
  : IKnobControlBase(bounds, parameter)
  , mLabel(label)
  {}

  void Draw(IGraphics& g) override
  {
    const IColor champagne(255, 246, 229, 194);
    const IColor blue(255, 33, 121, 255);
    const float cx = mRECT.MW();
    const float cy = mRECT.T + 69.f;
    const float radius = 34.f;

    const IRECT labelRect(cx - 64.f, mRECT.T + 1.f, cx + 64.f, mRECT.T + 24.f);
    g.FillRect(IColor(248, 3, 10, 17), labelRect);
    g.DrawText(IText(12.5f, champagne, "Roboto-Regular", EAlign::Center), mLabel, labelRect);

    // Cover the static concept knob, then rebuild its brushed gunmetal face as
    // a live control. Champagne ticks and the cobalt arc retain the approved look.
    g.FillCircle(IColor(225, 3, 8, 13), cx, cy, radius + 8.f);
    g.FillCircle(IColor(255, 1, 4, 8), cx + 1.5f, cy + 2.5f, radius + 5.5f);
    g.FillCircle(IColor(255, 78, 81, 78), cx, cy, radius + 3.5f);
    g.FillCircle(IColor(255, 7, 10, 14), cx, cy, radius + 1.5f);

    g.PathCircle(cx, cy, radius);
    g.PathFill(IPattern::CreateLinearGradient(cx - radius, cy - radius,
                                              cx + radius, cy + radius,
      {{IColor(255, 52, 55, 57), 0.f}, {IColor(255, 174, 174, 166), .28f},
       {IColor(255, 70, 73, 75), .52f}, {IColor(255, 137, 137, 130), .73f},
       {IColor(255, 31, 34, 37), 1.f}}));
    g.PathCircle(cx, cy, radius - .5f);
    g.PathFill(IPattern::CreateRadialGradient(cx - 7.f, cy - 8.f, radius * 1.15f,
      {{IColor(85, 242, 237, 222), 0.f}, {IColor(8, 255, 255, 255), .58f},
       {IColor(110, 0, 2, 5), 1.f}}));
    g.DrawCircle(IColor(255, 222, 211, 188), cx, cy, radius, nullptr, 1.2f);
    g.DrawCircle(IColor(80, 255, 255, 255), cx, cy, radius * .72f, nullptr, .45f);
    g.DrawCircle(IColor(50, 0, 0, 0), cx, cy, radius * .48f, nullptr, .45f);
    for (int index = 0; index < 32; ++index)
    {
      const float machiningAngle = index * 11.25f;
      g.DrawRadialLine(IColor(index % 2 == 0 ? 10 : 5, 255, 255, 255),
                       cx, cy, machiningAngle, radius * .14f, radius * .90f,
                       nullptr, .38f);
    }

    const float trackRadius = radius + 5.5f;
    const float valueAngle = -135.f + static_cast<float>(GetValue()) * 270.f;
    g.DrawArc(IColor(255, 22, 49, 83), cx, cy, trackRadius, -135.f, 135.f, nullptr, 2.2f);
    g.DrawArc(blue, cx, cy, trackRadius, -135.f, valueAngle, nullptr, 2.8f);
    g.DrawRadialLine(IColor(190, 0, 3, 8), cx + 1.f, cy + 1.f, valueAngle,
                     radius * .22f, radius * .82f, nullptr, 4.4f);
    g.DrawRadialLine(blue, cx, cy, valueAngle, radius * .22f, radius * .82f, nullptr, 3.1f);
    g.DrawRadialLine(IColor(255, 157, 220, 255), cx, cy, valueAngle,
                     radius * .35f, radius * .80f, nullptr, 1.f);
    g.FillCircle(IColor(255, 9, 15, 22), cx, cy, 3.2f);
    g.DrawCircle(IColor(190, 205, 197, 177), cx, cy, 3.2f, nullptr, .7f);

    WDL_String display;
    GetParam()->GetDisplayWithLabel(display);
    const IRECT valueRect(cx - 57.f, mRECT.T + 111.f, cx + 57.f, mRECT.T + 136.f);
    g.FillRect(IColor(248, 3, 10, 17), valueRect);
    g.DrawText(IText(12.f, champagne, "Roboto-Regular", EAlign::Center), display.Get(), valueRect);
  }

  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override
  {
    PromptUserInput(IRECT(mRECT.L + 20.f, mRECT.B - 18.f, mRECT.R - 20.f, mRECT.B));
  }

private:
  const char* mLabel;
};
#endif
}

ModernMetalBass::ModernMetalBass(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamInput)->InitGain("Input", 8.0, -18.0, 18.0, 0.1);
  GetParam(kParamSourceMatch)->InitPercentage("Input Shape", 82.0);
  GetParam(kParamLowCrossover)->InitDouble("Low Crossover", 178.0, 80.0, 300.0, 1.0, "Hz",
                                           IParam::kFlagsNone, "", IParam::ShapePowCurve(2.0));
  GetParam(kParamHighCrossover)->InitDouble("High Crossover", 1973.0, 800.0, 4000.0, 1.0, "Hz",
                                            IParam::kFlagsNone, "", IParam::ShapePowCurve(2.0));
  GetParam(kParamLowTight)->InitPercentage("Low Tight", 60.0);
  GetParam(kParamMidDrive)->InitPercentage("Mid Drive", 56.0);
  GetParam(kParamHighDrive)->InitPercentage("High Drive", 64.0);
  GetParam(kParamBody)->InitGain("Body", 0.0, -12.0, 12.0, 0.1);
  GetParam(kParamGrowl)->InitGain("Growl", 0.0, -12.0, 12.0, 0.1);
  GetParam(kParamClank)->InitGain("Clank", 3.0, -12.0, 12.0, 0.1);
  GetParam(kParamAttack)->InitPercentage("Attack", 25.0);
  GetParam(kParamGlue)->InitPercentage("Glue", 72.0);
  GetParam(kParamExciter)->InitPercentage("Exciter", 44.0);
  GetParam(kParamCabinet)->InitPercentage("Cabinet", 70.0);
  GetParam(kParamMix)->InitPercentage("Mix", 100.0);
  GetParam(kParamOutput)->InitGain("Output", 3.0, -18.0, 12.0, 0.1);
  GetParam(kParamOverdriveDrive)->InitPercentage("Overdrive Drive", 0.0);
  GetParam(kParamOverdriveTone)->InitPercentage("Overdrive Tone", 50.0);
  GetParam(kParamOverdriveBlend)->InitPercentage("Overdrive Blend", 0.0);
  GetParam(kParamOverdriveLevel)->InitGain("Overdrive Level", 0.0, -18.0, 6.0, 0.1);
  GetParam(kParamDistortionGain)->InitPercentage("Distortion Gain", 0.0);
  GetParam(kParamDistortionTone)->InitPercentage("Distortion Tone", 50.0);
  GetParam(kParamDistortionBlend)->InitPercentage("Distortion Blend", 0.0);
  GetParam(kParamDistortionLevel)->InitGain("Distortion Level", 0.0, -18.0, 6.0, 0.1);

  MakePreset("Modern Metal", 8.0, 82.0, 178.0, 1973.0, 60.0, 56.0, 64.0,
             0.0, 0.0, 3.0, 25.0, 72.0, 44.0, 70.0, 100.0, 3.0,
             0.0, 50.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("Deep Grind", 7.0, 64.0, 165.0, 2100.0, 72.0, 78.0, 34.0,
             3.0, 7.5, -1.0, 52.0, 66.0, 38.0, 82.0, 100.0, -4.5,
             0.0, 50.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("Pick Attack", 6.0, 88.0, 155.0, 1650.0, 70.0, 72.0, 58.0,
             -1.5, 7.0, 5.5, 82.0, 52.0, 56.0, 58.0, 100.0, -5.0,
             0.0, 50.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("Tight Clean", 5.0, 42.0, 190.0, 2200.0, 48.0, 24.0, 14.0,
             2.0, 1.5, -2.0, 35.0, 40.0, 14.0, 50.0, 88.0, -2.5,
             0.0, 50.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("Wide Bite", 6.0, 70.0, 175.0, 2350.0, 55.0, 50.0, 38.0,
             0.5, 4.0, 3.0, 60.0, 48.0, 34.0, 38.0, 80.0, -3.5,
             0.0, 50.0, 0.0, 0.0, 0.0, 50.0, 0.0, 0.0);

  MakePreset("OD: Tight Grind", 7.0, 80.0, 175.0, 1900.0, 65.0, 45.0, 50.0,
             0.0, 3.0, 3.0, 55.0, 62.0, 38.0, 62.0, 100.0, 0.0,
             48.0, 64.0, 48.0, -2.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("OD: Warm Steel", 6.0, 65.0, 170.0, 2200.0, 58.0, 40.0, 34.0,
             2.0, 4.0, 0.0, 40.0, 60.0, 30.0, 62.0, 100.0, 0.0,
             55.0, 38.0, 55.0, -3.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("OD: Pick Crunch", 6.0, 88.0, 160.0, 1700.0, 70.0, 42.0, 50.0,
             -1.0, 4.0, 5.0, 78.0, 52.0, 45.0, 52.0, 100.0, 0.0,
             46.0, 75.0, 44.0, -2.5, 0.0, 50.0, 0.0, 0.0);
  MakePreset("OD: Low-Tuned Push", 7.0, 70.0, 145.0, 1950.0, 78.0, 48.0, 35.0,
             3.0, 4.0, 1.0, 45.0, 68.0, 34.0, 72.0, 100.0, -1.0,
             65.0, 47.0, 58.0, -4.0, 0.0, 50.0, 0.0, 0.0);
  MakePreset("OD: Open Bite", 5.0, 78.0, 175.0, 2400.0, 55.0, 32.0, 38.0,
             1.0, 2.0, 4.0, 60.0, 45.0, 36.0, 35.0, 90.0, 0.0,
             38.0, 82.0, 40.0, -1.5, 0.0, 50.0, 0.0, 0.0);

  MakePreset("DIST: Modern Wall", 7.0, 82.0, 175.0, 1950.0, 68.0, 55.0, 58.0,
             0.0, 4.0, 3.0, 58.0, 68.0, 42.0, 65.0, 100.0, -2.0,
             0.0, 50.0, 0.0, 0.0, 62.0, 60.0, 55.0, -5.0);
  MakePreset("DIST: Tight Razor", 6.0, 90.0, 165.0, 1650.0, 78.0, 52.0, 62.0,
             -1.0, 4.0, 6.0, 82.0, 58.0, 52.0, 48.0, 100.0, -2.5,
             0.0, 50.0, 0.0, 0.0, 68.0, 75.0, 48.0, -5.5);
  MakePreset("DIST: Low-Tuned Mass", 7.0, 66.0, 145.0, 2050.0, 82.0, 62.0, 38.0,
             4.0, 5.0, 0.0, 42.0, 74.0, 34.0, 76.0, 100.0, -3.0,
             0.0, 50.0, 0.0, 0.0, 75.0, 38.0, 65.0, -7.0);
  MakePreset("DIST: Pick Grind", 6.0, 92.0, 155.0, 1550.0, 75.0, 58.0, 66.0,
             -1.0, 5.0, 6.0, 88.0, 56.0, 50.0, 52.0, 100.0, -3.0,
             0.0, 50.0, 0.0, 0.0, 66.0, 82.0, 52.0, -6.0);
  MakePreset("DIST: Controlled Chaos", 6.0, 76.0, 170.0, 1800.0, 72.0, 70.0, 55.0,
             0.0, 6.0, 4.0, 65.0, 78.0, 48.0, 60.0, 100.0, -4.0,
             0.0, 50.0, 0.0, 0.0, 88.0, 55.0, 70.0, -9.0);

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [this]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };

  mLayoutFunc = [this](IGraphics* graphics) {
    if (graphics->NControls())
      return;

    graphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    graphics->EnableMouseOver(true);
    graphics->EnableTooltips(true);
    graphics->AttachTextEntryControl();
    const IBitmap panelBitmap = graphics->LoadBitmap(MIDNIGHT_PANEL_FN);
    graphics->AttachControl(new MidnightPanelControl(graphics->GetBounds(), panelBitmap));

    const int parameters[24] = {
      kParamInput, kParamSourceMatch, kParamLowCrossover, kParamHighCrossover,
      kParamLowTight, kParamMidDrive, kParamHighDrive, kParamAttack,
      kParamBody, kParamGrowl, kParamClank, kParamExciter,
      kParamGlue, kParamCabinet, kParamMix, kParamOutput, kParamOverdriveDrive, kParamOverdriveTone,
      kParamOverdriveBlend, kParamOverdriveLevel, kParamDistortionGain, kParamDistortionTone,
      kParamDistortionBlend, kParamDistortionLevel
    };
    const char* labels[24] = {
      "INPUT", "INPUT SHAPE", "LOW CROSS", "HIGH CROSS",
      "LOW TIGHT", "MID DRIVE", "HIGH DRIVE", "ATTACK",
      "BODY", "GROWL", "CLANK", "EXCITER",
      "GLUE", "CABINET", "MIX", "OUTPUT", "OD DRIVE", "OD TONE",
      "OD BLEND", "OD LEVEL", "DIST GAIN", "DIST TONE", "DIST BLEND", "DIST LEVEL"
    };
    const char* tooltips[24] = {
      "Input: Sets the level feeding the processor. Raise it for more saturation; lower it if the tone becomes compressed or harsh.",
      "Input Shape: Prepares dark or bright basses for the drive stages. Higher settings add more upper-mid definition before distortion.",
      "Low Crossover: Sets the boundary below which bass stays clean and tightly compressed.",
      "High Crossover: Sets where the controlled pick and upper-harmonic band begins.",
      "Low Tight: Compresses and stabilizes the low band without adding distortion.",
      "Mid Drive: Adds the dense growl and distortion that forms the core of a modern metal bass tone.",
      "High Drive: Adds controlled edge to the upper band. Increase carefully with noisy sources.",
      "Attack: Restores pick articulation by emphasizing the fast part of each note.",
      "Body: Adjusts weight and fullness around 180 Hz.",
      "Growl: Adjusts the central bass definition around 850 Hz.",
      "Clank: Adjusts upper-mid string detail around 2.4 kHz.",
      "Exciter: Creates additional harmonics from the mid band for audible definition on small speakers.",
      "Glue: Applies slow, musical full-band compression after the three bands are recombined.",
      "Cabinet: Blends an original speaker-like filter curve. It does not use a copied impulse response.",
      "Mix: Blends your clean bass signal with the complete processed modern-metal tone.",
      "Output: Sets final level before the built-in safety ceiling at approximately -1 dBFS.",
      "OD Drive: Sets the amount of soft, touch-sensitive asymmetric clipping in the overdrive stage.",
      "OD Tone: Sets the high-frequency reach of the overdriven signal. Lower values are warmer; higher values add pick definition.",
      "OD Blend: Mixes the overdrive branch with the unaffected low-end foundation. Set to zero to bypass overdrive.",
      "OD Level: Sets the overdrive branch level before it is blended with the core bass tone.",
      "Dist Gain: Sets the amount of harder, denser symmetric clipping in the distortion stage.",
      "Dist Tone: Sets the high-frequency reach of the distorted signal. Lower values reduce fizz; higher values add bite.",
      "Dist Blend: Mixes the distortion branch with its input. Set to zero to bypass distortion.",
      "Dist Level: Sets the distortion branch level before it is blended with the signal."
    };

    for (int index = 0; index < 24; ++index)
    {
      const int section = index / 4;
      const int controlInSection = index % 4;
      const int panelRow = section / 2;
      const int panelColumn = section % 2;
      const float panelX = 28.f + panelColumn * 580.f;
      const float panelY = 151.f + panelRow * 190.f;
      const float centerX = panelX + 72.f + controlInSection * 140.f;
      const float controlTop = panelY + 31.f;
      const IRECT bounds(centerX - 65.f, controlTop, centerX + 65.f, controlTop + 137.f);
      graphics->AttachControl(new MidnightKnobControl(bounds, parameters[index], labels[index]))
        ->SetTooltip(tooltips[index]);
    }
  };
#endif
}

#if IPLUG_DSP
sample ModernMetalBass::Biquad::Process(sample x)
{
  const double y = b0 * x + z1;
  z1 = b1 * x - a1 * y + z2;
  z2 = b2 * x - a2 * y;
  return static_cast<sample>(y);
}

void ModernMetalBass::Biquad::Reset()
{
  z1 = 0.0;
  z2 = 0.0;
}

void ModernMetalBass::Biquad::SetCoefficients(double nb0, double nb1, double nb2,
                                             double na0, double na1, double na2)
{
  const double inverseA0 = 1.0 / na0;
  b0 = nb0 * inverseA0;
  b1 = nb1 * inverseA0;
  b2 = nb2 * inverseA0;
  a1 = na1 * inverseA0;
  a2 = na2 * inverseA0;
}

void ModernMetalBass::Biquad::SetHighPass(double sampleRate, double frequency, double q)
{
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients((1.0 + cosine) * .5, -(1.0 + cosine), (1.0 + cosine) * .5,
                  1.0 + alpha, -2.0 * cosine, 1.0 - alpha);
}

void ModernMetalBass::Biquad::SetLowPass(double sampleRate, double frequency, double q)
{
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients((1.0 - cosine) * .5, 1.0 - cosine, (1.0 - cosine) * .5,
                  1.0 + alpha, -2.0 * cosine, 1.0 - alpha);
}

void ModernMetalBass::Biquad::SetPeak(double sampleRate, double frequency, double q, double gainDB)
{
  const double amplitude = std::pow(10.0, gainDB / 40.0);
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients(1.0 + alpha * amplitude, -2.0 * cosine, 1.0 - alpha * amplitude,
                  1.0 + alpha / amplitude, -2.0 * cosine, 1.0 - alpha / amplitude);
}

void ModernMetalBass::Biquad::SetHighShelf(double sampleRate, double frequency, double gainDB)
{
  const double amplitude = std::pow(10.0, gainDB / 40.0);
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double sine = std::sin(omega);
  const double beta = 2.0 * std::sqrt(amplitude) * sine / std::sqrt(2.0);
  const double ap1 = amplitude + 1.0;
  const double am1 = amplitude - 1.0;
  SetCoefficients(amplitude * (ap1 + am1 * cosine + beta),
                  -2.0 * amplitude * (am1 + ap1 * cosine),
                  amplitude * (ap1 + am1 * cosine - beta),
                  ap1 - am1 * cosine + beta,
                  2.0 * (am1 - ap1 * cosine),
                  ap1 - am1 * cosine - beta);
}

void ModernMetalBass::OnePoleLowPass::SetFrequency(double sampleRate, double frequency)
{
  coefficient = 1.0 - std::exp(-2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate);
}

sample ModernMetalBass::OnePoleLowPass::Process(sample x)
{
  state += coefficient * (static_cast<double>(x) - state);
  return static_cast<sample>(state);
}

void ModernMetalBass::ChannelState::Reset()
{
  inputHighPass.Reset();
  sourceBoom.Reset();
  sourceDefinition.Reset();
  sourcePresence.Reset();
  lowSplit.Reset();
  lowSplitSecond.Reset();
  highSplit.Reset();
  highSplitSecond.Reset();
  harmonicLowPass.Reset();
  definitionGrowl.Reset();
  definitionClank.Reset();
  definitionShelf.Reset();
  bodyEQ.Reset();
  growlEQ.Reset();
  clankEQ.Reset();
  cabinetBody.Reset();
  cabinetNotch.Reset();
  cabinetClank.Reset();
  cabinetTilt.Reset();
  cabinetLowPass.Reset();
  overdriveHighPass.Reset();
  overdriveTone.Reset();
  distortionHighPass.Reset();
  distortionTone.Reset();
  lowEnvelope = fastEnvelope = slowEnvelope = glueEnvelope = 0.0;
}

void ModernMetalBass::UpdateCoefficients(ChannelState& state)
{
  const double sampleRate = GetSampleRate();
  const double match = GetParam(kParamSourceMatch)->Value() / 100.0;
  state.inputHighPass.SetHighPass(sampleRate, 27.0, .707);
  state.sourceBoom.SetPeak(sampleRate, 72.0, .72, -3.5 * match);
  state.sourceDefinition.SetPeak(sampleRate, 1450.0, .62, 5.0 * match);
  state.sourcePresence.SetHighShelf(sampleRate, 2200.0, 4.0 * match);
  state.lowSplit.SetFrequency(sampleRate, GetParam(kParamLowCrossover)->Value());
  state.lowSplitSecond.SetFrequency(sampleRate, GetParam(kParamLowCrossover)->Value());
  state.highSplit.SetFrequency(sampleRate, GetParam(kParamHighCrossover)->Value());
  state.highSplitSecond.SetFrequency(sampleRate, GetParam(kParamHighCrossover)->Value());
  state.harmonicLowPass.SetFrequency(sampleRate, 230.0);
  state.definitionGrowl.SetPeak(sampleRate, 950.0, .72, 2.0);
  state.definitionClank.SetPeak(sampleRate, 2300.0, .68, 7.0);
  state.definitionShelf.SetHighShelf(sampleRate, 1250.0, 4.0);
  state.bodyEQ.SetPeak(sampleRate, 180.0, .70, GetParam(kParamBody)->Value());
  state.growlEQ.SetPeak(sampleRate, 850.0, .78, GetParam(kParamGrowl)->Value());
  state.clankEQ.SetPeak(sampleRate, 2400.0, .82, GetParam(kParamClank)->Value());
  state.cabinetBody.SetPeak(sampleRate, 220.0, .90, -6.0);
  state.cabinetNotch.SetPeak(sampleRate, 470.0, 2.60, -14.0);
  state.cabinetClank.SetPeak(sampleRate, 3600.0, 1.25, -1.5);
  state.cabinetTilt.SetHighShelf(sampleRate, 3600.0, -5.0);
  state.cabinetLowPass.SetLowPass(sampleRate, 7200.0, .58);
  state.overdriveHighPass.SetHighPass(sampleRate, 75.0, .707);
  state.overdriveTone.SetLowPass(sampleRate,
    900.0 * std::pow(8.0, GetParam(kParamOverdriveTone)->Value() / 100.0), .62);
  state.distortionHighPass.SetHighPass(sampleRate, 120.0, .707);
  state.distortionTone.SetLowPass(sampleRate,
    1100.0 * std::pow(7.0, GetParam(kParamDistortionTone)->Value() / 100.0), .60);
}

sample ModernMetalBass::Saturate(sample input, double amount, double bias)
{
  const double normalizedAmount = std::clamp(amount, 0.0, 1.0);
  const double drive = 1.0 + 28.0 * std::pow(normalizedAmount, 1.25);
  const double offset = bias * normalizedAmount;
  const double firstStage = std::tanh(static_cast<double>(input) * drive + offset)
    - std::tanh(offset);
  const double secondDrive = 1.15 + 2.85 * normalizedAmount;
  const double secondStage = std::tanh(firstStage * secondDrive) / std::tanh(secondDrive);
  const double compensation = 1.0 / (.72 + .45 * std::sqrt(drive));
  return static_cast<sample>((.76 * firstStage + .24 * secondStage) * compensation
                             + .035 * input);
}

sample ModernMetalBass::SafeLimit(sample input)
{
  constexpr double knee = .72;
  constexpr double ceiling = .891250938;
  const double value = static_cast<double>(input);
  const double magnitude = std::abs(value);
  if (magnitude <= knee)
    return input;

  const double range = ceiling - knee;
  const double limited = knee + range * (1.0 - std::exp(-(magnitude - knee) / range));
  return static_cast<sample>(std::copysign(std::min(limited, ceiling), value));
}

sample ModernMetalBass::ProcessOverdrive(ChannelState& state, sample input,
                                         const BlockParameters& parameters)
{
  // A conditioned, softly clipped parallel branch keeps the fundamental
  // intact while adding the asymmetric even/odd harmonic balance associated
  // with touch-sensitive overdrive.
  const double drive = 1.0 + 16.0 * std::pow(parameters.overdriveDrive, 1.35);
  const double bias = .045 + .085 * parameters.overdriveDrive;
  const double conditioned = static_cast<double>(state.overdriveHighPass.Process(input));
  const sample preservedLow = static_cast<sample>(static_cast<double>(input) - conditioned);
  const double clipped = std::tanh(conditioned * drive + bias) - std::tanh(bias);
  const double gainCompensation = .95 / std::sqrt(drive);
  const sample toned = state.overdriveTone.Process(
    static_cast<sample>(clipped * gainCompensation));
  const double level = DecibelsToGain(parameters.overdriveLevel);
  const sample effected = static_cast<sample>(preservedLow + toned * level);
  return static_cast<sample>(input + (effected - input) * parameters.overdriveBlend);
}

sample ModernMetalBass::ProcessDistortion(ChannelState& state, sample input,
                                          const BlockParameters& parameters)
{
  // The distortion branch uses a tighter input filter and a progressively
  // harder symmetric knee than the overdrive stage for dense modern bite.
  const double gain = 2.0 + 42.0 * std::pow(parameters.distortionGain, 1.25);
  const double highPassed = static_cast<double>(state.distortionHighPass.Process(input));
  const sample preservedLow = static_cast<sample>(static_cast<double>(input) - highPassed);
  const double conditioned = highPassed * gain;
  const double threshold = .78 - .43 * parameters.distortionGain;
  const double hard = std::clamp(conditioned, -threshold, threshold) / threshold;
  const double soft = std::tanh(conditioned * 1.35);
  const double hardness = .35 + .55 * parameters.distortionGain;
  const double gainCompensation = 1.10 / std::sqrt(gain);
  const sample clipped = static_cast<sample>(
    (soft + (hard - soft) * hardness) * gainCompensation);
  const sample toned = state.distortionTone.Process(clipped);
  const double level = DecibelsToGain(parameters.distortionLevel);
  const sample effected = static_cast<sample>(preservedLow + toned * level);
  return static_cast<sample>(input + (effected - input) * parameters.distortionBlend);
}

sample ModernMetalBass::ProcessWet(ChannelState& state, sample input,
                                  const BlockParameters& parameters)
{
  const double sampleRate = GetSampleRate();
  sample matched = state.inputHighPass.Process(input);
  matched = state.sourcePresence.Process(state.sourceDefinition.Process(state.sourceBoom.Process(matched)));

  const sample low = state.lowSplitSecond.Process(state.lowSplit.Process(matched));
  const sample aboveLow = matched - low;
  const sample mid = state.highSplitSecond.Process(state.highSplit.Process(aboveLow));
  const sample high = aboveLow - mid;

  const double lowAttack = EnvelopeCoefficient(sampleRate, 3.0);
  const double lowRelease = EnvelopeCoefficient(sampleRate, 180.0);
  const double lowLevel = std::abs(static_cast<double>(low));
  const double lowCoefficient = lowLevel > state.lowEnvelope ? lowAttack : lowRelease;
  state.lowEnvelope = lowCoefficient * state.lowEnvelope + (1.0 - lowCoefficient) * lowLevel;
  const double lowThresholdDB = -11.0 - 10.0 * parameters.lowTight;
  const double lowRatio = 1.0 + 4.0 * parameters.lowTight;
  const double lowEnvelopeDB = 20.0 * std::log10(std::max(state.lowEnvelope, 1.0e-9));
  const double lowReductionDB = lowEnvelopeDB > lowThresholdDB
    ? (lowThresholdDB + (lowEnvelopeDB - lowThresholdDB) / lowRatio) - lowEnvelopeDB
    : 0.0;
  const sample controlledLow = static_cast<sample>(low * DecibelsToGain(lowReductionDB) * .92);

  const double absoluteInput = std::abs(static_cast<double>(matched));
  const double fastCoefficient = absoluteInput > state.fastEnvelope
    ? EnvelopeCoefficient(sampleRate, .25) : EnvelopeCoefficient(sampleRate, 18.0);
  const double slowCoefficient = absoluteInput > state.slowEnvelope
    ? EnvelopeCoefficient(sampleRate, 18.0) : EnvelopeCoefficient(sampleRate, 220.0);
  state.fastEnvelope = fastCoefficient * state.fastEnvelope + (1.0 - fastCoefficient) * absoluteInput;
  state.slowEnvelope = slowCoefficient * state.slowEnvelope + (1.0 - slowCoefficient) * absoluteInput;
  const double transient = std::clamp((state.fastEnvelope - state.slowEnvelope) * 18.0, 0.0, 2.0);
  const double attackGain = 1.0 + parameters.attack * transient;

  const sample drivenMid = Saturate(static_cast<sample>(mid * attackGain), parameters.midDrive, .055);
  const sample drivenHigh = Saturate(static_cast<sample>(high * attackGain), parameters.highDrive, -.035);
  const sample harmonicDifference = Saturate(mid, parameters.exciter, .07) - mid;
  const sample midTexture = static_cast<sample>(mid * (.34 - .18 * parameters.midDrive)
    + drivenMid * (.92 + .38 * parameters.midDrive));
  const sample excitedHigh = static_cast<sample>(high * (.28 - .12 * parameters.highDrive)
    + drivenHigh * (.82 + .28 * parameters.highDrive)
    + harmonicDifference * (.04 + .42 * parameters.exciter));

  // Generate definition from the fundamental notes before removing their
  // low-frequency content. This preserves pick clarity without boosting hiss.
  const double definitionDrive = .24 + .56 * parameters.midDrive;
  const sample fullBandHarmonics = Saturate(matched, definitionDrive, .065);
  sample generatedDefinition = fullBandHarmonics - state.harmonicLowPass.Process(fullBandHarmonics);
  generatedDefinition = state.definitionShelf.Process(
    state.definitionClank.Process(state.definitionGrowl.Process(generatedDefinition)));
  const double definitionAmount = .18 + .48 * parameters.sourceMatch;

  const sample drivenBands = static_cast<sample>(midTexture * .42 + excitedHigh * 1.16
                                                  + generatedDefinition * definitionAmount);
  sample combined = static_cast<sample>(controlledLow + drivenBands * .20);
  combined = state.clankEQ.Process(state.growlEQ.Process(state.bodyEQ.Process(combined)));
  combined = ProcessOverdrive(state, combined, parameters);
  combined = ProcessDistortion(state, combined, parameters);

  const sample cabinetFiltered = state.cabinetLowPass.Process(
    state.cabinetTilt.Process(state.cabinetClank.Process(
      state.cabinetNotch.Process(state.cabinetBody.Process(combined)))));
  combined = static_cast<sample>(combined + (cabinetFiltered - combined) * parameters.cabinet);

  const double glueLevel = std::abs(static_cast<double>(combined));
  const double glueCoefficient = glueLevel > state.glueEnvelope
    ? EnvelopeCoefficient(sampleRate, .2 + 3.0 * (1.0 - parameters.glue))
    : EnvelopeCoefficient(sampleRate, 350.0 + 1850.0 * parameters.glue);
  state.glueEnvelope = glueCoefficient * state.glueEnvelope + (1.0 - glueCoefficient) * glueLevel;
  const double thresholdDB = -9.0 - 11.0 * parameters.glue;
  const double ratio = 1.0 + 1.7 * parameters.glue;
  const double envelopeDB = 20.0 * std::log10(std::max(state.glueEnvelope, 1.0e-9));
  const double reductionDB = envelopeDB > thresholdDB
    ? (thresholdDB + (envelopeDB - thresholdDB) / ratio) - envelopeDB
    : 0.0;
  const double makeupDB = 2.5 * parameters.glue;
  return static_cast<sample>(combined * DecibelsToGain(reductionDB + makeupDB));
}

void ModernMetalBass::OnReset()
{
  const double sampleRate = GetSampleRate();
  mInputSmoother.SetSmoothTime(20.0, sampleRate);
  mMixSmoother.SetSmoothTime(15.0, sampleRate);
  mOutputSmoother.SetSmoothTime(20.0, sampleRate);
  mInputSmoother.SetValue(static_cast<sample>(DecibelsToGain(GetParam(kParamInput)->Value())));
  mMixSmoother.SetValue(static_cast<sample>(GetParam(kParamMix)->Value() / 100.0));
  mOutputSmoother.SetValue(static_cast<sample>(DecibelsToGain(GetParam(kParamOutput)->Value())));

  for (auto& state : mChannelStates)
  {
    state.Reset();
    UpdateCoefficients(state);
  }
}

void ModernMetalBass::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int channelCount = std::min({NInChansConnected(), NOutChansConnected(),
                                     static_cast<int>(mChannelStates.size())});
  const double inputTarget = DecibelsToGain(GetParam(kParamInput)->Value());
  const double mixTarget = GetParam(kParamMix)->Value() / 100.0;
  const double outputTarget = DecibelsToGain(GetParam(kParamOutput)->Value());
  const BlockParameters parameters {
    GetParam(kParamSourceMatch)->Value() / 100.0,
    GetParam(kParamLowTight)->Value() / 100.0,
    GetParam(kParamMidDrive)->Value() / 100.0,
    GetParam(kParamHighDrive)->Value() / 100.0,
    GetParam(kParamAttack)->Value() / 100.0,
    GetParam(kParamGlue)->Value() / 100.0,
    GetParam(kParamExciter)->Value() / 100.0,
    GetParam(kParamCabinet)->Value() / 100.0,
    GetParam(kParamOverdriveDrive)->Value() / 100.0,
    GetParam(kParamOverdriveTone)->Value() / 100.0,
    GetParam(kParamOverdriveBlend)->Value() / 100.0,
    GetParam(kParamOverdriveLevel)->Value(),
    GetParam(kParamDistortionGain)->Value() / 100.0,
    GetParam(kParamDistortionTone)->Value() / 100.0,
    GetParam(kParamDistortionBlend)->Value() / 100.0,
    GetParam(kParamDistortionLevel)->Value()
  };

  for (int channel = 0; channel < channelCount; ++channel)
    UpdateCoefficients(mChannelStates[channel]);

  for (int frame = 0; frame < nFrames; ++frame)
  {
    const sample inputGain = mInputSmoother.Process(static_cast<sample>(inputTarget));
    const sample mix = mMixSmoother.Process(static_cast<sample>(mixTarget));
    const sample outputGain = mOutputSmoother.Process(static_cast<sample>(outputTarget));

    for (int channel = 0; channel < channelCount; ++channel)
    {
      const sample dry = static_cast<sample>(inputs[channel][frame] * inputGain);
      const sample wet = ProcessWet(mChannelStates[channel], dry, parameters);
      const sample blended = static_cast<sample>(dry + (wet - dry) * mix);
      outputs[channel][frame] = SafeLimit(static_cast<sample>(blended * outputGain));
    }
  }

  for (int channel = channelCount; channel < NOutChansConnected(); ++channel)
    std::fill(outputs[channel], outputs[channel] + nFrames, 0.0);
}
#endif
