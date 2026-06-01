cmake_minimum_required(VERSION 3.22)

project(ToneflxLite VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
enable_testing()

set(TONEFLX_JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/JUCE" CACHE PATH "Path to a local JUCE checkout")

if(EXISTS "${TONEFLX_JUCE_PATH}/CMakeLists.txt")
    add_subdirectory("${TONEFLX_JUCE_PATH}" JUCE)
else()
    message(STATUS "Local JUCE checkout not found at ${TONEFLX_JUCE_PATH}; fetching JUCE with CMake FetchContent")
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.12
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(ToneflxLite
    COMPANY_NAME "Toneflx"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Tnfx
    PLUGIN_CODE Tflx
    FORMATS VST3 Standalone
    PRODUCT_NAME "TONEFLX Lite"
)

juce_generate_juce_header(ToneflxLite)

target_sources(ToneflxLite
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/dsp/Bitcrusher.h
        src/dsp/Chorus.h
        src/dsp/Delay.h
        src/dsp/Reverb.h
        src/dsp/Saturation.h
        src/generators/PresetEngine.h
        src/models/Preset.h
)

target_compile_definitions(ToneflxLite
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ToneflxLite
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

add_executable(PresetEngineTests
    tests/PresetEngineTests.cpp
)

target_compile_definitions(PresetEngineTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(PresetEngineTests
    PRIVATE
        juce::juce_core
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

add_test(NAME PresetEngineTests COMMAND PresetEngineTests)
