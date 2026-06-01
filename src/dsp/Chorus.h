#pragma once

#include <JuceHeader.h>
#include <cmath>

class ChorusModule
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
        const auto currentRateHz = rateHz;
        const auto currentDepthMs = depthMs;
        const auto currentMix = mix;
        const auto delayBufferSize = delayBuffer.getNumSamples();
        const auto phaseIncrement = juce::MathConstants<double>::twoPi * currentRateHz / sampleRate;

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto lfo = 0.5f + (0.5f * std::sin(static_cast<float>(phase)));
            const auto delayMs = baseDelayMs + (currentDepthMs * lfo);
            const auto delaySamples = (delayMs / 1000.0f) * static_cast<float>(sampleRate);
            const auto readPosition = static_cast<float>(writePosition) - delaySamples;
            const auto wrappedReadPosition = readPosition < 0.0f
                ? readPosition + static_cast<float>(delayBufferSize)
                : readPosition;
            const auto readIndexA = static_cast<int>(wrappedReadPosition) % delayBufferSize;
            const auto readIndexB = (readIndexA + 1) % delayBufferSize;
            const auto interpolation = wrappedReadPosition - static_cast<float>(readIndexA);

            for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                auto* delayData = delayBuffer.getWritePointer(channel);

                const auto dry = channelData[sample];
                const auto delayedA = delayData[readIndexA];
                const auto delayedB = delayData[readIndexB];
                const auto wet = delayedA + ((delayedB - delayedA) * interpolation);

                delayData[writePosition] = dry;
                channelData[sample] = dry + ((wet - dry) * currentMix);
            }

            writePosition = (writePosition + 1) % delayBufferSize;
            phase += phaseIncrement;

            if (phase >= juce::MathConstants<double>::twoPi)
                phase -= juce::MathConstants<double>::twoPi;
        }
    }

    void reset()
    {
        delayBuffer.clear();
        writePosition = 0;
        phase = 0.0;
    }

    void setRateHz(float newRateHz)
    {
        rateHz = juce::jlimit(0.05f, 5.0f, newRateHz);
    }

    float getRateHz() const
    {
        return rateHz;
    }

    void setDepthMs(float newDepthMs)
    {
        depthMs = juce::jlimit(0.0f, 20.0f, newDepthMs);
    }

    float getDepthMs() const
    {
        return depthMs;
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
    static constexpr auto maxDelayTimeSeconds = 0.08f;
    static constexpr auto baseDelayMs = 8.0f;

    double sampleRate = 44100.0;
    juce::uint32 numChannels = 0;
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    double phase = 0.0;
    float rateHz = 0.35f;
    float depthMs = 8.0f;
    float mix = 0.25f;
};
