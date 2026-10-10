#include "DualBassPreamp.h"
#include "IPlug_include_in_plug_src.h"

#include <algorithm>
#include <vector>

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

#if IPLUG_EDITOR
class BlueSteelBackgroundControl final : public IControl
{
public:
  explicit BlueSteelBackgroundControl(const IRECT& bounds)
  : IControl(bounds)
  {
    SetIgnoreMouse(true);
  }

  void Draw(IGraphics& g) override
  {
    const float sx = mRECT.W() / 900.f;
    const float sy = mRECT.H() / 560.f;
    const float scale = std::min(sx, sy);
    const auto R = [this, sx, sy](float x, float y, float w, float h) {
      return IRECT(mRECT.L + x * sx, mRECT.T + y * sy,
                   mRECT.L + (x + w) * sx, mRECT.T + (y + h) * sy);
    };

    const IColor cream(255, 235, 220, 190);
    const IColor blue(255, 23, 139, 235);
    const IColor edge(255, 26, 113, 178);
    const IColor panel(255, 14, 20, 25);
    const IColor shadow(175, 0, 3, 7);

    // Layered cobalt gradients and very fine streaks create the brushed,
    // anodised-steel faceplate of the selected concept.
    g.PathRect(mRECT);
    g.PathFill(IPattern::CreateLinearGradient(mRECT, EDirection::Vertical,
      {{IColor(255, 8, 42, 72), 0.f}, {IColor(255, 10, 57, 94), 0.42f},
       {IColor(255, 5, 35, 59), 1.f}}));

    for (int line = 0; line < 72; ++line)
    {
      const float y = mRECT.T + (5.f + line * 7.8f) * sy;
      const IColor grain = (line % 3 == 0) ? IColor(30, 99, 178, 226) : IColor(22, 0, 9, 17);
      g.DrawLine(grain, mRECT.L, y, mRECT.R, y, nullptr, 0.45f * scale);
    }

    // Header identity and an original signal-mark replace the tuner from the
    // visual concept. No tuner or pitch-analysis process exists in this build.
    g.DrawText(IText(17.f * scale, cream, "Roboto-Regular", EAlign::Near),
               "▮▮▮  PLAY  TEMPO", R(28, 18, 330, 24));
    g.DrawText(IText(30.f * scale, cream, "Roboto-Regular", EAlign::Near),
               "DUAL BASS ENGINE", R(28, 43, 470, 42));
    g.DrawLine(cream, R(28, 91, 110, 1).L, R(28, 91, 110, 1).T,
               R(28, 91, 110, 1).R, R(28, 91, 110, 1).T, nullptr, 1.2f * scale);
    g.DrawLine(IColor(255, 236, 71, 43), R(144, 91, 18, 1).L, R(144, 91, 18, 1).T,
               R(144, 91, 18, 1).R, R(144, 91, 18, 1).T, nullptr, 2.f * scale);
    g.DrawLine(cream, R(168, 91, 110, 1).L, R(168, 91, 110, 1).T,
               R(168, 91, 110, 1).R, R(168, 91, 110, 1).T, nullptr, 1.2f * scale);

    const IRECT signalPlate = R(340, 20, 250, 72);
    g.FillRoundRect(shadow, signalPlate.GetTranslated(0.f, 3.f * sy), 10.f * scale);
    g.FillRoundRect(IColor(255, 7, 13, 18), signalPlate, 10.f * scale);
    g.DrawRoundRect(IColor(255, 63, 77, 88), signalPlate, 10.f * scale, nullptr, 1.2f * scale);
    const int heights[] = {14, 25, 38, 52, 66, 52, 38, 25, 14};
    for (int i = 0; i < 9; ++i)
    {
      const float x = signalPlate.L + (73.f + i * 13.f) * sx;
      const float h = heights[i] * 0.55f * sy;
      g.DrawLine(i == 4 ? IColor(255, 75, 181, 255) : IColor(255, 27, 112, 189),
                 x, signalPlate.MH() - h * 0.5f, x, signalPlate.MH() + h * 0.5f,
                 nullptr, (i == 4 ? 3.f : 2.f) * scale);
    }
    g.DrawText(IText(8.5f * scale, IColor(255, 125, 153, 172), "Roboto-Regular", EAlign::Center),
               "TWO VOICES  •  ONE ENGINE", R(350, 70, 230, 14));

    // Recessed dual voice chassis.
    const IRECT leftPanel = R(20, 132, 386, 150);
    const IRECT rightPanel = R(494, 132, 386, 150);
    for (const IRECT& card : {leftPanel, rightPanel})
    {
      g.FillRoundRect(shadow, card.GetTranslated(0.f, 4.f * sy), 10.f * scale);
      g.FillRoundRect(panel, card, 10.f * scale);
      g.DrawRoundRect(IColor(255, 0, 5, 8), card.GetPadded(-2.f * scale), 8.f * scale, nullptr, 2.f * scale);
      g.DrawRoundRect(edge, card, 10.f * scale, nullptr, 1.4f * scale);

      const float screwR = 4.2f * scale;
      for (const auto& p : {IRECT(card.L + 13.f * sx, card.T + 13.f * sy, card.L + 13.f * sx, card.T + 13.f * sy),
                            IRECT(card.R - 13.f * sx, card.T + 13.f * sy, card.R - 13.f * sx, card.T + 13.f * sy),
                            IRECT(card.L + 13.f * sx, card.B - 13.f * sy, card.L + 13.f * sx, card.B - 13.f * sy),
                            IRECT(card.R - 13.f * sx, card.B - 13.f * sy, card.R - 13.f * sx, card.B - 13.f * sy)})
      {
        g.FillCircle(IColor(255, 3, 6, 8), p.L, p.T, screwR + 1.5f * scale);
        g.FillCircle(IColor(255, 48, 54, 57), p.L, p.T, screwR);
        g.DrawLine(IColor(255, 8, 10, 11), p.L - 2.2f * scale, p.T + 2.2f * scale,
                   p.L + 2.2f * scale, p.T - 2.2f * scale, nullptr, 1.f * scale);
      }
    }

    const auto DrawVoiceTab = [&](const IRECT& tab, const char* textValue) {
      g.FillRoundRect(IColor(255, 8, 18, 26), tab.GetTranslated(0.f, 2.f * sy), 5.f * scale);
      g.FillRoundRect(IColor(255, 10, 69, 122), tab, 5.f * scale);
      g.DrawRoundRect(blue, tab, 5.f * scale, nullptr, 1.f * scale);
      g.DrawText(IText(14.f * scale, IColor(255, 232, 236, 239), "Roboto-Regular", EAlign::Center),
                 textValue, tab);
    };
    DrawVoiceTab(R(158, 125, 110, 25), "VOICE A");
    DrawVoiceTab(R(632, 125, 110, 25), "VOICE B");

    // Red channel status lamps and centre signal emblem.
    for (float x : {216.f, 690.f})
    {
      const float px = mRECT.L + x * sx;
      const float py = mRECT.T + 210.f * sy;
      g.DrawLine(IColor(255, 61, 69, 73), px, py - 38.f * sy, px, py - 10.f * sy, nullptr, 1.f * scale);
      g.DrawLine(IColor(255, 61, 69, 73), px, py + 10.f * sy, px, py + 38.f * sy, nullptr, 1.f * scale);
      g.FillCircle(IColor(90, 255, 42, 21), px, py, 9.f * scale);
      g.FillCircle(IColor(255, 117, 19, 10), px, py, 5.f * scale);
      g.FillCircle(IColor(255, 255, 216, 183), px - 1.f * scale, py - 1.5f * scale, 1.5f * scale);
    }
    for (int i = 0; i < 7; ++i)
    {
      const float h = (i < 4 ? 10.f + i * 7.f : 10.f + (6 - i) * 7.f) * sy;
      const float x = mRECT.L + (430.f + i * 6.5f) * sx;
      g.DrawLine(IColor(255, 42, 133, 219), x, mRECT.T + 210.f * sy - h * .5f,
                 x, mRECT.T + 210.f * sy + h * .5f, nullptr, 2.f * scale);
    }

    // EQ shelf and lower utility strip.
    g.FillRect(IColor(222, 7, 39, 66), R(0, 292, 900, 111));
    g.DrawLine(IColor(255, 66, 144, 199), mRECT.L, mRECT.T + 292.f * sy,
               mRECT.R, mRECT.T + 292.f * sy, nullptr, 1.f * scale);
    g.DrawLine(IColor(255, 22, 111, 183), mRECT.L, mRECT.T + 402.f * sy,
               mRECT.R, mRECT.T + 402.f * sy, nullptr, 1.4f * scale);
    for (float x : {225.f, 450.f, 675.f})
      g.DrawLine(IColor(180, 31, 112, 169), mRECT.L + x * sx, mRECT.T + 306.f * sy,
                 mRECT.L + x * sx, mRECT.T + 390.f * sy, nullptr, 1.f * scale);

    g.FillRect(IColor(237, 5, 28, 47), R(0, 403, 900, 107));
    for (float x : {210.f, 440.f, 665.f})
      g.DrawLine(IColor(125, 31, 112, 169), mRECT.L + x * sx, mRECT.T + 416.f * sy,
                 mRECT.L + x * sx, mRECT.T + 497.f * sy, nullptr, .8f * scale);

    // Vented footer and the promised discoverability note.
    const IRECT footer = R(0, 510, 900, 50);
    g.PathRect(footer);
    g.PathFill(IPattern::CreateLinearGradient(footer, EDirection::Vertical,
      {{IColor(255, 12, 17, 21), 0.f}, {IColor(255, 3, 7, 9), 1.f}}));
    for (int row = 0; row < 3; ++row)
      for (int col = 0; col < 20; ++col)
      {
        const float x1 = mRECT.L + (14.f + col * 10.f) * sx;
        const float x2 = mRECT.L + (706.f + col * 10.f) * sx;
        const float y = mRECT.T + (522.f + row * 9.f) * sy;
        g.FillCircle(IColor(255, 0, 2, 3), x1, y, 1.7f * scale);
        g.FillCircle(IColor(255, 0, 2, 3), x2, y, 1.7f * scale);
      }
    g.DrawLine(IColor(255, 24, 117, 189), mRECT.L + 225.f * sx, mRECT.T + 535.f * sy,
               mRECT.L + 296.f * sx, mRECT.T + 535.f * sy, nullptr, 1.f * scale);
    g.DrawLine(IColor(255, 24, 117, 189), mRECT.L + 604.f * sx, mRECT.T + 535.f * sy,
               mRECT.L + 675.f * sx, mRECT.T + 535.f * sy, nullptr, 1.f * scale);
    g.DrawText(IText(10.5f * scale, cream, "Roboto-Regular", EAlign::Center),
               "Hover over any control to learn what it does.", R(300, 522, 300, 25));
    g.DrawText(IText(8.f * scale, IColor(255, 104, 128, 143), "Roboto-Regular", EAlign::Far),
               "v0.3.1", R(815, 542, 68, 12));
  }
};

