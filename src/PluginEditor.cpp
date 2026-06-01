#include "PluginEditor.h"
#include "PluginProcessor.h"

namespace
{
const auto backgroundColour = juce::Colour(0xff121212);
const auto panelColour = juce::Colour(0xff1a1a1a);
const auto accentColour = juce::Colour(0xffff8a3d);
const auto textColour = juce::Colour(0xffeaeaea);
const auto secondaryTextColour = juce::Colour(0xff8a8a8a);

void configureRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 88, 24);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff333333));
    slider.setColour(juce::Slider::thumbColourId, textColour);
    slider.setColour(juce::Slider::textBoxTextColourId, textColour);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void configureKnobLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, textColour);
    label.setFont(juce::FontOptions(14.0f, juce::Font::bold));
}

void configureDescriptorButton(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
    button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff242424));
    button.setColour(juce::TextButton::buttonOnColourId, accentColour);
    button.setColour(juce::TextButton::textColourOffId, textColour);
    button.setColour(juce::TextButton::textColourOnId, backgroundColour);
}
}

ToneflxLiteAudioProcessorEditor::ToneflxLiteAudioProcessorEditor(ToneflxLiteAudioProcessor& owner)
    : AudioProcessorEditor(&owner),
      audioProcessor(owner)
{
    titleLabel.setText("TONEFLX Lite", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, textColour);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    statusLabel.setText("Saturation + bitcrusher + chorus + delay + reverb active", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::textColourId, secondaryTextColour);
    statusLabel.setFont(juce::FontOptions(15.0f));
    addAndMakeVisible(statusLabel);

    for (auto* button : { &warmButton, &darkButton, &vintageButton, &wideButton, &dreamyButton })
    {
        configureDescriptorButton(*button);
        addAndMakeVisible(*button);
    }

    generateButton.setColour(juce::TextButton::buttonColourId, accentColour);
    generateButton.setColour(juce::TextButton::textColourOffId, backgroundColour);
    generateButton.onClick = [this] {
        const auto seed = audioProcessor.generatePresetForDescriptors(collectSelectedDescriptors());
        seedLabel.setText("Seed " + juce::String(seed), juce::dontSendNotification);
        seedEditor.setText(juce::String(seed), juce::dontSendNotification);
    };
    addAndMakeVisible(generateButton);

    recallButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff242424));
    recallButton.setColour(juce::TextButton::textColourOffId, textColour);
    recallButton.onClick = [this] {
        const auto seedText = seedEditor.getText().trim();

        if (seedText.isEmpty())
            return;

        const auto seed = static_cast<std::uint32_t>(seedText.getLargeIntValue());
        audioProcessor.generatePresetForDescriptors(collectSelectedDescriptors(), seed);
        seedLabel.setText("Seed " + juce::String(seed), juce::dontSendNotification);
    };
    addAndMakeVisible(recallButton);

    seedLabel.setText("Seed -", juce::dontSendNotification);
    seedLabel.setJustificationType(juce::Justification::centredLeft);
    seedLabel.setColour(juce::Label::textColourId, secondaryTextColour);
    seedLabel.setFont(juce::FontOptions(14.0f));
    addAndMakeVisible(seedLabel);

    seedEditor.setInputRestrictions(10, "0123456789");
    seedEditor.setTextToShowWhenEmpty("seed", secondaryTextColour);
    seedEditor.setJustification(juce::Justification::centredLeft);
    seedEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff242424));
    seedEditor.setColour(juce::TextEditor::textColourId, textColour);
    seedEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    seedEditor.setColour(juce::TextEditor::focusedOutlineColourId, accentColour);
    addAndMakeVisible(seedEditor);

    configureRotarySlider(driveSlider);
    addAndMakeVisible(driveSlider);

    configureKnobLabel(driveLabel, "Drive");
    addAndMakeVisible(driveLabel);

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::saturationDrive,
        driveSlider);

    configureRotarySlider(mixSlider);
    addAndMakeVisible(mixSlider);

    configureKnobLabel(mixLabel, "Sat Mix");
    addAndMakeVisible(mixLabel);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::saturationMix,
        mixSlider);

    configureRotarySlider(bitDepthSlider);
    bitDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 88, 24);
    addAndMakeVisible(bitDepthSlider);

    configureKnobLabel(bitDepthLabel, "Bits");
    addAndMakeVisible(bitDepthLabel);

    bitDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::bitcrusherDepth,
        bitDepthSlider);

    configureRotarySlider(bitRateSlider);
    addAndMakeVisible(bitRateSlider);

    configureKnobLabel(bitRateLabel, "Rate");
    addAndMakeVisible(bitRateLabel);

    bitRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::bitcrusherRate,
        bitRateSlider);

    configureRotarySlider(bitMixSlider);
    addAndMakeVisible(bitMixSlider);

    configureKnobLabel(bitMixLabel, "Crush Mix");
    addAndMakeVisible(bitMixLabel);

    bitMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::bitcrusherMix,
        bitMixSlider);

    configureRotarySlider(chorusRateSlider);
    addAndMakeVisible(chorusRateSlider);

    configureKnobLabel(chorusRateLabel, "Ch Rate");
    addAndMakeVisible(chorusRateLabel);

    chorusRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::chorusRate,
        chorusRateSlider);

    configureRotarySlider(chorusDepthSlider);
    addAndMakeVisible(chorusDepthSlider);

    configureKnobLabel(chorusDepthLabel, "Depth");
    addAndMakeVisible(chorusDepthLabel);

    chorusDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::chorusDepth,
        chorusDepthSlider);

    configureRotarySlider(chorusMixSlider);
    addAndMakeVisible(chorusMixSlider);

    configureKnobLabel(chorusMixLabel, "Ch Mix");
    addAndMakeVisible(chorusMixLabel);

    chorusMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::chorusMix,
        chorusMixSlider);

    configureRotarySlider(delayTimeSlider);
    addAndMakeVisible(delayTimeSlider);

    configureKnobLabel(delayTimeLabel, "Time");
    addAndMakeVisible(delayTimeLabel);

    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::delayTime,
        delayTimeSlider);

    configureRotarySlider(delayFeedbackSlider);
    addAndMakeVisible(delayFeedbackSlider);

    configureKnobLabel(delayFeedbackLabel, "Feedback");
    addAndMakeVisible(delayFeedbackLabel);

    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::delayFeedback,
        delayFeedbackSlider);

    configureRotarySlider(delayMixSlider);
    addAndMakeVisible(delayMixSlider);

    configureKnobLabel(delayMixLabel, "Delay Mix");
    addAndMakeVisible(delayMixLabel);

    delayMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::delayMix,
        delayMixSlider);

    configureRotarySlider(reverbRoomSlider);
    addAndMakeVisible(reverbRoomSlider);

    configureKnobLabel(reverbRoomLabel, "Room");
    addAndMakeVisible(reverbRoomLabel);

    reverbRoomAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::reverbRoomSize,
        reverbRoomSlider);

    configureRotarySlider(reverbDampingSlider);
    addAndMakeVisible(reverbDampingSlider);

    configureKnobLabel(reverbDampingLabel, "Damping");
    addAndMakeVisible(reverbDampingLabel);

    reverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::reverbDamping,
        reverbDampingSlider);

    configureRotarySlider(reverbWidthSlider);
    addAndMakeVisible(reverbWidthSlider);

    configureKnobLabel(reverbWidthLabel, "Width");
    addAndMakeVisible(reverbWidthLabel);

    reverbWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::reverbWidth,
        reverbWidthSlider);

    configureRotarySlider(reverbMixSlider);
    addAndMakeVisible(reverbMixSlider);

    configureKnobLabel(reverbMixLabel, "Verb Mix");
    addAndMakeVisible(reverbMixLabel);

    reverbMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::reverbMix,
        reverbMixSlider);

    restoreGenerationControls();

    setSize(900, 940);
}

