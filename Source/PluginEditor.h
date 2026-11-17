#pragma once
#include "PluginProcessor.h"

namespace yl {
    const juce::Colour bg (0xff150e10), hdr (0xff1b1316), panel (0xff21181c), line (0xff3a2b32), pink (0xffffc9dd), pink2 (0xffff9fc4),
                       mute (0xff9a8a92), plum (0xff7a3558), ink (0xff0f0a0c), cream (0xfffff4f7);
}

class YoloLook : public juce::LookAndFeel_V4
{
public:
    YoloLook();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider&) override;
    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h, float pos, float, float, juce::Slider::SliderStyle, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool, int, int, int, int, juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override { return juce::FontOptions (14.f); }
};

// segmented pill selector driven by getter/setter lambdas
class Seg : public juce::Component
{
public:
    Seg (juce::StringArray l, int columns, std::function<int()> g, std::function<void (int)> s)
        : labels (std::move (l)), cols (columns), get (std::move (g)), set (std::move (s)) {}
    juce::StringArray labels; int cols;
    std::function<int()> get; std::function<void (int)> set;
    bool chips = false;      // true: separate round chips (key grid); false: one joined pill
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
private:
    juce::Rectangle<float> cell (int i) const;
};

class Piano : public juce::Component, private juce::Timer
{
public:
    Piano (YoloProcessor& p, const bool& flatsRef) : proc (p), flats (flatsRef) { startTimerHz (20); }
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    float heatIn[12] {}, heatOut[12] {};
    bool showHeat = true, showDetected = true;
private:
    static constexpr int chipH = 28, heatH = 58;
    int keyAt (juce::Point<float>) const;
    juce::Rectangle<float> keyRegion() const { return getLocalBounds().toFloat().withTrimmedTop ((float) (chipH + heatH)); }
    void timerCallback() override;
    YoloProcessor& proc; const bool& flats;
};

class Scene : public juce::Component, private juce::Timer
{
public:
    explicit Scene (YoloProcessor&);
    void paint (juce::Graphics&) override;
private:
    struct Petal { float x, y, vx, vy, rot, vr, s; };
    struct Drop { float x, y, len, v; };
    void timerCallback() override;
    YoloProcessor& proc; float angle = 0, level = 0;
    std::vector<Petal> petals; std::vector<Drop> rain; juce::Random rnd;
};

class YoloEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit YoloEditor (YoloProcessor&);
    ~YoloEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    struct Content; std::unique_ptr<Content> content;
    juce::ComponentBoundsConstrainer constrainer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YoloEditor)
};