class BlueSteelKnobControl final : public IKnobControlBase
{
public:
  BlueSteelKnobControl(const IRECT& bounds, int paramIdx, const char* label,
                       const char* subLabel, const char* lowText, const char* highText,
                       bool large = false)
  : IKnobControlBase(bounds, paramIdx)
  , mLabel(label)
  , mSubLabel(subLabel)
  , mLowText(lowText)
  , mHighText(highText)
  , mLarge(large)
  {}

  void Draw(IGraphics& g) override
  {
    const float s = std::min(mRECT.W() / (mLarge ? 145.f : 130.f),
                             mRECT.H() / (mLarge ? 132.f : 102.f));
    const IColor cream(255, 235, 220, 190);
    const IColor blue(255, 30, 139, 235);
    const float labelH = (mSubLabel && *mSubLabel) ? 15.f * s : 20.f * s;
    g.DrawText(IText((mLarge ? 12.f : 11.f) * s, cream, "Roboto-Regular", EAlign::Center),
               mLabel, IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.T + labelH));
    if (mSubLabel && *mSubLabel)
      g.DrawText(IText(10.5f * s, blue, "Roboto-Regular", EAlign::Center),
                 mSubLabel, IRECT(mRECT.L, mRECT.T + 14.f * s, mRECT.R, mRECT.T + 30.f * s));

