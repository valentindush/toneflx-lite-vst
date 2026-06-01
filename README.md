# TONEFLX Lite

TONEFLX Lite is a small JUCE/CMake multi-FX audio plugin focused on controlled-random sound design.

The first milestone is intentionally simple:

- Build VST3 and Standalone targets.
- Process audio through saturation, bitcrusher, chorus, delay, and reverb modules.
- Provide a minimal editor window with saturation, bitcrusher, chorus, delay, and reverb controls.
- Generate parameter sets from simple descriptors: warm, dark, vintage, wide, and dreamy.
- Establish a folder structure for DSP modules, preset generation, and UI components.

## Requirements

Install the CLI build tools:

```bash
brew install cmake ninja git
xcode-select --install
```

## Build

You can either place a JUCE checkout at `JUCE/` or let CMake fetch JUCE automatically.

```bash
cmake --preset macos-debug
cmake --build --preset macos-debug
```

## Descriptor Generation

Use the descriptor buttons in the plugin editor, then press `GENERATE`. The generator creates a random seed, narrows sensible parameter ranges based on the selected descriptors, and writes the result through the plugin parameters so the knobs, audio engine, automation, and saved state all update together.

To recreate a sound, keep the same descriptors selected, enter the seed in the seed field, and press `RECALL`.

The last generated seed and descriptor choices are stored in the plugin state, so DAW sessions can restore the generator context along with the parameter values.

Named presets can be saved and loaded from the editor. Preset files are stored as JSON in:

```text
~/Documents/TONEFLX Lite Presets
```