ToneflxLiteAudioProcessorEditor::~ToneflxLiteAudioProcessorEditor() = default;

juce::StringArray ToneflxLiteAudioProcessorEditor::collectSelectedDescriptors() const
{
    juce::StringArray descriptors;

    if (warmButton.getToggleState())
        descriptors.add("warm");

    if (darkButton.getToggleState())
        descriptors.add("dark");

    if (vintageButton.getToggleState())
        descriptors.add("vintage");

    if (wideButton.getToggleState())
        descriptors.add("wide");

    if (dreamyButton.getToggleState())
        descriptors.add("dreamy");

    return descriptors;
}

void ToneflxLiteAudioProcessorEditor::restoreGenerationControls()
{
    if (! audioProcessor.hasGenerationMetadata())
        return;

    const auto seed = audioProcessor.getLastGenerationSeed();
    seedLabel.setText("Seed " + juce::String(seed), juce::dontSendNotification);
    seedEditor.setText(juce::String(seed), juce::dontSendNotification);
    setDescriptorToggleStates(audioProcessor.getLastGenerationDescriptors());
}

void ToneflxLiteAudioProcessorEditor::setDescriptorToggleStates(const juce::StringArray& descriptors)
{
    warmButton.setToggleState(descriptors.contains("warm"), juce::dontSendNotification);
    darkButton.setToggleState(descriptors.contains("dark"), juce::dontSendNotification);
    vintageButton.setToggleState(descriptors.contains("vintage"), juce::dontSendNotification);
    wideButton.setToggleState(descriptors.contains("wide"), juce::dontSendNotification);
    dreamyButton.setToggleState(descriptors.contains("dreamy"), juce::dontSendNotification);
}

void ToneflxLiteAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    auto bounds = getLocalBounds().toFloat().reduced(24.0f);
    g.setColour(panelColour);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(accentColour);
    g.fillRoundedRectangle(bounds.withHeight(4.0f), 2.0f);
}

void ToneflxLiteAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(32);
    titleLabel.setBounds(bounds.removeFromTop(64));
    statusLabel.setBounds(bounds.removeFromTop(28));

    bounds.removeFromTop(16);
    auto descriptorRow = bounds.removeFromTop(36).withSizeKeepingCentre(622, 36);
    constexpr auto descriptorButtonWidth = 88;
    constexpr auto descriptorGap = 10;

    warmButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    darkButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    vintageButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    wideButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    dreamyButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(24);
    generateButton.setBounds(descriptorRow.removeFromLeft(118));

    bounds.removeFromTop(10);
    auto seedRow = bounds.removeFromTop(32).withSizeKeepingCentre(430, 32);
    seedLabel.setBounds(seedRow.removeFromLeft(150));
    seedRow.removeFromLeft(12);
    seedEditor.setBounds(seedRow.removeFromLeft(150));
    seedRow.removeFromLeft(12);
    recallButton.setBounds(seedRow.removeFromLeft(106));

    bounds.removeFromTop(20);
    auto firstRow = bounds.removeFromTop(110).withSizeKeepingCentre(340, 110);
    bounds.removeFromTop(16);
    auto secondRow = bounds.removeFromTop(110).withSizeKeepingCentre(530, 110);
    bounds.removeFromTop(16);
    auto thirdRow = bounds.removeFromTop(110).withSizeKeepingCentre(530, 110);
    bounds.removeFromTop(16);
    auto fourthRow = bounds.removeFromTop(110).withSizeKeepingCentre(530, 110);
    bounds.removeFromTop(16);
    auto fifthRow = bounds.removeFromTop(110).withSizeKeepingCentre(720, 110);

    auto driveArea = firstRow.removeFromLeft(150);
    firstRow.removeFromLeft(40);
    auto mixArea = firstRow.removeFromLeft(150);

    auto bitDepthArea = secondRow.removeFromLeft(150);
    secondRow.removeFromLeft(40);
    auto bitRateArea = secondRow.removeFromLeft(150);
    secondRow.removeFromLeft(40);
    auto bitMixArea = secondRow.removeFromLeft(150);

    auto chorusRateArea = thirdRow.removeFromLeft(150);
    thirdRow.removeFromLeft(40);
    auto chorusDepthArea = thirdRow.removeFromLeft(150);
    thirdRow.removeFromLeft(40);
    auto chorusMixArea = thirdRow.removeFromLeft(150);

    auto delayTimeArea = fourthRow.removeFromLeft(150);
    fourthRow.removeFromLeft(40);
    auto delayFeedbackArea = fourthRow.removeFromLeft(150);
    fourthRow.removeFromLeft(40);
    auto delayMixArea = fourthRow.removeFromLeft(150);

    auto reverbRoomArea = fifthRow.removeFromLeft(150);
    fifthRow.removeFromLeft(40);
    auto reverbDampingArea = fifthRow.removeFromLeft(150);
    fifthRow.removeFromLeft(40);
    auto reverbWidthArea = fifthRow.removeFromLeft(150);
    fifthRow.removeFromLeft(40);
    auto reverbMixArea = fifthRow.removeFromLeft(150);

    driveLabel.setBounds(driveArea.removeFromTop(24));
    driveSlider.setBounds(driveArea);

    mixLabel.setBounds(mixArea.removeFromTop(24));
    mixSlider.setBounds(mixArea);

    bitDepthLabel.setBounds(bitDepthArea.removeFromTop(24));
    bitDepthSlider.setBounds(bitDepthArea);

    bitRateLabel.setBounds(bitRateArea.removeFromTop(24));
    bitRateSlider.setBounds(bitRateArea);

    bitMixLabel.setBounds(bitMixArea.removeFromTop(24));
    bitMixSlider.setBounds(bitMixArea);

    chorusRateLabel.setBounds(chorusRateArea.removeFromTop(24));
    chorusRateSlider.setBounds(chorusRateArea);

    chorusDepthLabel.setBounds(chorusDepthArea.removeFromTop(24));
    chorusDepthSlider.setBounds(chorusDepthArea);

    chorusMixLabel.setBounds(chorusMixArea.removeFromTop(24));
    chorusMixSlider.setBounds(chorusMixArea);

    delayTimeLabel.setBounds(delayTimeArea.removeFromTop(24));
    delayTimeSlider.setBounds(delayTimeArea);

    delayFeedbackLabel.setBounds(delayFeedbackArea.removeFromTop(24));
    delayFeedbackSlider.setBounds(delayFeedbackArea);

    delayMixLabel.setBounds(delayMixArea.removeFromTop(24));
    delayMixSlider.setBounds(delayMixArea);

    reverbRoomLabel.setBounds(reverbRoomArea.removeFromTop(24));
    reverbRoomSlider.setBounds(reverbRoomArea);

    reverbDampingLabel.setBounds(reverbDampingArea.removeFromTop(24));
    reverbDampingSlider.setBounds(reverbDampingArea);

    reverbWidthLabel.setBounds(reverbWidthArea.removeFromTop(24));
    reverbWidthSlider.setBounds(reverbWidthArea);

    reverbMixLabel.setBounds(reverbMixArea.removeFromTop(24));
    reverbMixSlider.setBounds(reverbMixArea);
}