    const float centerY = mRECT.T + (mLarge ? 75.f : 62.f) * s;
    const float cx = mRECT.MW();
    const float radius = (mLarge ? 31.f : 25.f) * s;
    const float tickR = radius + 8.f * s;

    for (int i = 0; i < 11; ++i)
    {
      const float a = -135.f + i * 27.f;
      const float rad = DegToRad(a);
      const float tx = cx + std::sin(rad) * tickR;
      const float ty = centerY - std::cos(rad) * tickR;
      g.FillCircle(cream, tx, ty, (i == 0 || i == 5 || i == 10) ? 1.5f * s : 1.05f * s);
    }

    g.FillCircle(IColor(160, 0, 2, 4), cx + 2.f * s, centerY + 4.f * s, radius + 6.f * s);
    g.FillCircle(IColor(255, 2, 5, 7), cx, centerY, radius + 6.f * s);
    g.FillCircle(IColor(255, 64, 60, 51), cx, centerY, radius + 3.f * s);
    g.FillCircle(IColor(255, 10, 12, 13), cx, centerY, radius + 1.f * s);

    g.PathCircle(cx, centerY, radius);
    g.PathFill(IPattern::CreateLinearGradient(cx - radius, centerY - radius,
                                              cx + radius, centerY + radius,
      {{IColor(255, 69, 74, 78), 0.f}, {IColor(255, 226, 226, 219), .28f},
       {IColor(255, 115, 117, 116), .52f}, {IColor(255, 244, 242, 229), .72f},
       {IColor(255, 58, 61, 63), 1.f}}));
    g.DrawCircle(IColor(255, 223, 215, 194), cx, centerY, radius, nullptr, 1.f * s);

    // Fine radial machining lines make the vector knob read as brushed metal.
    for (int i = 0; i < 22; ++i)
    {
      const float a = i * (360.f / 22.f);
      g.DrawRadialLine(IColor(22, 255, 255, 255), cx, centerY, a,
                       radius * .15f, radius * .92f, nullptr, .45f * s);
    }

    const float valueAngle = -135.f + static_cast<float>(GetValue()) * 270.f;
    g.DrawRadialLine(IColor(190, 0, 4, 10), cx + 1.f * s, centerY + 1.f * s,
                     valueAngle, radius * .25f, radius * .82f, nullptr, 4.f * s);
    g.DrawRadialLine(IColor(255, 8, 94, 217), cx, centerY,
                     valueAngle, radius * .25f, radius * .82f, nullptr, 3.f * s);
    g.DrawRadialLine(IColor(255, 148, 216, 255), cx, centerY,
                     valueAngle, radius * .34f, radius * .80f, nullptr, 1.f * s);

    const float textY = centerY + radius + 7.f * s;
    g.DrawText(IText(8.5f * s, cream, "Roboto-Regular", EAlign::Near), mLowText,
               IRECT(cx - tickR, textY, cx - 2.f * s, textY + 13.f * s));
    g.DrawText(IText(8.5f * s, cream, "Roboto-Regular", EAlign::Far), mHighText,
               IRECT(cx + 2.f * s, textY, cx + tickR, textY + 13.f * s));
  }

private:
  const char* mLabel;
  const char* mSubLabel;
  const char* mLowText;
  const char* mHighText;
  bool mLarge;
};

class BlueVoiceSwitchControl final : public ISwitchControlBase
{
public:
  BlueVoiceSwitchControl(const IRECT& bounds, int paramIdx)
  : ISwitchControlBase(bounds, paramIdx, nullptr, 2)
  {}

