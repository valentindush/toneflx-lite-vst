#include <juce_core/juce_core.h>
#include "../src/generators/PresetEngine.h"

#include <cmath>
#include <iostream>

namespace
{
bool nearlyEqual(float lhs, float rhs)
{
    return std::abs(lhs - rhs) < 0.00001f;
}

bool samePreset(const GeneratedPreset& lhs, const GeneratedPreset& rhs)
{
    return lhs.seed == rhs.seed
        && nearlyEqual(lhs.saturationDrive, rhs.saturationDrive)
        && nearlyEqual(lhs.saturationMix, rhs.saturationMix)
        && lhs.bitcrusherDepth == rhs.bitcrusherDepth
        && lhs.bitcrusherRate == rhs.bitcrusherRate
        && nearlyEqual(lhs.bitcrusherMix, rhs.bitcrusherMix)
        && nearlyEqual(lhs.chorusRate, rhs.chorusRate)
        && nearlyEqual(lhs.chorusDepth, rhs.chorusDepth)
        && nearlyEqual(lhs.chorusMix, rhs.chorusMix)
        && nearlyEqual(lhs.delayTime, rhs.delayTime)
        && nearlyEqual(lhs.delayFeedback, rhs.delayFeedback)
        && nearlyEqual(lhs.delayMix, rhs.delayMix)
        && nearlyEqual(lhs.reverbRoomSize, rhs.reverbRoomSize)
        && nearlyEqual(lhs.reverbDamping, rhs.reverbDamping)
        && nearlyEqual(lhs.reverbWidth, rhs.reverbWidth)
        && nearlyEqual(lhs.reverbMix, rhs.reverbMix)
        && nearlyEqual(lhs.outputTrim, rhs.outputTrim);
}

bool inRange(float value, float min, float max)
{
    return value >= min && value <= max;
}

bool isValidPluginRange(const GeneratedPreset& preset)
{
    return inRange(preset.saturationDrive, 1.0f, 12.0f)
        && inRange(preset.saturationMix, 0.0f, 1.0f)
        && preset.bitcrusherDepth >= 4
        && preset.bitcrusherDepth <= 16
        && preset.bitcrusherRate >= 1
        && preset.bitcrusherRate <= 32
        && inRange(preset.bitcrusherMix, 0.0f, 1.0f)
        && inRange(preset.chorusRate, 0.05f, 5.0f)
        && inRange(preset.chorusDepth, 0.0f, 20.0f)
        && inRange(preset.chorusMix, 0.0f, 1.0f)
        && inRange(preset.delayTime, 10.0f, 2000.0f)
        && inRange(preset.delayFeedback, 0.0f, 0.92f)
        && inRange(preset.delayMix, 0.0f, 1.0f)
        && inRange(preset.reverbRoomSize, 0.0f, 1.0f)
        && inRange(preset.reverbDamping, 0.0f, 1.0f)
        && inRange(preset.reverbWidth, 0.0f, 1.0f)
        && inRange(preset.reverbMix, 0.0f, 1.0f)
        && inRange(preset.outputTrim, -24.0f, 6.0f);
}

int fail(const char* message)
{
    std::cerr << message << '\n';
    return 1;
}
}

int main()
{
    const juce::StringArray descriptors { "warm", "wide", "dreamy" };
    const auto first = PresetEngine::generate(descriptors, 12345);
    const auto second = PresetEngine::generate(descriptors, 12345);
    const auto third = PresetEngine::generate(descriptors, 54321);
    const auto harsh = PresetEngine::generate({ "harsh" }, 12345);

    if (! samePreset(first, second))
        return fail("Expected identical descriptors and seed to generate the same preset.");

    if (samePreset(first, third))
        return fail("Expected different seeds to generate different presets.");

    if (! isValidPluginRange(first))
        return fail("Generated preset is outside plugin parameter ranges.");

    if (! isValidPluginRange(harsh))
        return fail("Harsh preset is outside plugin parameter ranges.");

    if (harsh.outputTrim > -2.0f)
        return fail("Harsh descriptor should reduce output trim.");

    std::cout << "PresetEngine tests passed\n";
    return 0;
}
