#include "PluginEditor.h"
using namespace yl;
static const char* kSharp[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char* kFlat[12]  = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};
static bool isBlack (int n) { return n==1||n==3||n==6||n==8||n==10; }
static juce::Font serif (float h, int style = juce::Font::plain) { return juce::FontOptions (juce::Font::getDefaultSerifFontName(), h, style); }
static const float PI = juce::MathConstants<float>::pi;

YoloLook::YoloLook()
{
    setColour (juce::PopupMenu::backgroundColourId, panel); setColour (juce::PopupMenu::textColourId, pink);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, plum); setColour (juce::PopupMenu::highlightedTextColourId, cream);
}

void YoloLook::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (.5f);
    g.setColour (ink); g.fillRoundedRectangle (r, 8.f); g.setColour (line); g.drawRoundedRectangle (r, 8.f, 1.f);
    juce::Path a; a.addTriangle (w - 24.f, h * .42f, w - 12.f, h * .42f, w - 18.f, h * .62f); g.setColour (mute); g.fillPath (a);
}

void YoloLook::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider& s)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.f);
    const float R = juce::jmin (b.getWidth(), b.getHeight()) * .5f; auto c = b.getCentre();
    // tick ring
    const int N = 56; const float rIn = R * .86f, rOut = R;
    for (int i = 0; i < N; ++i)
    {
        const float t = (float) i / (N - 1), a = a0 + t * (a1 - a0) - PI * .5f;
        const bool on = t <= pos + 0.001f;
        juce::Path tk; const float len = on ? (rOut - rIn) : (rOut - rIn) * .7f;
        tk.addRectangle (-1.4f, -rOut, 2.8f, len);
        g.setColour (on ? pink.interpolatedWith (pink2, t) : line.brighter (.25f));
        g.fillPath (tk, juce::AffineTransform::rotation (a + PI * .5f).translated (c));
    }
    const float rb = rIn - 6.f;
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2b2025), c.x, c.y - rb, ink, c.x, c.y + rb, false));
    g.fillEllipse (c.x - rb, c.y - rb, 2 * rb, 2 * rb);
    g.setColour (line); g.drawEllipse (c.x - rb, c.y - rb, 2 * rb, 2 * rb, 1.5f);
    const float ang = a0 + pos * (a1 - a0);
    const float kx = c.x + std::sin (ang) * (rIn + 1), ky = c.y - std::cos (ang) * (rIn + 1);
    g.setColour (cream); g.fillEllipse (kx - 4.5f, ky - 4.5f, 9.f, 9.f);
    const float big = rb > 80.f ? 40.f : (rb > 50.f ? 34.f : 22.f);
    g.setColour (cream); g.setFont (serif (big, juce::Font::bold));
    g.drawText (s.getTextFromValue (s.getValue()).upToFirstOccurrenceOf (" ", false, false), juce::Rectangle<float> (c.x - rb, c.y - big * .8f, 2 * rb, big * 1.2f), juce::Justification::centred);
    g.setColour (mute); g.setFont (juce::FontOptions (rb > 50.f ? 13.f : 12.f));
    g.drawText (s.getName(), juce::Rectangle<float> (c.x - rb, c.y + big * .45f, 2 * rb, 20.f), juce::Justification::centred);
}

void YoloLook::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float pos, float, float, juce::Slider::SliderStyle, juce::Slider& s)
{
    const float ty = y + h - 11.f;
    g.setColour (cream.withAlpha (.85f)); g.setFont (juce::FontOptions (13.f, juce::Font::bold));
    g.drawText (s.getName(), x, y, w / 2, 16, juce::Justification::centredLeft);
    g.setColour (mute); g.drawText (s.getTextFromValue (s.getValue()) + (s.getName().isEmpty() ? "" : "%"), x + w / 2, y, w / 2, 16, juce::Justification::centredRight);
    g.setColour (line); g.fillRoundedRectangle ((float) x + 6, ty - 2, (float) w - 12, 4.f, 2.f);
    g.setGradientFill (juce::ColourGradient (pink2, (float) x, ty, pink, (float) x + w, ty, false));
    g.fillRoundedRectangle ((float) x + 6, ty - 2, juce::jmax (0.f, pos - x - 6), 4.f, 2.f);
    g.setColour (cream); g.fillEllipse (pos - 8, ty - 8, 16, 16);
    g.setColour (pink2); g.drawEllipse (pos - 8, ty - 8, 16, 16, 2.f);
}

