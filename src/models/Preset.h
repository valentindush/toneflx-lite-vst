#pragma once

#include <juce_core/juce_core.h>
#include <cstdint>

struct GeneratedPreset
{
    std::uint32_t seed = 0;

    float saturationDrive = 2.5f;
    float saturationMix = 1.0f;

    int bitcrusherDepth = 12;
    int bitcrusherRate = 1;
    float bitcrusherMix = 0.35f;

    float chorusRate = 0.35f;
    float chorusDepth = 8.0f;
    float chorusMix = 0.25f;

    float delayTime = 320.0f;
    float delayFeedback = 0.28f;
    float delayMix = 0.25f;

    float reverbRoomSize = 0.45f;
    float reverbDamping = 0.35f;
    float reverbWidth = 0.8f;
    float reverbMix = 0.2f;

    float outputTrim = 0.0f;
};

struct PresetSnapshot
{
    juce::String name { "Untitled" };
    juce::StringArray descriptors;
    GeneratedPreset preset;
};