  void Draw(IGraphics& g) override
  {
    const float s = std::min(mRECT.W() / 210.f, mRECT.H() / 74.f);
    const IColor cream(255, 235, 220, 190);
    const IRECT title(mRECT.L, mRECT.T, mRECT.R, mRECT.T + 19.f * s);
    const IRECT shell(mRECT.L, mRECT.T + 23.f * s, mRECT.R, mRECT.B);
    const IRECT inner = shell.GetPadded(-5.f * s);
    const float half = inner.W() * .5f;
    const int selected = GetSelectedIdx();

    g.DrawText(IText(10.f * s, cream, "Roboto-Regular", EAlign::Center), "ACTIVE VOICE", title);
    g.FillRoundRect(IColor(145, 0, 3, 6), shell.GetTranslated(0.f, 3.f * s), 25.f * s);
    g.FillRoundRect(IColor(255, 42, 47, 49), shell, 25.f * s);
    g.FillRoundRect(IColor(255, 5, 10, 14), inner, 21.f * s);

    for (int i = 0; i < 2; ++i)
    {
      IRECT segment(inner.L + i * half, inner.T, inner.L + (i + 1) * half, inner.B);
      segment = segment.GetPadded(2.f * s);
      if (i == selected)
      {
        g.FillRoundRect(IColor(75, 38, 168, 255), segment.GetPadded(3.f * s), 18.f * s);
        g.PathRoundRect(segment, 18.f * s);
        g.PathFill(IPattern::CreateLinearGradient(segment, EDirection::Vertical,
          {{IColor(255, 31, 181, 255), 0.f}, {IColor(255, 6, 75, 193), 1.f}}));
        g.DrawRoundRect(IColor(255, 135, 227, 255), segment, 18.f * s, nullptr, 1.2f * s);
      }
      else
        g.DrawRoundRect(IColor(255, 74, 82, 83), segment, 18.f * s, nullptr, 1.f * s);

      g.DrawText(IText(20.f * s, i == selected ? IColor(255, 242, 245, 247) : cream,
                       "Roboto-Regular", EAlign::Center),
                 i == 0 ? "A" : "B", segment);
    }
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    SetValue(x < mRECT.MW() ? 0.0 : 1.0);
    SetDirty(true);
  }
};

class BlueSteelToggleControl final : public ISwitchControlBase
{
public:
  BlueSteelToggleControl(const IRECT& bounds, int paramIdx)
  : ISwitchControlBase(bounds, paramIdx, nullptr, 2)
  {}

  void Draw(IGraphics& g) override
  {
    const float s = std::min(mRECT.W() / 145.f, mRECT.H() / 90.f);
    const IColor cream(255, 235, 220, 190);
    const IColor orange(255, 255, 164, 44);
    const float cx = mRECT.MW();
    const float cy = mRECT.T + 59.f * s;

    g.DrawText(IText(11.f * s, cream, "Roboto-Regular", EAlign::Center), "ATTACK",
               IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.T + 17.f * s));
    g.FillCircle(IColor(75, 255, 143, 28), cx, mRECT.T + 27.f * s, 8.f * s);
    g.FillCircle(GetValue() > .5 ? orange : IColor(255, 95, 61, 22),
                 cx, mRECT.T + 27.f * s, 4.f * s);

    const IRECT base(cx - 31.f * s, cy - 14.f * s, cx + 31.f * s, cy + 17.f * s);
    g.FillRoundRect(IColor(255, 3, 7, 9), base, 15.f * s);
    g.DrawRoundRect(IColor(255, 66, 72, 73), base, 15.f * s, nullptr, 1.f * s);
    const float lean = GetValue() > .5 ? 9.f * s : -9.f * s;
    g.DrawLine(IColor(255, 27, 24, 20), cx, cy + 4.f * s, cx + lean, cy - 18.f * s, nullptr, 7.f * s);
    g.DrawLine(IColor(255, 207, 200, 181), cx, cy + 2.f * s, cx + lean, cy - 18.f * s, nullptr, 4.f * s);
    g.FillCircle(IColor(255, 235, 226, 203), cx + lean, cy - 18.f * s, 4.f * s);
    g.DrawText(IText(8.f * s, cream, "Roboto-Regular", EAlign::Near), "SOFT",
               IRECT(mRECT.L, cy + 16.f * s, cx - 8.f * s, cy + 31.f * s));
    g.DrawText(IText(8.f * s, cream, "Roboto-Regular", EAlign::Far), "HARD",
               IRECT(cx + 8.f * s, cy + 16.f * s, mRECT.R, cy + 31.f * s));
  }
};

class PhotoKnobControl final : public IKnobControlBase
{
public:
  PhotoKnobControl(const IRECT& bounds, const IBitmap& bitmap, int paramIdx)
  : IKnobControlBase(bounds, paramIdx)
  , mBitmap(bitmap)
  {}

  void Draw(IGraphics& g) override
  {
    g.DrawFittedBitmap(mBitmap, mRECT);

    const float radius = std::min(mRECT.W(), mRECT.H()) * 0.5f;
    const float angle = -135.f + static_cast<float>(GetValue()) * 270.f;
    const float cx = mRECT.MW();
    const float cy = mRECT.MH();
    g.DrawRadialLine(IColor(190, 0, 3, 8), cx + 1.f, cy + 1.f, angle,
                     radius * .18f, radius * .76f, nullptr, 4.4f);
    g.DrawRadialLine(IColor(255, 4, 80, 205), cx, cy, angle,
                     radius * .18f, radius * .76f, nullptr, 3.2f);
    g.DrawRadialLine(IColor(255, 160, 225, 255), cx, cy, angle,
                     radius * .28f, radius * .73f, nullptr, 1.1f);
  }

private:
  IBitmap mBitmap;
};

class PhotoPanelControl final : public IControl
{
public:
  PhotoPanelControl(const IRECT& bounds, const IBitmap& bitmap)
  : IControl(bounds)
  , mBitmap(bitmap)
  {
    SetIgnoreMouse(true);
  }

  void Draw(IGraphics& g) override
  {
    g.DrawFittedBitmap(mBitmap, mRECT);
  }

private:
  IBitmap mBitmap;
};

