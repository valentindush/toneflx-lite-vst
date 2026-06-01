#pragma once

#include <juce_core/juce_core.h>
#include "../models/Preset.h"

class PresetJson
{
public:
    static juce::String toJson(const PresetSnapshot& snapshot)
    {
        auto root = std::make_unique<juce::DynamicObject>();
        root->setProperty("name", snapshot.name);
        root->setProperty("seed", static_cast<double>(snapshot.preset.seed));

        juce::Array<juce::var> descriptors;
        for (const auto& descriptor : snapshot.descriptors)
            descriptors.add(descriptor);

        root->setProperty("descriptors", descriptors);

        auto parameters = std::make_unique<juce::DynamicObject>();
        parameters->setProperty("saturationDrive", snapshot.preset.saturationDrive);
        parameters->setProperty("saturationMix", snapshot.preset.saturationMix);
        parameters->setProperty("bitcrusherDepth", snapshot.preset.bitcrusherDepth);
        parameters->setProperty("bitcrusherRate", snapshot.preset.bitcrusherRate);
        parameters->setProperty("bitcrusherMix", snapshot.preset.bitcrusherMix);
        parameters->setProperty("chorusRate", snapshot.preset.chorusRate);
        parameters->setProperty("chorusDepth", snapshot.preset.chorusDepth);
        parameters->setProperty("chorusMix", snapshot.preset.chorusMix);
        parameters->setProperty("delayTime", snapshot.preset.delayTime);
        parameters->setProperty("delayFeedback", snapshot.preset.delayFeedback);
        parameters->setProperty("delayMix", snapshot.preset.delayMix);
        parameters->setProperty("reverbRoomSize", snapshot.preset.reverbRoomSize);
        parameters->setProperty("reverbDamping", snapshot.preset.reverbDamping);
        parameters->setProperty("reverbWidth", snapshot.preset.reverbWidth);
        parameters->setProperty("reverbMix", snapshot.preset.reverbMix);
        parameters->setProperty("outputTrim", snapshot.preset.outputTrim);
        root->setProperty("parameters", juce::var(parameters.release()));

        return juce::JSON::toString(juce::var(root.release()), true);
    }

    static bool fromJson(const juce::String& json, PresetSnapshot& snapshot)
    {
        const auto parsed = juce::JSON::parse(json);
        const auto* root = parsed.getDynamicObject();

        if (root == nullptr)
            return false;

        const auto parameters = root->getProperty("parameters");
        const auto* parameterObject = parameters.getDynamicObject();

        if (parameterObject == nullptr)
            return false;

        snapshot.name = root->getProperty("name").toString();
        snapshot.preset.seed = static_cast<std::uint32_t>(static_cast<double>(root->getProperty("seed")));
        snapshot.descriptors.clear();

        if (const auto* descriptors = root->getProperty("descriptors").getArray())
            for (const auto& descriptor : *descriptors)
                snapshot.descriptors.add(descriptor.toString());

        snapshot.preset.saturationDrive = getFloat(*parameterObject, "saturationDrive");
        snapshot.preset.saturationMix = getFloat(*parameterObject, "saturationMix");
        snapshot.preset.bitcrusherDepth = getInt(*parameterObject, "bitcrusherDepth");
        snapshot.preset.bitcrusherRate = getInt(*parameterObject, "bitcrusherRate");
        snapshot.preset.bitcrusherMix = getFloat(*parameterObject, "bitcrusherMix");
        snapshot.preset.chorusRate = getFloat(*parameterObject, "chorusRate");
        snapshot.preset.chorusDepth = getFloat(*parameterObject, "chorusDepth");
        snapshot.preset.chorusMix = getFloat(*parameterObject, "chorusMix");
        snapshot.preset.delayTime = getFloat(*parameterObject, "delayTime");
        snapshot.preset.delayFeedback = getFloat(*parameterObject, "delayFeedback");
        snapshot.preset.delayMix = getFloat(*parameterObject, "delayMix");
        snapshot.preset.reverbRoomSize = getFloat(*parameterObject, "reverbRoomSize");
        snapshot.preset.reverbDamping = getFloat(*parameterObject, "reverbDamping");
        snapshot.preset.reverbWidth = getFloat(*parameterObject, "reverbWidth");
        snapshot.preset.reverbMix = getFloat(*parameterObject, "reverbMix");
        snapshot.preset.outputTrim = getFloat(*parameterObject, "outputTrim");

        return true;
    }

    static bool saveToFile(const PresetSnapshot& snapshot, const juce::File& file)
    {
        if (! file.getParentDirectory().createDirectory())
            return false;

        return file.replaceWithText(toJson(snapshot));
    }

    static bool loadFromFile(const juce::File& file, PresetSnapshot& snapshot)
    {
        if (! file.existsAsFile())
            return false;

        return fromJson(file.loadFileAsString(), snapshot);
    }

private:
    static float getFloat(const juce::DynamicObject& object, const juce::Identifier& key)
    {
        return static_cast<float>(static_cast<double>(object.getProperty(key)));
    }

    static int getInt(const juce::DynamicObject& object, const juce::Identifier& key)
    {
        return static_cast<int>(object.getProperty(key));
    }
};
