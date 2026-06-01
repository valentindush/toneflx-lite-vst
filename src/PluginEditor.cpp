#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "utils/PresetJson.h"

namespace
{
const auto backgroundColour = juce::Colour(0xff121212);
const auto panelColour = juce::Colour(0xff1a1a1a);
const auto accentColour = juce::Colour(0xffff8a3d);
const auto textColour = juce::Colour(0xffeaeaea);
const auto secondaryTextColour = juce::Colour(0xff8a8a8a);
const auto sectionTextColour = juce::Colour(0xffffb17a);

void configureRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff333333));
    slider.setColour(juce::Slider::thumbColourId, textColour);
    slider.setColour(juce::Slider::textBoxTextColourId, textColour);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
}

void configureKnobLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, textColour);
    label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
}

void configureSectionLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, sectionTextColour);
    label.setFont(juce::FontOptions(13.0f, juce::Font::bold));
}

void configureDescriptorButton(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
    button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff242424));
    button.setColour(juce::TextButton::buttonOnColourId, accentColour);
    button.setColour(juce::TextButton::textColourOffId, textColour);
    button.setColour(juce::TextButton::textColourOnId, backgroundColour);
}

void layoutKnob(juce::Rectangle<int> area, juce::Label& label, juce::Slider& slider)
{
    label.setBounds(area.removeFromTop(18));
    slider.setBounds(area);
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

    configureSectionLabel(saturationSectionLabel, "SATURATION");
    addAndMakeVisible(saturationSectionLabel);

    configureSectionLabel(bitcrusherSectionLabel, "BITCRUSHER");
    addAndMakeVisible(bitcrusherSectionLabel);

    configureSectionLabel(chorusSectionLabel, "CHORUS");
    addAndMakeVisible(chorusSectionLabel);

    configureSectionLabel(delaySectionLabel, "DELAY");
    addAndMakeVisible(delaySectionLabel);

    configureSectionLabel(reverbSectionLabel, "REVERB");
    addAndMakeVisible(reverbSectionLabel);

    configureSectionLabel(outputSectionLabel, "OUTPUT");
    addAndMakeVisible(outputSectionLabel);

    for (auto* button : { &warmButton, &darkButton, &vintageButton, &harshButton, &wideButton, &dreamyButton })
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

    presetNameEditor.setTextToShowWhenEmpty("preset name", secondaryTextColour);
    presetNameEditor.setJustification(juce::Justification::centredLeft);
    presetNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff242424));
    presetNameEditor.setColour(juce::TextEditor::textColourId, textColour);
    presetNameEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    presetNameEditor.setColour(juce::TextEditor::focusedOutlineColourId, accentColour);
    addAndMakeVisible(presetNameEditor);

    savePresetButton.setColour(juce::TextButton::buttonColourId, accentColour);
    savePresetButton.setColour(juce::TextButton::textColourOffId, backgroundColour);
    savePresetButton.onClick = [this] { saveCurrentPreset(); };
    addAndMakeVisible(savePresetButton);

    presetMenu.setTextWhenNothingSelected("saved presets");
    presetMenu.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff242424));
    presetMenu.setColour(juce::ComboBox::textColourId, textColour);
    presetMenu.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    presetMenu.setColour(juce::ComboBox::arrowColourId, accentColour);
    addAndMakeVisible(presetMenu);

    loadPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff242424));
    loadPresetButton.setColour(juce::TextButton::textColourOffId, textColour);
    loadPresetButton.onClick = [this] { loadSelectedPreset(); };
    addAndMakeVisible(loadPresetButton);

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

    configureRotarySlider(outputTrimSlider);
    addAndMakeVisible(outputTrimSlider);

    configureKnobLabel(outputTrimLabel, "Trim");
    addAndMakeVisible(outputTrimLabel);

    outputTrimAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters,
        ToneflxParameters::outputTrim,
        outputTrimSlider);

    restoreGenerationControls();
    refreshPresetMenu();

    setSize(760, 560);
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

    if (harshButton.getToggleState())
        descriptors.add("harsh");

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
    harshButton.setToggleState(descriptors.contains("harsh"), juce::dontSendNotification);
    wideButton.setToggleState(descriptors.contains("wide"), juce::dontSendNotification);
    dreamyButton.setToggleState(descriptors.contains("dreamy"), juce::dontSendNotification);
}

