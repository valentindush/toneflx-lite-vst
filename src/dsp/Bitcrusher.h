#pragma once

#include <JuceHeader.h>
#include <cmath>

class BitcrusherModule
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        maximumBlockSize = spec.maximumBlockSize;
        numChannels = spec.numChannels;

        heldSamples.resize(static_cast<int>(numChannels));
        holdCounters.resize(static_cast<int>(numChannels));
        reset();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const auto currentBitDepth = bitDepth;
        const auto currentRateReduction = rateReduction;
        const auto currentMix = mix;
        const auto levels = static_cast<float>((1 << currentBitDepth) - 1);

        for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            auto& heldSample = heldSamples.getReference(channel);
            auto& holdCounter = holdCounters.getReference(channel);

            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const auto dry = samples[sample];

                if (holdCounter <= 0)
                {
                    const auto clipped = juce::jlimit(-1.0f, 1.0f, dry);
                    const auto normalised = (clipped + 1.0f) * 0.5f;
                    const auto quantised = std::round(normalised * levels) / levels;
                    heldSample = (quantised * 2.0f) - 1.0f;
                    holdCounter = currentRateReduction;
                }

                --holdCounter;

                samples[sample] = dry + ((heldSample - dry) * currentMix);
            }
        }
    }

    void reset()
    {
        heldSamples.fill(0.0f);
        holdCounters.fill(0);
    }

    void setBitDepth(int newBitDepth)
    {
        bitDepth = juce::jlimit(4, 16, newBitDepth);
    }

    int getBitDepth() const
    {
        return bitDepth;
    }

    void setRateReduction(int newRateReduction)
    {
        rateReduction = juce::jlimit(1, 32, newRateReduction);
    }

    int getRateReduction() const
    {
        return rateReduction;
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
    double sampleRate = 44100.0;
    juce::uint32 maximumBlockSize = 0;
    juce::uint32 numChannels = 0;
    juce::Array<float> heldSamples;
    juce::Array<int> holdCounters;
    int bitDepth = 12;
    int rateReduction = 1;
    float mix = 0.35f;
};
