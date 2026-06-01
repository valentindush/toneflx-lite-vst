#pragma once

#include <JuceHeader.h>

class ReverbModule
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        reverb.prepare(spec);
        reset();
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        updateParameters();

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);
    }

    void reset()
    {
        reverb.reset();
    }

    void setRoomSize(float newRoomSize)
    {
        roomSize = juce::jlimit(0.0f, 1.0f, newRoomSize);
    }

    float getRoomSize() const
    {
        return roomSize;
    }

    void setDamping(float newDamping)
    {
        damping = juce::jlimit(0.0f, 1.0f, newDamping);
    }

    float getDamping() const
    {
        return damping;
    }

    void setWidth(float newWidth)
    {
        width = juce::jlimit(0.0f, 1.0f, newWidth);
    }

    float getWidth() const
    {
        return width;
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
    void updateParameters()
    {
        juce::Reverb::Parameters params;
        params.roomSize = roomSize;
        params.damping = damping;
        params.width = width;
        params.wetLevel = mix;
        params.dryLevel = 1.0f - mix;
        params.freezeMode = 0.0f;
        reverb.setParameters(params);
    }

    double sampleRate = 44100.0;
    juce::dsp::Reverb reverb;
    float roomSize = 0.45f;
    float damping = 0.35f;
    float width = 0.8f;
    float mix = 0.2f;
};
