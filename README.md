# TNPC

TNPC is an offline-only, engine-agnostic C++20 core library that represents a compact, deterministic AI runtime for NPC decision-making. It provides a behavior-tree based brain with blackboard/memory, perception stubs, intent queues, and deterministic debug tracing. The goal is to serve as the core logic layer that can be embedded into Unity/Unreal adapters without runtime networking or external dependencies.

## Features

- Deterministic tick pipeline with seedable RNG
- Blackboard (variant key/value) + short-term memory with TTL
- Minimal, robust behavior tree (selector/sequence/condition/action)
- Perception interface with a default stub
- Intent queue with stable priority ordering
- Debug trace capture with human-readable export
- JSON export for `DebugTrace`
- CLI demo simulation + unit tests (offline, header-only test harness)

## Build (Windows MSVC)

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release
```

## Build (Linux/macOS)

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Run the demo

The demo is a deterministic sandbox that showcases how multiple NPCs tick through a small hunger/threat scenario. It is intended to validate reproducibility, intent emission, and debug tracing without any game engine.

```bash
./sim_demo --seed 42 --ticks 30 --agents 2 --verbose
```

On Windows (multi-config builds), the executable is located at `build/Release/sim_demo.exe`.

## Adapter path (Unity / Unreal)

The `airuntime` namespace is intentionally engine-agnostic. A Unity/Unreal adapter would typically:

- Map engine perception data into `PerceptionInput`
- Translate `Intent` entries into engine-specific actions (movement, cover, animation, etc.)
- Forward engine timing to `Context::delta_seconds` and `Context::time_seconds`
- Stream `DebugTrace` output into engine visualizers or telemetry

Adapter stubs live in `include/airuntime/adapters/adapter_interfaces.h` and provide placeholder
interfaces for perception, intent execution, and engine tick hooks.

## Offline-only guarantee

The library and demo perform no network calls and require no downloads during builds. All dependencies are vendored in-repo and header-only.