class PhotoVoiceSwitchControl final : public ISwitchControlBase
{
public:
  PhotoVoiceSwitchControl(const IRECT& bounds, int paramIdx)
  : ISwitchControlBase(bounds, paramIdx, nullptr, 2)
  {}

  void Draw(IGraphics& g) override
  {
    const float radius = mRECT.H() * .48f;
    const IRECT inner = mRECT.GetPadded(-3.f);
    const float half = inner.W() * .5f;

    // This opaque neutral shell hides the selected state baked into the
    // photorealistic background. The live selection is then drawn above it.
    g.FillRoundRect(IColor(255, 21, 25, 26), mRECT, radius);
    g.DrawRoundRect(IColor(255, 104, 104, 92), mRECT, radius, nullptr, 1.2f);

    for (int i = 0; i < 2; ++i)
    {
      IRECT segment(inner.L + i * half, inner.T,
                    inner.L + (i + 1) * half, inner.B);
      segment = segment.GetPadded(1.5f);
      if (i == GetSelectedIdx())
      {
        g.FillRoundRect(IColor(100, 19, 153, 255), segment.GetPadded(2.f), radius);
        g.PathRoundRect(segment, radius);
        g.PathFill(IPattern::CreateLinearGradient(segment, EDirection::Vertical,
          {{IColor(255, 33, 190, 255), 0.f}, {IColor(255, 4, 61, 182), 1.f}}));
        g.DrawRoundRect(IColor(255, 145, 229, 255), segment, radius, nullptr, 1.2f);
      }
      else
      {
        g.FillRoundRect(IColor(255, 16, 23, 27), segment, radius);
        g.DrawRoundRect(IColor(255, 73, 78, 75), segment, radius, nullptr, .8f);
      }

      g.DrawText(IText(22.f, i == GetSelectedIdx() ? IColor(255, 245, 242, 231)
                                                   : IColor(255, 190, 178, 154),
                       "Roboto-Regular", EAlign::Center),
                 i == 0 ? "A" : "B", segment);
    }
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    SetValue(x < mRECT.MW() ? 0.0 : 1.0);
    SetDirty(true);
  }
};

class PhotoAttackToggleControl final : public ISwitchControlBase
{
public:
  PhotoAttackToggleControl(const IRECT& bounds, int paramIdx)
  : ISwitchControlBase(bounds, paramIdx, nullptr, 2)
  {}

  void Draw(IGraphics& g) override
  {
    const float cx = mRECT.MW();
    const float ledY = mRECT.T + 9.f;
    const float baseY = mRECT.T + 43.f;
    const IRECT base(cx - 31.f, baseY - 15.f, cx + 31.f, baseY + 16.f);

    g.FillCircle(IColor(80, 255, 142, 28), cx, ledY, 9.f);
    g.FillCircle(GetValue() > .5 ? IColor(255, 255, 174, 53)
                                 : IColor(255, 105, 65, 22), cx, ledY, 4.2f);
    g.FillRoundRect(IColor(255, 4, 8, 10), base, 16.f);
    g.DrawRoundRect(IColor(255, 77, 79, 73), base, 16.f, nullptr, 1.f);

    const float lean = GetValue() > .5 ? 9.f : -9.f;
    g.DrawLine(IColor(255, 27, 24, 20), cx, baseY + 4.f,
               cx + lean, baseY - 20.f, nullptr, 7.f);
    g.DrawLine(IColor(255, 214, 207, 187), cx, baseY + 2.f,
               cx + lean, baseY - 20.f, nullptr, 4.f);
    g.FillCircle(IColor(255, 238, 229, 207), cx + lean, baseY - 20.f, 4.2f);
  }
};
#endif
}

