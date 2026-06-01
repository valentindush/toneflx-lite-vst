#pragma once

#include <JuceHeader.h>

class ToneflxLiteAudioProcessor;

class ToneflxLiteAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit ToneflxLiteAudioProcessorEditor(ToneflxLiteAudioProcessor&);
    ~ToneflxLiteAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ToneflxLiteAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::TextButton warmButton { "warm" };
    juce::TextButton darkButton { "dark" };
    juce::TextButton vintageButton { "vintage" };
    juce::TextButton wideButton { "wide" };
    juce::TextButton dreamyButton { "dreamy" };
    juce::TextButton generateButton { "GENERATE" };
    juce::TextButton recallButton { "RECALL" };
    juce::Label seedLabel;
    juce::TextEditor seedEditor;
    juce::Slider driveSlider;
    juce::Slider mixSlider;
    juce::Slider bitDepthSlider;
    juce::Slider bitRateSlider;
    juce::Slider bitMixSlider;
    juce::Slider chorusRateSlider;
    juce::Slider chorusDepthSlider;
    juce::Slider chorusMixSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Slider delayMixSlider;
    juce::Slider reverbRoomSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWidthSlider;
    juce::Slider reverbMixSlider;
    juce::Label driveLabel;
    juce::Label mixLabel;
    juce::Label bitDepthLabel;
    juce::Label bitRateLabel;
    juce::Label bitMixLabel;
    juce::Label chorusRateLabel;
    juce::Label chorusDepthLabel;
    juce::Label chorusMixLabel;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    juce::Label delayMixLabel;
    juce::Label reverbRoomLabel;
    juce::Label reverbDampingLabel;
    juce::Label reverbWidthLabel;
    juce::Label reverbMixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbRoomAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToneflxLiteAudioProcessorEditor)
};