void YoloLook::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool)
{
    auto r = b.getLocalBounds().toFloat(); const bool on = b.getToggleState();
    if (b.getComponentID() == "byp")
    {
        auto c = juce::Point<float> (r.getX() + 22, r.getCentreY());
        g.setColour (on ? pink2 : mute); juce::Path a; a.addCentredArc (c.x, c.y, 8, 8, 0, .6f, 2 * PI - .6f, true); a.startNewSubPath (c.x, c.y - 10); a.lineTo (c.x, c.y - 2);
        g.strokePath (a, juce::PathStrokeType (2.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setFont (juce::FontOptions (13.f, juce::Font::bold)); g.setColour (on ? pink2 : cream.withAlpha (.7f));
        g.drawFittedText (b.getButtonText(), r.withTrimmedLeft (44).toNearestInt(), juce::Justification::centredLeft, 2);
        return;
    }
    auto sw = juce::Rectangle<float> (r.getX() + 2, r.getCentreY() - 10, 40, 20);
    g.setColour (on ? pink2 : ink); g.fillRoundedRectangle (sw, 10.f); g.setColour (on ? pink : line); g.drawRoundedRectangle (sw, 10.f, 1.5f);
    g.setColour (cream); g.fillEllipse (on ? sw.getRight() - 18 : sw.getX() + 2, sw.getY() + 2, 16, 16);
    g.setFont (juce::FontOptions (14.f, juce::Font::bold)); g.setColour (cream);
    g.drawText (b.getButtonText(), r.withTrimmedLeft (50).toNearestInt(), juce::Justification::centredLeft);
}

// ---------- segmented ----------
juce::Rectangle<float> Seg::cell (int i) const
{
    const int n = labels.size(), rows = (n + cols - 1) / cols;
    const float cw = (float) getWidth() / cols, ch = (float) getHeight() / rows;
    return { (i % cols) * cw, (i / cols) * ch, cw, ch };
}
void Seg::paint (juce::Graphics& g)
{
    const int sel = get ? get() : -1;
    if (! chips) { auto all = getLocalBounds().toFloat().reduced (.5f); g.setColour (ink); g.fillRoundedRectangle (all, all.getHeight() / 2); g.setColour (line); g.drawRoundedRectangle (all, all.getHeight() / 2, 1.f); }
    for (int i = 0; i < labels.size(); ++i)
    {
        auto r = cell (i).reduced (chips ? 3.f : 3.f);
        if (i == sel) { g.setColour (chips ? pink2 : pink); g.fillRoundedRectangle (r, chips ? 8.f : r.getHeight() / 2); }
        else if (chips) { g.setColour (ink); g.fillRoundedRectangle (r, 8.f); g.setColour (line); g.drawRoundedRectangle (r, 8.f, 1.f); }
        g.setColour (i == sel ? ink : cream.withAlpha (.8f)); g.setFont (juce::FontOptions (chips ? 12.f : 14.f, juce::Font::bold));
        g.drawText (labels[i], r.toNearestInt(), juce::Justification::centred);
    }
}
void Seg::mouseDown (const juce::MouseEvent& e)
{
    for (int i = 0; i < labels.size(); ++i) if (cell (i).contains (e.position)) { if (set) set (i); repaint(); return; }
}

// ---------- piano ----------
static void currentMask (YoloProcessor& p, bool out[12])
{
    bool cu[12]; for (int i = 0; i < 12; ++i) cu[i] = p.apvts.getRawParameterValue ("n" + juce::String (i))->load() > .5f;
    YoloProcessor::scaleMask ((int) p.apvts.getRawParameterValue ("key")->load(), (int) p.apvts.getRawParameterValue ("scale")->load(), cu, out);
}
static juce::Rectangle<float> keyRect (juce::Rectangle<float> b, int n)
{
    static const int wi[12] = {0,-1,1,-1,2,3,-1,4,-1,5,-1,6};
    const float ww = b.getWidth() / 7.f;
    if (! isBlack (n)) return { b.getX() + wi[n] * ww, b.getY(), ww, b.getHeight() };
    return { b.getX() + (wi[n - 1] + 1) * ww - ww * .27f, b.getY(), ww * .54f, b.getHeight() * .62f };
}
void Piano::timerCallback()
{
    for (int i = 0; i < 12; ++i) { heatIn[i] *= .96f; heatOut[i] *= .96f; }
    if (proc.engine.voiced.load())
    {
        auto cls = [] (float m) { return ((int) std::lround (m) % 12 + 12) % 12; };
        heatIn[cls (proc.engine.detM.load())] += 1.f; heatOut[cls (proc.engine.outM.load())] += 1.f;
    }
    repaint();
}
int Piano::keyAt (juce::Point<float> pt) const
{
    auto kb = keyRegion();
    for (int n = 0; n < 12; ++n) if (isBlack (n) && keyRect (kb, n).contains (pt)) return n;
    for (int n = 0; n < 12; ++n) if (! isBlack (n) && keyRect (kb, n).contains (pt)) return n;
    // chip row: chips are evenly spaced over the full width
    if (pt.y < chipH + heatH && pt.y >= heatH) return juce::jlimit (0, 11, (int) (pt.x / (getWidth() / 12.f)));
    return -1;
}
void Piano::mouseDown (const juce::MouseEvent& e)
{
    const int n = keyAt (e.position); if (n < 0) return;
    bool al[12]; currentMask (proc, al);
    for (int i = 0; i < 12; ++i)
        if (auto* p = proc.apvts.getParameter ("n" + juce::String (i))) p->setValueNotifyingHost ((i == n ? ! al[i] : al[i]) ? 1.f : 0.f);
    if (auto* s = proc.apvts.getParameter ("scale")) s->setValueNotifyingHost (s->convertTo0to1 (3.f));
}
void Piano::paint (juce::Graphics& g)
{
    bool al[12]; currentMask (proc, al);
    const bool vc = proc.engine.voiced.load();
    const int act = ((int) std::lround (proc.engine.outM.load()) % 12 + 12) % 12;
    const float cw = getWidth() / 12.f;
    float mx = 6.f; for (int i = 0; i < 12; ++i) mx = juce::jmax (mx, heatIn[i], heatOut[i]);
    // heat
    g.setColour (ink); g.fillRect (0, 0, getWidth(), heatH);
    for (int i = 0; i < 12; ++i)
    {
        auto col = juce::Rectangle<float> (i * cw + 5, 3, cw - 10, heatH - 6);
        g.setColour (line.withAlpha (.5f)); for (float yy = col.getY(); yy < col.getBottom(); yy += 4.f) g.fillRect (col.getX(), yy, col.getWidth(), 1.f);
        if (showHeat)
        {
            const float hi = juce::jmin (1.f, heatIn[i] / mx) * (col.getHeight() * .5f - 2), ho = juce::jmin (1.f, heatOut[i] / mx) * (col.getHeight() * .5f - 2);
            g.setColour (pink2); g.fillRect (col.getX(), col.getY() + col.getHeight() * .5f - 2 - hi, col.getWidth(), hi);
            g.setColour (cream); g.fillRect (col.getX(), col.getBottom() - ho, col.getWidth(), ho);
        }
    }
    // chips
    for (int i = 0; i < 12; ++i)
    {
        auto r = juce::Rectangle<float> (i * cw + 4, (float) heatH + 3, cw - 8, (float) chipH - 6);
        const bool hot = showDetected && vc && i == act;
        g.setColour (hot ? pink2 : (al[i] ? cream : ink)); g.fillRoundedRectangle (r, 6.f);
        g.setColour (line); g.drawRoundedRectangle (r, 6.f, 1.f);
        g.setColour (hot || al[i] ? ink : mute); g.setFont (juce::FontOptions (13.f, juce::Font::bold));
        g.drawText (flats ? kFlat[i] : kSharp[i], r.toNearestInt(), juce::Justification::centred);
    }
    // keys
    auto kb = keyRegion();
    for (int pass = 0; pass < 2; ++pass)
        for (int n = 0; n < 12; ++n)
        {
            if (isBlack (n) != (pass == 1)) continue;
            auto r = keyRect (kb, n).reduced (1.f, 0);
            const bool hot = showDetected && vc && n == act;
            juce::Colour top = isBlack (n) ? juce::Colour (0xffa04770) : cream, bot = isBlack (n) ? juce::Colour (0xff6b2a4a) : pink2.brighter (.1f);
            if (! al[n]) { top = isBlack (n) ? juce::Colour (0xff2a1c22) : juce::Colour (0xff3a2a31); bot = isBlack (n) ? juce::Colour (0xff1a1216) : juce::Colour (0xff2a1d23); }
            if (hot) { top = pink; bot = pink2; }
            g.setGradientFill (juce::ColourGradient (top, 0, r.getY(), bot, 0, r.getBottom(), false));
            g.fillRoundedRectangle (r.withTrimmedTop (-6), 8.f);
            g.setColour (ink.withAlpha (.6f)); g.drawRoundedRectangle (r.withTrimmedTop (-6), 8.f, 1.f);
        }
}

// ---------- scene ----------
Scene::Scene (YoloProcessor& p) : proc (p)
{
    for (int i = 0; i < 26; ++i) petals.push_back ({ rnd.nextFloat() * 620, rnd.nextFloat() * 370, rnd.nextFloat() - .5f, .4f + rnd.nextFloat() * .8f, rnd.nextFloat() * 6, (rnd.nextFloat() - .5f) * .05f, 4.f + rnd.nextFloat() * 5.f });
    for (int i = 0; i < 38; ++i) rain.push_back ({ rnd.nextFloat() * 620, rnd.nextFloat() * 370, 10.f + rnd.nextFloat() * 24.f, 4.f + rnd.nextFloat() * 6.f });
    startTimerHz (30);
}
void Scene::timerCallback()
{
    level += (juce::jlimit (0.f, 1.f, proc.engine.peakIn.load() * 3.f) - level) * .25f;
    angle += .006f + level * .1f;
    const float w = (float) getWidth(), h = (float) getHeight();
    for (auto& q : petals) { q.x += q.vx + std::sin (q.y * .02f) * .3f; q.y += q.vy; q.rot += q.vr; if (q.y > h + 10) { q.y = -10; q.x = rnd.nextFloat() * w; } if (q.x > w + 10) q.x = -10; if (q.x < -10) q.x = w; }
    for (auto& d : rain) { d.y += d.v * (1.f + level * 1.5f); if (d.y > h + 30) { d.y = -30; d.x = rnd.nextFloat() * w; } }
    repaint();
}
void Scene::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat(); auto c = b.getCentre();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff1c1317), c.x, c.y, ink, 0, 0, true)); g.fillAll();
    g.setColour (pink.withAlpha (.12f)); for (auto& d : rain) g.drawLine (d.x, d.y, d.x - 2, d.y + d.len, 1.f);
    for (auto& q : petals)
    {
        juce::Path p; p.addEllipse (-q.s, -q.s * .55f, q.s * 2, q.s * 1.1f);
        g.setColour (pink.withAlpha (.55f)); g.fillPath (p, juce::AffineTransform::rotation (q.rot).translated (q.x, q.y));
    }
    const float r = 108.f;
    g.setColour (pink.withAlpha (.10f + level * .25f)); g.fillEllipse (c.x - r - 30, c.y - r - 30, 2 * r + 60, 2 * r + 60);
    g.setColour (pink.withAlpha (.35f));
    for (int i = 0; i < 72; ++i) { const float a = i * 2 * PI / 72 + angle * .3f; g.fillEllipse (c.x + std::cos (a) * (r + 44) - 1.2f, c.y + std::sin (a) * (r + 44) - 1.2f, 2.4f, 2.4f); }
    g.setColour (pink); g.drawEllipse (c.x - r - 14, c.y - r - 14, 2 * r + 28, 2 * r + 28, 3.f);
    juce::Graphics::ScopedSaveState ss (g);
    g.addTransform (juce::AffineTransform::rotation (angle, c.x, c.y));
    g.setColour (cream); g.fillEllipse (c.x - r, c.y - r, 2 * r, 2 * r);
    juce::Path half; half.addPieSegment (c.x - r, c.y - r, 2 * r, 2 * r, 0.f, PI, 0.f);
    g.setColour (ink); g.fillPath (half);
    g.setColour (cream); g.fillEllipse (c.x - r / 2, c.y - r, r, r);
    g.setColour (ink);   g.fillEllipse (c.x - r / 2, c.y, r, r);
    g.fillEllipse (c.x - r * .12f, c.y - r * .62f, r * .24f, r * .24f);
    g.setColour (cream); g.fillEllipse (c.x - r * .12f, c.y + r * .38f, r * .24f, r * .24f);
}