DualBassPreamp::DualBassPreamp(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamChannel)->InitEnum("Voice", 0, {"Voice A", "Voice B"});
  GetParam(kParamGain1)->InitPercentage("Drive A", 38.0);
  GetParam(kParamLevel1)->InitGain("Output A", 0.0, -24.0, 12.0, 0.1);
  GetParam(kParamGain2)->InitPercentage("Drive B", 62.0);
  GetParam(kParamLevel2)->InitGain("Output B", 1.5, -24.0, 12.0, 0.1);
  GetParam(kParamLow)->InitGain("Bass 50 Hz", 0.0, -18.0, 18.0, 0.1);
  GetParam(kParamMid1)->InitGain("Body 200 Hz", 0.0, -18.0, 18.0, 0.1);
  GetParam(kParamMid2)->InitGain("Punch 500 Hz", 0.0, -18.0, 18.0, 0.1);
  GetParam(kParamHigh)->InitGain("Presence 3 kHz", 0.0, -18.0, 18.0, 0.1);
  GetParam(kParamBlend)->InitPercentage("Character Mix", 100.0);
  GetParam(kParamBite)->InitBool("Attack", false);
  GetParam(kParamCabSim)->InitBool("Legacy Speaker Switch", true, "", IParam::kFlagCannotAutomate);
  GetParam(kParamBypass)->InitBool("Legacy Internal Bypass", false, "", IParam::kFlagCannotAutomate);
  GetParam(kParamInput)->InitGain("Input", 0.0, -18.0, 18.0, 0.1);
  GetParam(kParamCabinet)->InitPercentage("Cabinet", 70.0);

  MakePreset("Wide Clean", 0, 18.0, 0.0, 40.0, 1.0, 2.0, 1.0, -1.0, 1.0, 55.0, 0, 1, 0, 0.0, 55.0);
  MakePreset("Tight Attack", 0, 48.0, 0.0, 68.0, 2.0, 1.0, 2.0, 4.0, 2.0, 100.0, 1, 1, 0, -1.0, 70.0);
  MakePreset("Vintage Round", 0, 32.0, 0.0, 52.0, 1.0, 4.0, -2.0, 0.0, -4.0, 78.0, 0, 1, 0, 0.0, 88.0);
  MakePreset("Mid Grind", 1, 46.0, -1.0, 78.0, -2.0, 0.0, 5.0, 8.0, 3.0, 90.0, 1, 0, 0, -2.0, 35.0);
  MakePreset("Deep Clean", 0, 12.0, 0.0, 34.0, 2.0, 5.0, 0.0, -2.0, -1.0, 35.0, 0, 1, 0, 1.0, 75.0);
  MakePreset("Dense Pick", 1, 40.0, -1.0, 65.0, 0.0, 0.0, 3.0, 6.0, 4.0, 92.0, 1, 1, 0, -3.0, 65.0);

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [this]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
  };

  mLayoutFunc = [this](IGraphics* pGraphics) {
    const IRECT bounds = pGraphics->GetBounds();
    if (pGraphics->NControls())
      return;

    const IRECT channelRect(731, 58, 916, 108);
    const IRECT gain1Rect(83, 197, 155, 269);
    const IRECT level1Rect(300, 197, 372, 269);
    const IRECT gain2Rect(587, 197, 659, 269);
    const IRECT level2Rect(802, 197, 874, 269);
    const IRECT lowRect(110, 338, 174, 402);
    const IRECT mid1Rect(355, 338, 419, 402);
    const IRECT mid2Rect(585, 338, 649, 402);
    const IRECT highRect(813, 338, 877, 402);
    const IRECT inputRect(64, 458, 122, 516);
    const IRECT blendRect(334, 458, 392, 516);
    const IRECT biteRect(548, 458, 626, 526);
    const IRECT cabinetRect(751, 458, 809, 516);

    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->EnableTooltips(true);

    const IBitmap panelBitmap = pGraphics->LoadBitmap(BLUE_STEEL_PANEL_FN);
    const IBitmap knobBitmap = pGraphics->LoadBitmap(METAL_KNOB_FN);
    pGraphics->AttachControl(new PhotoPanelControl(bounds, panelBitmap), kCtrlTagBackground);

    pGraphics->AttachControl(new PhotoKnobControl(gain1Rect, knobBitmap, kParamGain1), kCtrlTagGain1)
      ->SetTooltip("Drive A: Sets the saturation and harmonic intensity stored for Voice A.");
    pGraphics->AttachControl(new PhotoKnobControl(level1Rect, knobBitmap, kParamLevel1), kCtrlTagLevel1)
      ->SetTooltip("Output A: Matches the final level of Voice A without changing its drive character.");
    pGraphics->AttachControl(new PhotoKnobControl(gain2Rect, knobBitmap, kParamGain2), kCtrlTagGain2)
      ->SetTooltip("Drive B: Sets the saturation and harmonic intensity stored for Voice B.");
    pGraphics->AttachControl(new PhotoKnobControl(level2Rect, knobBitmap, kParamLevel2), kCtrlTagLevel2)
      ->SetTooltip("Output B: Matches the final level of Voice B without changing its drive character.");
    pGraphics->AttachControl(new PhotoKnobControl(lowRect, knobBitmap, kParamLow), kCtrlTagLow)
      ->SetTooltip("Bass 50 Hz: Boosts or cuts the deepest bass weight.");
    pGraphics->AttachControl(new PhotoKnobControl(mid1Rect, knobBitmap, kParamMid1), kCtrlTagMid1)
      ->SetTooltip("Body 200 Hz: Adjusts warmth, fullness and low-mid body.");
    pGraphics->AttachControl(new PhotoKnobControl(mid2Rect, knobBitmap, kParamMid2), kCtrlTagMid2)
      ->SetTooltip("Punch 500 Hz: Shapes note impact and midrange definition.");
    pGraphics->AttachControl(new PhotoKnobControl(highRect, knobBitmap, kParamHigh), kCtrlTagHigh)
      ->SetTooltip("Presence 3 kHz: Adds or removes upper-mid clarity and string detail.");
    pGraphics->AttachControl(new PhotoKnobControl(inputRect, knobBitmap, kParamInput), kCtrlTagInput)
      ->SetTooltip("Input: Matches the instrument level entering the drive circuit. Start at 0 dB.");
    pGraphics->AttachControl(new PhotoKnobControl(blendRect, knobBitmap, kParamBlend), kCtrlTagBlend)
      ->SetTooltip("Character Mix: Blends the clean bass with the selected driven voice.");
    pGraphics->AttachControl(new PhotoAttackToggleControl(biteRect, kParamBite), kCtrlTagBite)
      ->SetTooltip("Attack: Adds pick definition and upper-mid presence before the drive stage.");
    pGraphics->AttachControl(new PhotoKnobControl(cabinetRect, knobBitmap, kParamCabinet), kCtrlTagCabinet)
      ->SetTooltip("Cabinet: Smooths harsh highs and adds speaker-like body. Set to 0% to disable.");
    pGraphics->AttachControl(new PhotoVoiceSwitchControl(channelRect, kParamChannel), kCtrlTagChannel)
      ->SetTooltip("Active Voice: Selects which stored Drive and Output pair is currently heard.");
  };
#endif
}

#if IPLUG_DSP
sample DualBassPreamp::Biquad::Process(sample x)
{
  const double y = b0 * x + z1;
  z1 = b1 * x - a1 * y + z2;
  z2 = b2 * x - a2 * y;
  return static_cast<sample>(y);
}

