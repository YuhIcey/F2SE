# F2SE  

**Fallout 2 Script Extender — By Frank**

## What is F2SE?

F2SE (Fallout 2 Script Extender) is a custom-built runtime extension system designed to hook into Fallout 2's internal engine and provide advanced scripting functionality beyond what the base game allows. Its primary goal was to enable deep access to game memory, support custom scripts, and serve as the foundation for future multiplayer and modding capabilities.

Inspired by tools such as NVSE (New Vegas Script Extender) and FOSE (Fallout 3 Script Extender), F2SE was created from the ground up to work specifically with Fallout 2’s engine. It allows developers to inject scripts at runtime, patch memory structures, hook engine-level behavior, and interact directly with the game state. 

The original design included the following features:
- Memory hooks for real-time game manipulation
- Script execution tied to game events and lifecycle states
- Basic modular plugin framework (under construction)
- Early-stage anti-cheat hooks
- Debug and developer logging output
- UI and input extension support
- Support for reading engine variables and modifying game logic on-the-fly

F2SE made it possible to develop systems like:
- Real-time player synchronization for multiplayer mods
- Dynamic quest, inventory, and combat manipulation
- In-game debugging tools and overlay systems
- External server communication for persistent game state

## Project Status: Discontinued

This project is no longer being actively developed.

While F2SE does technically work — it successfully injects and executes scripts, hooks into Fallout 2’s main game loop, and provides basic patching functionality — it is **extremely incomplete**. Many features are either half-finished, unstable, or untested. There is no versioning system, no user interface, and no safety net for memory patching errors. Use at your own risk.

F2SE has been officially **discontinued** and will not receive further updates or support in its current form. However, the core concepts and codebase are now being carried forward into **F2MPx**, an upcoming multiplayer mod and engine-layer rewrite for Fallout 2 that includes:
- Full multiplayer support
- A rebuilt script extender module
- Secure networking
- WASD movement
- Server authority over player state

All future development will occur under the **F2MPx** project umbrella, which includes a reworked scripting and plugin system based on what was started with F2SE.

## Final Notes

F2SE served as a prototype and internal testbed for future tech in Fallout 2 multiplayer development. If you’re reading this looking for a stable public release or documentation, there isn’t one. This was an experimental dev tool, and it remains rough around the edges.

For future updates and a fully realized system, refer to the F2MPx project.
