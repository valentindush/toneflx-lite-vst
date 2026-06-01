#pragma once

#include <JuceHeader.h>
#include "dsp/Bitcrusher.h"
#include "dsp/Chorus.h"
#include "dsp/Delay.h"
#include "dsp/Reverb.h"
#include "dsp/Saturation.h"
#include "models/Preset.h"

namespace ToneflxParameters
{
static constexpr auto saturationDrive = "saturationDrive";
static constexpr auto saturationMix = "saturationMix";
static constexpr auto bitcrusherDepth = "bitcrusherDepth";
static constexpr auto bitcrusherRate = "bitcrusherRate";
static constexpr auto bitcrusherMix = "bitcrusherMix";
static constexpr auto chorusRate = "chorusRate";
static constexpr auto chorusDepth = "chorusDepth";
static constexpr auto chorusMix = "chorusMix";
static constexpr auto delayTime = "delayTime";
static constexpr auto delayFeedback = "delayFeedback";
static constexpr auto delayMix = "delayMix";
static constexpr auto reverbRoomSize = "reverbRoomSize";
static constexpr auto reverbDamping = "reverbDamping";
static constexpr auto reverbWidth = "reverbWidth";
static constexpr auto reverbMix = "reverbMix";
}

class ToneflxLiteAudioProcessor final : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;

    ToneflxLiteAudioProcessor();
    ~ToneflxLiteAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    std::uint32_t generatePresetForDescriptors(const juce::StringArray& descriptors);
    std::uint32_t generatePresetForDescriptors(const juce::StringArray& descriptors, std::uint32_t seed);
    bool hasGenerationMetadata() const;
    std::uint32_t getLastGenerationSeed() const;
    juce::StringArray getLastGenerationDescriptors() const;
    PresetSnapshot createPresetSnapshot(const juce::String& name) const;
    void applyPresetSnapshot(const PresetSnapshot& snapshot);

    APVTS parameters;

private:
    static APVTS::ParameterLayout createParameterLayout();
    float getParameterValue(const juce::String& parameterID) const;
    void applyGeneratedPreset(const GeneratedPreset& preset);
    void storeGenerationMetadata(const juce::StringArray& descriptors, std::uint32_t seed);
    void setParameterValue(const juce::String& parameterID, float value);

    SaturationModule saturation;
    BitcrusherModule bitcrusher;
    ChorusModule chorus;
    DelayModule delay;
    ReverbModule reverb;
    std::atomic<float>* saturationDriveParameter = nullptr;
    std::atomic<float>* saturationMixParameter = nullptr;
    std::atomic<float>* bitcrusherDepthParameter = nullptr;
    std::atomic<float>* bitcrusherRateParameter = nullptr;
    std::atomic<float>* bitcrusherMixParameter = nullptr;
    std::atomic<float>* chorusRateParameter = nullptr;
    std::atomic<float>* chorusDepthParameter = nullptr;
    std::atomic<float>* chorusMixParameter = nullptr;
    std::atomic<float>* delayTimeParameter = nullptr;
    std::atomic<float>* delayFeedbackParameter = nullptr;
    std::atomic<float>* delayMixParameter = nullptr;
    std::atomic<float>* reverbRoomSizeParameter = nullptr;
    std::atomic<float>* reverbDampingParameter = nullptr;
    std::atomic<float>* reverbWidthParameter = nullptr;
    std::atomic<float>* reverbMixParameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToneflxLiteAudioProcessor)
};