// ---------- editor ----------
struct YoloEditor::Content : public juce::Component, private juce::Timer
{
    Content (YoloProcessor& p)
        : proc (p), scene (p), piano (p, flats)
    {
        auto& a = proc.apvts;
        addAndMakeVisible (scene); addAndMakeVisible (piano);
        auto dial = [this] (juce::Slider& s, const juce::String& name, bool rotary)
        {
            s.setName (name);
            s.setSliderStyle (rotary ? juce::Slider::RotaryHorizontalVerticalDrag : juce::Slider::LinearHorizontal);
            s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
            if (rotary) s.setRotaryParameters (PI * 1.25f, PI * 2.75f, true);
            addAndMakeVisible (s);
        };
        dial (speed, "Speed (ms)", true); dial (sustain, "Sustain", true); dial (amount, "Amount", true);
        dial (inG, "Input", false); dial (outG, "Output", false); dial (dbl, "Doubler", false); dial (flex, "Flex-Tune", false); dial (hum, "Humanize", false); dial (ref, "", false);
        aSp = std::make_unique<SA> (a, "speed", speed); aSu = std::make_unique<SA> (a, "sustain", sustain); aAm = std::make_unique<SA> (a, "amount", amount);
        aIn = std::make_unique<SA> (a, "inGain", inG); aOut = std::make_unique<SA> (a, "outGain", outG); aDb = std::make_unique<SA> (a, "doubler", dbl);
        aFl = std::make_unique<SA> (a, "flex", flex); aHu = std::make_unique<SA> (a, "humanize", hum); aRef = std::make_unique<SA> (a, "ref", ref);

        auto choiceSeg = [this] (const char* id, juce::StringArray l, int cols, bool chips)
        {
            auto* par = proc.apvts.getParameter (id);
            auto s = std::make_unique<Seg> (l, cols, [par] { return (int) std::lround (par->convertFrom0to1 (par->getValue())); },
                                            [par] (int i) { par->setValueNotifyingHost (par->convertTo0to1 ((float) i)); });
            s->chips = chips; addAndMakeVisible (*s); return s;
        };
        scaleSeg = choiceSeg ("scale", {"Chromatic", "Major", "Minor", "Note"}, 4, false);
        stabSeg = choiceSeg ("stab", {"None", "Short", "Mid", "Long"}, 4, false);
        keySeg = choiceSeg ("key", juce::StringArray (kSharp, 12), 4, true);
        accSeg = std::make_unique<Seg> (juce::StringArray {"#", "b"}, 2, [this] { return flats ? 1 : 0; }, [this] (int i) { flats = i == 1; repaintKeys(); });
        addAndMakeVisible (*accSeg);
        for (auto* t : {&inst, &lowlat, &bypass, &heatT, &detT}) addAndMakeVisible (t);
        inst.setButtonText ("Instrument"); lowlat.setButtonText ("Low Latency"); bypass.setButtonText ("Global Bypass"); bypass.setComponentID ("byp");
        heatT.setButtonText ("Heat Maps"); detT.setButtonText ("Detected"); heatT.setToggleState (true, juce::dontSendNotification); detT.setToggleState (true, juce::dontSendNotification);
        heatT.onClick = [this] { piano.showHeat = heatT.getToggleState(); }; detT.onClick = [this] { piano.showDetected = detT.getToggleState(); };
        aInst = std::make_unique<BA> (a, "inst", inst); aLow = std::make_unique<BA> (a, "lowlat", lowlat); aByp = std::make_unique<BA> (a, "bypass", bypass);
        for (int i = 0; i < proc.getNumPrograms(); ++i) preset.addItem (proc.getProgramName (i), i + 1);
        preset.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);
        preset.onChange = [this] { proc.setCurrentProgram (preset.getSelectedId() - 1); };
        preset.setJustificationType (juce::Justification::centred); preset.setColour (juce::ComboBox::textColourId, mute);
        addAndMakeVisible (preset);
        startTimerHz (10);
    }
    void repaintKeys()
    {
        for (int i = 0; i < 12; ++i) keySeg->labels.set (i, flats ? kFlat[i] : kSharp[i]);
        keySeg->repaint(); piano.repaint();
    }
    void timerCallback() override { repaint (0, getHeight() - 34, getWidth(), 34); }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (bg);
        g.setColour (hdr); g.fillRect (0, 0, getWidth(), 70); g.setColour (ink); g.fillRect (0, 69, getWidth(), 1);
        g.setGradientFill (juce::ColourGradient (pink, 16, 14, pink2, 58, 56, false)); g.fillRoundedRectangle (14, 14, 42, 42, 9.f);
        g.setColour (ink); g.fillEllipse (22, 22, 26, 26); g.setColour (cream); { juce::Path hp; hp.addPieSegment (22, 22, 26, 26, 0.f, PI, 0.f); g.fillPath (hp); } g.fillEllipse (28.5f, 22, 13, 13); g.setColour (ink); g.fillEllipse (28.5f, 35, 13, 13);
        g.setColour (cream); g.setFont (serif (38, juce::Font::bold)); g.drawText ("Yolo", 0, 4, getWidth(), 62, juce::Justification::centred);
        g.setColour (line); g.fillRect (getWidth() / 2 + 60, 36, 300, 1); g.fillRect (getWidth() / 2 - 360, 36, 300, 1);
        // dial panel
        g.setColour (panel); g.fillRoundedRectangle (650, 82, 436, 360, 22.f); g.setColour (line); g.drawRoundedRectangle (650, 82, 436, 360, 22.f, 1.f);
        // mode row
        g.setColour (hdr); g.fillRect (0, 456, getWidth(), 64); g.setColour (ink); g.fillRect (0, 455, getWidth(), 1);
        // left column
        g.setColour (panel); g.fillRect (0, 520, 196, 300);
        g.setColour (cream.withAlpha (.8f)); g.setFont (serif (15, juce::Font::italic | juce::Font::bold)); g.drawText ("Levels", 14, 524, 100, 20, juce::Justification::centredLeft);
        // right column
        g.setColour (panel); g.fillRect (942, 520, 158, 300);
        g.setColour (line); for (int y : {590, 670, 750}) g.fillRect (946, y, 150, 1);
        g.setColour (cream.withAlpha (.8f)); g.setFont (juce::FontOptions (13.f, juce::Font::bold));
        g.drawText ("Pitch Reference", 946, 756, 150, 18, juce::Justification::centred);
        g.setColour (ink); g.fillRoundedRectangle (968, 796, 106, 22, 11.f);
        g.setColour (pink); g.setFont (juce::FontOptions (12.f, juce::Font::bold));
        g.drawText ("440 x " + juce::String (std::pow (2.0, ref.getValue() / 1200.0), 4) + " = " + juce::String (440.0 * std::pow (2.0, ref.getValue() / 1200.0), 1) + "Hz", 960, 796, 122, 22, juce::Justification::centred);
        // status
        g.setColour (hdr); g.fillRect (0, getHeight() - 34, getWidth(), 34);
        g.setColour (mute); g.setFont (juce::FontOptions (12.f));
        juce::String det = "listening";
        if (proc.engine.voiced.load())
        {
            auto nm = [this] (float m) { const int c = ((int) std::lround (m) % 12 + 12) % 12; return juce::String (flats ? kFlat[c] : kSharp[c]); };
            det = nm (proc.engine.detM.load()) + " -> " + nm (proc.engine.outM.load());
        }
        g.drawText (det, 18, getHeight() - 34, 200, 34, juce::Justification::centredLeft);
        g.drawText ("latency " + juce::String (proc.getLatencySamples()) + " smp  |  " + juce::String (proc.getSampleRate() / 1000.0, 1) + " kHz  |  " + (inst.getToggleState() ? "Instrument" : "Voice"),
                    260, getHeight() - 34, 560, 34, juce::Justification::centred);
        g.drawText ("YOLO 1.0", getWidth() - 140, getHeight() - 34, 120, 34, juce::Justification::centredRight);
    }

    void resized() override
    {
        preset.setBounds (70, 18, 230, 34);
        scene.setBounds (0, 70, 640, 386);
        speed.setBounds (656, 96, 250, 250);
        sustain.setBounds (918, 138, 120, 120);
        amount.setBounds (760, 262, 180, 180);
        scaleSeg->setBounds (16, 466, 320, 40);
        accSeg->setBounds (348, 468, 76, 36);
        stabSeg->setBounds (440, 466, 280, 40);
        inst.setBounds (740, 468, 140, 36); lowlat.setBounds (900, 468, 160, 36);
        int y = 548;
        for (auto* s : {&inG, &outG, &dbl, &flex, &hum}) { s->setBounds (14, y, 170, 36); y += 44; }
        keySeg->setBounds (14, 760, 170, 56);
        piano.setBounds (198, 520, 740, 300);
        heatT.setBounds (950, 520, 146, 66); detT.setBounds (950, 596, 146, 70); bypass.setBounds (950, 676, 146, 70);
        ref.setBounds (954, 770, 138, 24);
    }

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    YoloProcessor& proc; bool flats = false;
    Scene scene; Piano piano;
    juce::Slider speed, sustain, amount, inG, outG, dbl, flex, hum, ref;
    juce::ToggleButton inst, lowlat, bypass, heatT, detT;
    juce::ComboBox preset;
    std::unique_ptr<Seg> scaleSeg, stabSeg, keySeg, accSeg;
    std::unique_ptr<SA> aSp, aSu, aAm, aIn, aOut, aDb, aFl, aHu, aRef;
    std::unique_ptr<BA> aInst, aLow, aByp;
};

YoloEditor::YoloEditor (YoloProcessor& p) : AudioProcessorEditor (&p)
{
    static YoloLook look; juce::ignoreUnused (look);
    content = std::make_unique<Content> (p);
    content->setLookAndFeel (&look);
    content->setSize (1100, 860);
    addAndMakeVisible (*content);
    constrainer.setFixedAspectRatio (1100.0 / 860.0);
    constrainer.setSizeLimits (825, 645, 1650, 1290);
    setConstrainer (&constrainer);
    setResizable (true, true);
    setSize (1100, 860);
}
YoloEditor::~YoloEditor() { content->setLookAndFeel (nullptr); }
void YoloEditor::paint (juce::Graphics& g) { g.fillAll (bg); }
void YoloEditor::resized()
{
    const float s = (float) getWidth() / 1100.f;
    content->setTransform (juce::AffineTransform::scale (s));
    content->setBounds (0, 0, 1100, 860);
}
void YoloEditor::timerCallback() {}
