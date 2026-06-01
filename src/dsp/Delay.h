#pragma once

#include <JuceHeader.h>

class DelayModule
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        numChannels = spec.numChannels;

        const auto maxDelaySamples = static_cast<int>(sampleRate * maxDelayTimeSeconds);
        delayBuffer.setSize(static_cast<int>(numChannels), maxDelaySamples + 1);
        reset();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const auto delaySamples = juce::jlimit(1, delayBuffer.getNumSamples() - 1,
                                              static_cast<int>((delayTimeMs / 1000.0f) * static_cast<float>(sampleRate)));
        const auto currentFeedback = feedback;
        const auto currentMix = mix;
        const auto delayBufferSize = delayBuffer.getNumSamples();

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto readPosition = (writePosition + delayBufferSize - delaySamples) % delayBufferSize;

            for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                auto* delayData = delayBuffer.getWritePointer(channel);
                const auto dry = channelData[sample];
                const auto delayed = delayData[readPosition];

                delayData[writePosition] = dry + (delayed * currentFeedback);
                channelData[sample] = dry + ((delayed - dry) * currentMix);
            }

            writePosition = (writePosition + 1) % delayBufferSize;
        }
    }

    void reset()
    {
        delayBuffer.clear();
        writePosition = 0;
    }

    void setDelayTimeMs(float newDelayTimeMs)
    {
        delayTimeMs = juce::jlimit(10.0f, maxDelayTimeSeconds * 1000.0f, newDelayTimeMs);
    }

    float getDelayTimeMs() const
    {
        return delayTimeMs;
    }

    void setFeedback(float newFeedback)
    {
        feedback = juce::jlimit(0.0f, 0.92f, newFeedback);
    }

    float getFeedback() const
    {
        return feedback;
    }

    void setMix(float newMix)
    {
        mix = juce::jlimit(0.0f, 1.0f, newMix);
    }

    float getMix() const
    {
        return mix;
    }

private:
    static constexpr auto maxDelayTimeSeconds = 2.0f;

    double sampleRate = 44100.0;
    juce::uint32 numChannels = 0;
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    float delayTimeMs = 320.0f;
    float feedback = 0.28f;
    float mix = 0.25f;
};
