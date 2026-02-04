# TNPC — Offline-First NPC AI Toolkit (Concept)

**TNPC** is a concept project for an **offline-only** AI toolkit that helps game developers add believable, testable NPC intelligence to games **quickly**—with a future goal of supporting **both Unity and Unreal Engine**.

> This repository is intentionally **empty of code** right now.  
> It exists to present the idea, scope, and long-term direction.

---

## Vision

Build an AI runtime that lets developers plug in “real NPC brains” without building a full AI stack from scratch:

- **Deterministic decision-making** (reproducible gameplay + debuggable behavior)
- **Engine-agnostic core** with thin adapters for Unity and Unreal
- **Offline-first by design** (no cloud dependency)
- **Modular features**: movement intents, survival needs, combat behaviors, dialogue (optional), and tooling

---

## Why Offline-Only?

Many games need:
- predictable performance and costs (no per-request fees),
- reliable behavior in offline mode,
- console-friendly deployment,
- full control over content, safety, and data.

TNPC is designed around those constraints from day one.

---

## Core Idea (Planned Architecture)

### 1) C++ Core Runtime (engine-agnostic)
A portable C++ library that provides:
- **NPC Brain**
  - Behavior Trees and/or Utility AI / GOAP (selectable)
  - Needs-based decision systems (hunger, thirst, fatigue, temperature, fear, etc.)
  - Perception and threat evaluation (vision/hearing abstractions)
- **Memory & Knowledge**
  - Blackboard/state variables
  - Short-term / long-term memory (facts with TTL and persistence options)
- **Intent/Action Output**
  - NPCs output **intents** like `MoveTo`, `TakeCover`, `Interact`, `Attack`, `Speak`
  - The game/engine executes them (TNPC doesn’t replace the engine movement/animation system)

### 2) Engine Adapters
- **Unity Adapter (C#)**  
  Wraps the core runtime, provides editor tooling and components.
- **Unreal Adapter (C++ / Blueprints)**  
  Exposes the runtime to gameplay code and Blueprints.

### 3) Debugging & Tooling (must-have)
- Decision traces (“why did NPC do that?”)
- Reproducible ticks (seeded RNG, replayable simulations)
- Visual debugging (blackboard inspector, behavior tree trace)
- Data-driven authoring (NPC profiles, needs curves, behavior definitions)

---

## Planned Features (High Level)

### Movement & Survival Behaviors
- Patrol, roam, investigate, flee, seek shelter, gather resources
- Dynamic needs: hunger, thirst, warmth, stamina, morale
- Threat-based responses: cover seeking, alerting allies, retreat logic

### Dialogue (Optional / Offline)
Two potential modes:
1. **Safe template + retrieval** (controlled, deterministic)
2. **Local LLM integration** (optional addon, offline inference, strict tool/action boundaries)

### Voice (Optional / Offline)
Offline STT/TTS as optional integrations (LOD-based so it doesn’t melt CPUs).

---

## What TNPC Is *Not*
- A full replacement for Unity/Unreal navigation or animation systems
- “One model that magically solves everything”
- A cloud service

TNPC is envisioned as a **game AI runtime** that is controllable, testable, and shippable offline.

---

## Target Use Cases
- RPGs with believable villagers, companions, and enemies
- Survival games with needs-driven NPCs and emergent behaviors
- Sandbox worlds where NPCs react to changing conditions

---

## Roadmap (Concept)
- **Phase 1:** C++ core runtime (deterministic tick + intents + behavior system)
- **Phase 2:** Unity adapter (components + debug overlay + example scene)
- **Phase 3:** Unreal adapter (plugin + Blueprint nodes + example level)
- **Phase 4:** Authoring tools + data-driven NPC profiles
- **Phase 5:** Optional offline dialogue/voice addons

---

## Contributing / Feedback
This repo currently serves as an idea and specification space.
If you’re interested in the concept, feel free to open an issue with:
- desired feature set,
- engine constraints (Unity/Unreal versions),
- platform targets (PC/console),
- offline requirements (CPU-only vs GPU allowed).

---

## License
Planned: MIT (subject to change once implementation begins).