void ToneflxLiteAudioProcessorEditor::refreshPresetMenu()
{
    presetMenu.clear(juce::dontSendNotification);

    juce::Array<juce::File> presetFiles;
    getPresetDirectory().findChildFiles(presetFiles, juce::File::findFiles, false, "*.toneflx.json");

    auto itemId = 1;
    for (const auto& file : presetFiles)
        presetMenu.addItem(file.getFileNameWithoutExtension().replace(".toneflx", ""), itemId++);
}

void ToneflxLiteAudioProcessorEditor::saveCurrentPreset()
{
    const auto name = presetNameEditor.getText().trim().isEmpty()
        ? juce::String("Untitled")
        : presetNameEditor.getText().trim();

    const auto snapshot = audioProcessor.createPresetSnapshot(name);

    if (PresetJson::saveToFile(snapshot, getPresetFileForName(snapshot.name)))
    {
        presetNameEditor.setText(snapshot.name, juce::dontSendNotification);
        refreshPresetMenu();
        presetMenu.setText(snapshot.name, juce::dontSendNotification);
    }
}

void ToneflxLiteAudioProcessorEditor::loadSelectedPreset()
{
    const auto name = presetMenu.getText().trim();

    if (name.isEmpty())
        return;

    PresetSnapshot snapshot;
    if (! PresetJson::loadFromFile(getPresetFileForName(name), snapshot))
        return;

    audioProcessor.applyPresetSnapshot(snapshot);
    presetNameEditor.setText(snapshot.name, juce::dontSendNotification);
    seedLabel.setText("Seed " + juce::String(snapshot.preset.seed), juce::dontSendNotification);
    seedEditor.setText(juce::String(snapshot.preset.seed), juce::dontSendNotification);
    setDescriptorToggleStates(snapshot.descriptors);
}

juce::File ToneflxLiteAudioProcessorEditor::getPresetDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("TONEFLX Lite Presets");
}

juce::File ToneflxLiteAudioProcessorEditor::getPresetFileForName(const juce::String& name) const
{
    const auto legalName = juce::File::createLegalFileName(name.trim().isEmpty() ? "Untitled" : name.trim());
    return getPresetDirectory().getChildFile(legalName + ".toneflx.json");
}

void ToneflxLiteAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    auto bounds = getLocalBounds().toFloat().reduced(16.0f);
    g.setColour(panelColour);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(accentColour);
    g.fillRoundedRectangle(bounds.withHeight(4.0f), 2.0f);
}

void ToneflxLiteAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(30, 24);
    auto header = bounds.removeFromTop(50);
    titleLabel.setBounds(header.removeFromTop(30));
    statusLabel.setBounds(header);

    bounds.removeFromTop(10);
    auto descriptorRow = bounds.removeFromTop(32).withSizeKeepingCentre(564, 32);
    constexpr auto descriptorButtonWidth = 68;
    constexpr auto descriptorGap = 6;

    warmButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    darkButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    vintageButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    harshButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    wideButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(descriptorGap);
    dreamyButton.setBounds(descriptorRow.removeFromLeft(descriptorButtonWidth));
    descriptorRow.removeFromLeft(18);
    generateButton.setBounds(descriptorRow.removeFromLeft(108));

    bounds.removeFromTop(8);
    auto seedRow = bounds.removeFromTop(28).withSizeKeepingCentre(392, 28);
    seedLabel.setBounds(seedRow.removeFromLeft(132));
    seedRow.removeFromLeft(12);
    seedEditor.setBounds(seedRow.removeFromLeft(130));
    seedRow.removeFromLeft(12);
    recallButton.setBounds(seedRow.removeFromLeft(94));

    constexpr auto groupGap = 14;
    constexpr auto knobWidth = 68;
    constexpr auto knobGap = 8;
    constexpr auto groupHeaderHeight = 22;
    constexpr auto moduleHeight = 116;

    bounds.removeFromTop(8);
    auto presetRow = bounds.removeFromTop(28).withSizeKeepingCentre(560, 28);
    presetNameEditor.setBounds(presetRow.removeFromLeft(154));
    presetRow.removeFromLeft(8);
    savePresetButton.setBounds(presetRow.removeFromLeft(76));
    presetRow.removeFromLeft(16);
    presetMenu.setBounds(presetRow.removeFromLeft(206));
    presetRow.removeFromLeft(8);
    loadPresetButton.setBounds(presetRow.removeFromLeft(82));

    bounds.removeFromTop(12);
    auto firstRow = bounds.removeFromTop(moduleHeight).withSizeKeepingCentre(634, moduleHeight);
    auto saturationGroup = firstRow.removeFromLeft(144);
    firstRow.removeFromLeft(groupGap);
    auto bitcrusherGroup = firstRow.removeFromLeft(220);
    firstRow.removeFromLeft(groupGap);
    auto chorusGroup = firstRow.removeFromLeft(220);

    bounds.removeFromTop(16);
    auto secondRow = bounds.removeFromTop(moduleHeight).withSizeKeepingCentre(606, moduleHeight);
    auto delayGroup = secondRow.removeFromLeft(220);
    secondRow.removeFromLeft(groupGap);
    auto reverbGroup = secondRow.removeFromLeft(290);
    secondRow.removeFromLeft(groupGap);
    auto outputGroup = secondRow.removeFromLeft(68);

    saturationSectionLabel.setBounds(saturationGroup.removeFromTop(groupHeaderHeight));
    bitcrusherSectionLabel.setBounds(bitcrusherGroup.removeFromTop(groupHeaderHeight));
    chorusSectionLabel.setBounds(chorusGroup.removeFromTop(groupHeaderHeight));
    delaySectionLabel.setBounds(delayGroup.removeFromTop(groupHeaderHeight));
    reverbSectionLabel.setBounds(reverbGroup.removeFromTop(groupHeaderHeight));
    outputSectionLabel.setBounds(outputGroup.removeFromTop(groupHeaderHeight));

    auto driveArea = saturationGroup.removeFromLeft(knobWidth);
    saturationGroup.removeFromLeft(knobGap);
    auto mixArea = saturationGroup.removeFromLeft(knobWidth);

    auto bitDepthArea = bitcrusherGroup.removeFromLeft(knobWidth);
    bitcrusherGroup.removeFromLeft(knobGap);
    auto bitRateArea = bitcrusherGroup.removeFromLeft(knobWidth);
    bitcrusherGroup.removeFromLeft(knobGap);
    auto bitMixArea = bitcrusherGroup.removeFromLeft(knobWidth);

    auto chorusRateArea = chorusGroup.removeFromLeft(knobWidth);
    chorusGroup.removeFromLeft(knobGap);
    auto chorusDepthArea = chorusGroup.removeFromLeft(knobWidth);
    chorusGroup.removeFromLeft(knobGap);
    auto chorusMixArea = chorusGroup.removeFromLeft(knobWidth);

    auto delayTimeArea = delayGroup.removeFromLeft(knobWidth);
    delayGroup.removeFromLeft(knobGap);
    auto delayFeedbackArea = delayGroup.removeFromLeft(knobWidth);
    delayGroup.removeFromLeft(knobGap);
    auto delayMixArea = delayGroup.removeFromLeft(knobWidth);

    auto reverbRoomArea = reverbGroup.removeFromLeft(knobWidth);
    reverbGroup.removeFromLeft(6);
    auto reverbDampingArea = reverbGroup.removeFromLeft(knobWidth);
    reverbGroup.removeFromLeft(6);
    auto reverbWidthArea = reverbGroup.removeFromLeft(knobWidth);
    reverbGroup.removeFromLeft(6);
    auto reverbMixArea = reverbGroup.removeFromLeft(knobWidth);
    auto outputTrimArea = outputGroup.removeFromLeft(knobWidth);

    layoutKnob(driveArea, driveLabel, driveSlider);
    layoutKnob(mixArea, mixLabel, mixSlider);
    layoutKnob(bitDepthArea, bitDepthLabel, bitDepthSlider);
    layoutKnob(bitRateArea, bitRateLabel, bitRateSlider);
    layoutKnob(bitMixArea, bitMixLabel, bitMixSlider);
    layoutKnob(chorusRateArea, chorusRateLabel, chorusRateSlider);
    layoutKnob(chorusDepthArea, chorusDepthLabel, chorusDepthSlider);
    layoutKnob(chorusMixArea, chorusMixLabel, chorusMixSlider);
    layoutKnob(delayTimeArea, delayTimeLabel, delayTimeSlider);
    layoutKnob(delayFeedbackArea, delayFeedbackLabel, delayFeedbackSlider);
    layoutKnob(delayMixArea, delayMixLabel, delayMixSlider);
    layoutKnob(reverbRoomArea, reverbRoomLabel, reverbRoomSlider);
    layoutKnob(reverbDampingArea, reverbDampingLabel, reverbDampingSlider);
    layoutKnob(reverbWidthArea, reverbWidthLabel, reverbWidthSlider);
    layoutKnob(reverbMixArea, reverbMixLabel, reverbMixSlider);
    layoutKnob(outputTrimArea, outputTrimLabel, outputTrimSlider);
}
