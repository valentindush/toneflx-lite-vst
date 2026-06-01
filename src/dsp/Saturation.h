#pragma once

#include <JuceHeader.h>
#include <cmath>

class SaturationModule
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        maximumBlockSize = spec.maximumBlockSize;
        numChannels = spec.numChannels;
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const auto currentDrive = drive;
        const auto currentMix = mix;
        const auto normaliser = std::tanh(currentDrive);

        for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);

            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const auto dry = samples[sample];
                const auto wet = std::tanh(dry * currentDrive) / normaliser;
                samples[sample] = dry + ((wet - dry) * currentMix);
            }
        }
    }

    void reset()
    {
    }

    void setDrive(float newDrive)
    {
        drive = juce::jlimit(1.0f, 12.0f, newDrive);
    }

    float getDrive() const
    {
        return drive;
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
    float drive = 1.0f;
    float mix = 1.0f;
};
