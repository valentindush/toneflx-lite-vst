#pragma once

#include <juce_core/juce_core.h>
#include "../models/Preset.h"
#include <random>

class PresetEngine
{
public:
    static GeneratedPreset generate(const juce::StringArray& descriptors, std::uint32_t seed)
    {
        std::mt19937 rng(seed);
        GeneratedPreset preset;
        preset.seed = seed;

        Range saturationDrive { 1.4f, 4.5f };
        Range saturationMix { 0.45f, 1.0f };
        Range bitDepth { 8.0f, 16.0f };
        Range bitRate { 1.0f, 6.0f };
        Range bitMix { 0.0f, 0.45f };
        Range chorusRate { 0.12f, 1.8f };
        Range chorusDepth { 2.0f, 12.0f };
        Range chorusMix { 0.0f, 0.45f };
        Range delayTime { 90.0f, 520.0f };
        Range delayFeedback { 0.05f, 0.38f };
        Range delayMix { 0.0f, 0.36f };
        Range reverbRoom { 0.18f, 0.62f };
        Range reverbDamping { 0.18f, 0.65f };
        Range reverbWidth { 0.45f, 1.0f };
        Range reverbMix { 0.0f, 0.28f };
        Range outputTrim { -4.5f, -1.0f };

        for (const auto& descriptor : descriptors)
        {
            if (descriptor == "warm")
            {
                saturationDrive = merge(saturationDrive, { 2.0f, 4.2f });
                saturationMix = merge(saturationMix, { 0.65f, 1.0f });
                bitMix = merge(bitMix, { 0.0f, 0.18f });
                reverbDamping = merge(reverbDamping, { 0.35f, 0.7f });
            }
            else if (descriptor == "dark")
            {
                chorusMix = merge(chorusMix, { 0.08f, 0.35f });
                delayFeedback = merge(delayFeedback, { 0.18f, 0.5f });
                reverbDamping = merge(reverbDamping, { 0.55f, 0.9f });
                reverbMix = merge(reverbMix, { 0.1f, 0.32f });
            }
            else if (descriptor == "vintage")
            {
                bitDepth = merge(bitDepth, { 6.0f, 12.0f });
                bitRate = merge(bitRate, { 2.0f, 14.0f });
                bitMix = merge(bitMix, { 0.12f, 0.45f });
                chorusDepth = merge(chorusDepth, { 5.0f, 16.0f });
            }
            else if (descriptor == "harsh")
            {
                saturationDrive = merge(saturationDrive, { 5.0f, 10.0f });
                saturationMix = merge(saturationMix, { 0.75f, 1.0f });
                bitDepth = merge(bitDepth, { 4.0f, 9.0f });
                bitRate = merge(bitRate, { 6.0f, 24.0f });
                bitMix = merge(bitMix, { 0.28f, 0.7f });
                reverbMix = merge(reverbMix, { 0.0f, 0.18f });
                outputTrim = merge(outputTrim, { -6.0f, -2.0f });
            }
            else if (descriptor == "wide")
            {
                chorusMix = merge(chorusMix, { 0.22f, 0.55f });
                chorusDepth = merge(chorusDepth, { 7.0f, 18.0f });
                reverbWidth = merge(reverbWidth, { 0.75f, 1.0f });
                delayMix = merge(delayMix, { 0.1f, 0.38f });
            }
            else if (descriptor == "dreamy")
            {
                chorusRate = merge(chorusRate, { 0.08f, 0.55f });
                delayTime = merge(delayTime, { 320.0f, 950.0f });
                delayFeedback = merge(delayFeedback, { 0.24f, 0.65f });
                reverbRoom = merge(reverbRoom, { 0.55f, 0.95f });
                reverbMix = merge(reverbMix, { 0.18f, 0.45f });
            }
        }

        preset.saturationDrive = randomFloat(rng, saturationDrive);
        preset.saturationMix = randomFloat(rng, saturationMix);
        preset.bitcrusherDepth = juce::roundToInt(randomFloat(rng, bitDepth));
        preset.bitcrusherRate = juce::roundToInt(randomFloat(rng, bitRate));
        preset.bitcrusherMix = randomFloat(rng, bitMix);
        preset.chorusRate = randomFloat(rng, chorusRate);
        preset.chorusDepth = randomFloat(rng, chorusDepth);
        preset.chorusMix = randomFloat(rng, chorusMix);
        preset.delayTime = randomFloat(rng, delayTime);
        preset.delayFeedback = randomFloat(rng, delayFeedback);
        preset.delayMix = randomFloat(rng, delayMix);
        preset.reverbRoomSize = randomFloat(rng, reverbRoom);
        preset.reverbDamping = randomFloat(rng, reverbDamping);
        preset.reverbWidth = randomFloat(rng, reverbWidth);
        preset.reverbMix = randomFloat(rng, reverbMix);
        preset.outputTrim = randomFloat(rng, outputTrim);

        return preset;
    }

private:
    struct Range
    {
        float min = 0.0f;
        float max = 1.0f;
    };

    static Range merge(Range current, Range incoming)
    {
        const auto minValue = juce::jmax(current.min, incoming.min);
        const auto maxValue = juce::jmin(current.max, incoming.max);

        if (minValue <= maxValue)
            return { minValue, maxValue };

        return incoming;
    }

    static float randomFloat(std::mt19937& rng, Range range)
    {
        std::uniform_real_distribution<float> distribution(range.min, range.max);
        return distribution(rng);
    }
};