void DualBassPreamp::Biquad::Reset()
{
  z1 = 0.0;
  z2 = 0.0;
}

void DualBassPreamp::Biquad::SetCoefficients(double nb0, double nb1, double nb2,
                                             double na0, double na1, double na2)
{
  const double inverseA0 = 1.0 / na0;
  b0 = nb0 * inverseA0;
  b1 = nb1 * inverseA0;
  b2 = nb2 * inverseA0;
  a1 = na1 * inverseA0;
  a2 = na2 * inverseA0;
}

void DualBassPreamp::Biquad::SetHighPass(double sampleRate, double frequency, double q)
{
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients((1.0 + cosine) * 0.5, -(1.0 + cosine), (1.0 + cosine) * 0.5,
                  1.0 + alpha, -2.0 * cosine, 1.0 - alpha);
}

void DualBassPreamp::Biquad::SetLowPass(double sampleRate, double frequency, double q)
{
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients((1.0 - cosine) * 0.5, 1.0 - cosine, (1.0 - cosine) * 0.5,
                  1.0 + alpha, -2.0 * cosine, 1.0 - alpha);
}

void DualBassPreamp::Biquad::SetPeak(double sampleRate, double frequency, double q, double gainDB)
{
  const double amplitude = std::pow(10.0, gainDB / 40.0);
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double alpha = std::sin(omega) / (2.0 * q);
  SetCoefficients(1.0 + alpha * amplitude, -2.0 * cosine, 1.0 - alpha * amplitude,
                  1.0 + alpha / amplitude, -2.0 * cosine, 1.0 - alpha / amplitude);
}

void DualBassPreamp::Biquad::SetLowShelf(double sampleRate, double frequency, double gainDB)
{
  const double amplitude = std::pow(10.0, gainDB / 40.0);
  const double omega = 2.0 * kPi * ClampFrequency(sampleRate, frequency) / sampleRate;
  const double cosine = std::cos(omega);
  const double sine = std::sin(omega);
  const double beta = 2.0 * std::sqrt(amplitude) * sine / std::sqrt(2.0);
  const double ap1 = amplitude + 1.0;
  const double am1 = amplitude - 1.0;
  SetCoefficients(amplitude * (ap1 - am1 * cosine + beta),
                  2.0 * amplitude * (am1 - ap1 * cosine),
                  amplitude * (ap1 - am1 * cosine - beta),
                  ap1 + am1 * cosine + beta,
                  -2.0 * (am1 + ap1 * cosine),
                  ap1 + am1 * cosine - beta);
}

void DualBassPreamp::Biquad::SetHighShelf(double sampleRate, double frequency, double gainDB)
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

void DualBassPreamp::ChannelState::Reset()
{
  ampHighPass.Reset();
  attackShelf.Reset();
  attackPeak.Reset();
  distortionHighPass.Reset();
  speakerBody.Reset();
  speakerTilt.Reset();
  speakerLowPass.Reset();
  lowEQ.Reset();
  mid1EQ.Reset();
  mid2EQ.Reset();
  highEQ.Reset();
}

void DualBassPreamp::UpdateFilterCoefficients(ChannelState& state, double lowDB, double mid1DB,
                                               double mid2DB, double highDB)
{
  constexpr double oversamplingRate = 4.0;
  const double baseSampleRate = GetSampleRate();
  const double highSampleRate = baseSampleRate * oversamplingRate;

  state.ampHighPass.SetHighPass(highSampleRate, 24.0, 0.707);
  state.attackShelf.SetHighShelf(highSampleRate, 850.0, 4.5);
  state.attackPeak.SetPeak(highSampleRate, 2350.0, 0.72, 7.0);
  state.distortionHighPass.SetHighPass(highSampleRate, 20.0, 0.707);

  // A deliberately smooth, non-IR speaker curve: low-mid body, a broad
  // upper-mid tilt, then a damped top-end roll-off.
  state.speakerBody.SetPeak(highSampleRate, 760.0, 0.60, 1.2);
  state.speakerTilt.SetHighShelf(highSampleRate, 1750.0, -5.0);
  state.speakerLowPass.SetLowPass(highSampleRate, 5200.0, 0.62);

  state.lowEQ.SetLowShelf(baseSampleRate, 50.0, lowDB);
  state.mid1EQ.SetPeak(baseSampleRate, 200.0, 0.78, mid1DB);
  state.mid2EQ.SetPeak(baseSampleRate, 500.0, 0.82, mid2DB);
  state.highEQ.SetHighShelf(baseSampleRate, 3000.0, highDB);
}

sample DualBassPreamp::Saturate(sample input, double driveAmount)
{
  const double amount = std::clamp(driveAmount, 0.0, 1.0);
  const double driveDB = 26.0 * std::pow(amount, 1.35);
  const double drive = std::pow(10.0, driveDB / 20.0);
  const double bias = 0.018 + 0.082 * amount;

  const double stage1 = std::tanh(input * drive + bias) - std::tanh(bias);
  const double stage2Drive = 1.0 + 2.1 * amount;
  const double stage2Bias = 0.042 * amount;
  const double stage2 = std::tanh(stage1 * stage2Drive - stage2Bias) + std::tanh(stage2Bias);
  const double compensation = 1.0 / (1.0 + 1.25 * amount);
  const double wet = (0.88 * stage2 + 0.12 * input) * compensation;
  return static_cast<sample>(wet);
}

