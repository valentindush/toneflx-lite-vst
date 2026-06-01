#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "generators/PresetEngine.h"

ToneflxLiteAudioProcessor::ToneflxLiteAudioProcessor()
    : AudioProcessor(BusesProperties()
       #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
        #endif
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
       #endif
      ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    saturationDriveParameter = parameters.getRawParameterValue(ToneflxParameters::saturationDrive);
    saturationMixParameter = parameters.getRawParameterValue(ToneflxParameters::saturationMix);
    bitcrusherDepthParameter = parameters.getRawParameterValue(ToneflxParameters::bitcrusherDepth);
    bitcrusherRateParameter = parameters.getRawParameterValue(ToneflxParameters::bitcrusherRate);
    bitcrusherMixParameter = parameters.getRawParameterValue(ToneflxParameters::bitcrusherMix);
    chorusRateParameter = parameters.getRawParameterValue(ToneflxParameters::chorusRate);
    chorusDepthParameter = parameters.getRawParameterValue(ToneflxParameters::chorusDepth);
    chorusMixParameter = parameters.getRawParameterValue(ToneflxParameters::chorusMix);
    delayTimeParameter = parameters.getRawParameterValue(ToneflxParameters::delayTime);
    delayFeedbackParameter = parameters.getRawParameterValue(ToneflxParameters::delayFeedback);
    delayMixParameter = parameters.getRawParameterValue(ToneflxParameters::delayMix);
    reverbRoomSizeParameter = parameters.getRawParameterValue(ToneflxParameters::reverbRoomSize);
    reverbDampingParameter = parameters.getRawParameterValue(ToneflxParameters::reverbDamping);
    reverbWidthParameter = parameters.getRawParameterValue(ToneflxParameters::reverbWidth);
    reverbMixParameter = parameters.getRawParameterValue(ToneflxParameters::reverbMix);
}

ToneflxLiteAudioProcessor::~ToneflxLiteAudioProcessor() = default;

const juce::String ToneflxLiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ToneflxLiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ToneflxLiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ToneflxLiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ToneflxLiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ToneflxLiteAudioProcessor::getNumPrograms()
{
    return 1;
}

int ToneflxLiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ToneflxLiteAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ToneflxLiteAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void ToneflxLiteAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void ToneflxLiteAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    saturation.prepare(spec);
    bitcrusher.prepare(spec);
    chorus.prepare(spec);
    delay.prepare(spec);
    reverb.prepare(spec);
}

void ToneflxLiteAudioProcessor::releaseResources()
{
    saturation.reset();
    bitcrusher.reset();
    chorus.reset();
    delay.reset();
    reverb.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ToneflxLiteAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono()
        && mainOutput != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (mainOutput != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
}
#endif

void ToneflxLiteAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    for (auto channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());

    if (saturationDriveParameter != nullptr)
        saturation.setDrive(saturationDriveParameter->load());

    if (saturationMixParameter != nullptr)
        saturation.setMix(saturationMixParameter->load());

    saturation.process(buffer);

    if (bitcrusherDepthParameter != nullptr)
        bitcrusher.setBitDepth(juce::roundToInt(bitcrusherDepthParameter->load()));

    if (bitcrusherRateParameter != nullptr)
        bitcrusher.setRateReduction(juce::roundToInt(bitcrusherRateParameter->load()));

    if (bitcrusherMixParameter != nullptr)
        bitcrusher.setMix(bitcrusherMixParameter->load());

    bitcrusher.process(buffer);

    if (chorusRateParameter != nullptr)
        chorus.setRateHz(chorusRateParameter->load());

    if (chorusDepthParameter != nullptr)
        chorus.setDepthMs(chorusDepthParameter->load());

    if (chorusMixParameter != nullptr)
        chorus.setMix(chorusMixParameter->load());

    chorus.process(buffer);

    if (delayTimeParameter != nullptr)
        delay.setDelayTimeMs(delayTimeParameter->load());

    if (delayFeedbackParameter != nullptr)
        delay.setFeedback(delayFeedbackParameter->load());

    if (delayMixParameter != nullptr)
        delay.setMix(delayMixParameter->load());

    delay.process(buffer);

    if (reverbRoomSizeParameter != nullptr)
        reverb.setRoomSize(reverbRoomSizeParameter->load());

    if (reverbDampingParameter != nullptr)
        reverb.setDamping(reverbDampingParameter->load());

    if (reverbWidthParameter != nullptr)
        reverb.setWidth(reverbWidthParameter->load());

    if (reverbMixParameter != nullptr)
        reverb.setMix(reverbMixParameter->load());

    reverb.process(buffer);
}

bool ToneflxLiteAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ToneflxLiteAudioProcessor::createEditor()
{
    return new ToneflxLiteAudioProcessorEditor(*this);
}

void ToneflxLiteAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState(); auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void ToneflxLiteAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

std::uint32_t ToneflxLiteAudioProcessor::generatePresetForDescriptors(const juce::StringArray& descriptors)
{
    const auto seed = static_cast<std::uint32_t>(juce::Random::getSystemRandom().nextInt64());
    const auto preset = PresetEngine::generate(descriptors, seed);
    applyGeneratedPreset(preset);

    return seed;
}

std::uint32_t ToneflxLiteAudioProcessor::generatePresetForDescriptors(const juce::StringArray& descriptors,
                                                                      std::uint32_t seed)
{
    const auto preset = PresetEngine::generate(descriptors, seed);
    applyGeneratedPreset(preset);

    return seed;
}

void ToneflxLiteAudioProcessor::applyGeneratedPreset(const GeneratedPreset& preset)
{
    setParameterValue(ToneflxParameters::saturationDrive, preset.saturationDrive);
    setParameterValue(ToneflxParameters::saturationMix, preset.saturationMix);
    setParameterValue(ToneflxParameters::bitcrusherDepth, static_cast<float>(preset.bitcrusherDepth));
    setParameterValue(ToneflxParameters::bitcrusherRate, static_cast<float>(preset.bitcrusherRate));
    setParameterValue(ToneflxParameters::bitcrusherMix, preset.bitcrusherMix);
    setParameterValue(ToneflxParameters::chorusRate, preset.chorusRate);
    setParameterValue(ToneflxParameters::chorusDepth, preset.chorusDepth);
    setParameterValue(ToneflxParameters::chorusMix, preset.chorusMix);
    setParameterValue(ToneflxParameters::delayTime, preset.delayTime);
    setParameterValue(ToneflxParameters::delayFeedback, preset.delayFeedback);
    setParameterValue(ToneflxParameters::delayMix, preset.delayMix);
    setParameterValue(ToneflxParameters::reverbRoomSize, preset.reverbRoomSize);
    setParameterValue(ToneflxParameters::reverbDamping, preset.reverbDamping);
    setParameterValue(ToneflxParameters::reverbWidth, preset.reverbWidth);
    setParameterValue(ToneflxParameters::reverbMix, preset.reverbMix);
}

void ToneflxLiteAudioProcessor::setParameterValue(const juce::String& parameterID, float value)
{
    if (auto* parameter = parameters.getParameter(parameterID))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
        parameter->endChangeGesture();
    }
}

ToneflxLiteAudioProcessor::APVTS::ParameterLayout ToneflxLiteAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::saturationDrive, 1 },
        "Saturation Drive",
        juce::NormalisableRange<float> { 1.0f, 12.0f, 0.01f, 0.55f },
        2.5f,
        juce::AudioParameterFloatAttributes().withLabel("x")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::saturationMix, 1 },
        "Saturation Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        1.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { ToneflxParameters::bitcrusherDepth, 1 },
        "Bitcrusher Depth",
        4,
        16,
        12,
        juce::AudioParameterIntAttributes().withLabel("bit")));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { ToneflxParameters::bitcrusherRate, 1 },
        "Bitcrusher Rate",
        1,
        32,
        1,
        juce::AudioParameterIntAttributes().withLabel("x")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::bitcrusherMix, 1 },
        "Bitcrusher Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.35f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::chorusRate, 1 },
        "Chorus Rate",
        juce::NormalisableRange<float> { 0.05f, 5.0f, 0.01f, 0.45f },
        0.35f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::chorusDepth, 1 },
        "Chorus Depth",
        juce::NormalisableRange<float> { 0.0f, 20.0f, 0.1f },
        8.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::chorusMix, 1 },
        "Chorus Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.25f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::delayTime, 1 },
        "Delay Time",
        juce::NormalisableRange<float> { 10.0f, 2000.0f, 1.0f, 0.45f },
        320.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::delayFeedback, 1 },
        "Delay Feedback",
        juce::NormalisableRange<float> { 0.0f, 0.92f, 0.01f },
        0.28f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::delayMix, 1 },
        "Delay Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.25f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::reverbRoomSize, 1 },
        "Reverb Room Size",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.45f,
        juce::AudioParameterFloatAttributes()));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::reverbDamping, 1 },
        "Reverb Damping",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.35f,
        juce::AudioParameterFloatAttributes()));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::reverbWidth, 1 },
        "Reverb Width",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.8f,
        juce::AudioParameterFloatAttributes()));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ToneflxParameters::reverbMix, 1 },
        "Reverb Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.2f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](auto value, auto) {
                return juce::String(juce::roundToInt(value * 100.0f));
            })
            .withValueFromStringFunction([](const auto& text) {
                return text.getFloatValue() / 100.0f;
            })));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ToneflxLiteAudioProcessor();
}