sample DualBassPreamp::ProcessSignal(ChannelState& state, OverSampler<sample>& oversampler,
                                     sample input, const OversampledParams& params)
{
  return oversampler.Process(input, [&state, &params](sample highRateInput) {
    const sample clean = highRateInput;
    const sample ampInput = state.ampHighPass.Process(highRateInput);
    const sample attackShaped = state.attackPeak.Process(state.attackShelf.Process(ampInput));
    const sample preEmphasized = static_cast<sample>(
      ampInput + (attackShaped - ampInput) * params.attackMix);

    const sample saturated = Saturate(preEmphasized, params.driveAmount);
    const sample driven = state.distortionHighPass.Process(saturated);
    const sample speakerFiltered = state.speakerLowPass.Process(
      state.speakerTilt.Process(state.speakerBody.Process(driven)));
    const sample voiced = static_cast<sample>(
      driven + (speakerFiltered - driven) * params.speakerMix);

    return static_cast<sample>(
      clean + (voiced - clean) * params.characterMix);
  });
}

void DualBassPreamp::OnReset()
{
  const double sampleRate = GetSampleRate();
  mInputSmoother.SetSmoothTime(20.0, sampleRate);
  mDriveSmoother.SetSmoothTime(20.0, sampleRate);
  mLevelSmoother.SetSmoothTime(20.0, sampleRate);
  mBlendSmoother.SetSmoothTime(15.0, sampleRate);
  mAttackSmoother.SetSmoothTime(8.0, sampleRate);
  mSpeakerSmoother.SetSmoothTime(8.0, sampleRate);

  const int channel = std::clamp(GetParam(kParamChannel)->Int(), 0, 1);
  const int gainParam = channel == 0 ? kParamGain1 : kParamGain2;
  const int levelParam = channel == 0 ? kParamLevel1 : kParamLevel2;
  mInputSmoother.SetValue(static_cast<sample>(std::pow(10.0, GetParam(kParamInput)->Value() / 20.0)));
  mDriveSmoother.SetValue(static_cast<sample>(GetParam(gainParam)->Value() / 100.0));
  mLevelSmoother.SetValue(static_cast<sample>(std::pow(10.0, GetParam(levelParam)->Value() / 20.0)));
  mBlendSmoother.SetValue(static_cast<sample>(GetParam(kParamBlend)->Value() / 100.0));
  mAttackSmoother.SetValue(GetParam(kParamBite)->Bool() ? 1.0 : 0.0);
  mSpeakerSmoother.SetValue(static_cast<sample>(GetParam(kParamCabinet)->Value() / 100.0));

  mOversamplerLeft.Reset();
  mOversamplerRight.Reset();

  for (auto& state : mChannelStates)
  {
    state.Reset();
    UpdateFilterCoefficients(state, GetParam(kParamLow)->Value(), GetParam(kParamMid1)->Value(),
                             GetParam(kParamMid2)->Value(), GetParam(kParamHigh)->Value());
  }
}

void DualBassPreamp::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = std::min(NOutChansConnected(), static_cast<int>(mChannelStates.size()));
  const int selectedChannel = std::clamp(GetParam(kParamChannel)->Int(), 0, 1);
  const int gainParam = selectedChannel == 0 ? kParamGain1 : kParamGain2;
  const int levelParam = selectedChannel == 0 ? kParamLevel1 : kParamLevel2;
  const double inputTarget = std::pow(10.0, GetParam(kParamInput)->Value() / 20.0);
  const double driveTarget = GetParam(gainParam)->Value() / 100.0;
  const double levelTarget = std::pow(10.0, GetParam(levelParam)->Value() / 20.0);
  const double blendTarget = GetParam(kParamBlend)->Value() / 100.0;
  const double attackTarget = GetParam(kParamBite)->Bool() ? 1.0 : 0.0;
  const double speakerTarget = GetParam(kParamCabinet)->Value() / 100.0;

  for (int channel = 0; channel < nChans; ++channel)
  {
    UpdateFilterCoefficients(mChannelStates[channel], GetParam(kParamLow)->Value(),
                             GetParam(kParamMid1)->Value(), GetParam(kParamMid2)->Value(),
                             GetParam(kParamHigh)->Value());
  }

  for (int sampleIndex = 0; sampleIndex < nFrames; ++sampleIndex)
  {
    const double inputGain = mInputSmoother.Process(static_cast<sample>(inputTarget));
    const double driveAmount = mDriveSmoother.Process(static_cast<sample>(driveTarget));
    const double outputLevel = mLevelSmoother.Process(static_cast<sample>(levelTarget));
    const double characterMix = mBlendSmoother.Process(static_cast<sample>(blendTarget));
    const double attack = mAttackSmoother.Process(static_cast<sample>(attackTarget));
    const double speaker = mSpeakerSmoother.Process(static_cast<sample>(speakerTarget));
    const OversampledParams params {driveAmount, characterMix, attack, speaker};

    for (int channel = 0; channel < nChans; ++channel)
    {
      ChannelState& state = mChannelStates[channel];
      const sample dry = inputs[channel][sampleIndex];
      OverSampler<sample>& oversampler = channel == 0 ? mOversamplerLeft : mOversamplerRight;
      sample mixed = ProcessSignal(state, oversampler, static_cast<sample>(dry * inputGain), params);
      mixed = state.lowEQ.Process(mixed);
      mixed = state.mid1EQ.Process(mixed);
      mixed = state.mid2EQ.Process(mixed);
      mixed = state.highEQ.Process(mixed);

      const double processed = std::clamp(mixed * outputLevel, -8.0, 8.0);
      outputs[channel][sampleIndex] = static_cast<sample>(processed);
    }
  }
}
#endif
