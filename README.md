# SDLCoreLib

SDLCoreLib is a lightweight application framework built on top of SDL3. It provides a structured base application class and a set of utility systems designed to simplify common tasks in 2D application and game development. The library abstracts low-level SDL components while keeping the API transparent and predictable.

SDLCoreLib focuses on the following areas:
 - Window creation and management
 - Texture and rendering utilities
 - Text and font handling
 - Input handling
 - Timing utilities
 - Basic audio abstraction
 - A unified application lifecycle

The goal is to reduce boilerplate code and provide a clean starting point for SDL-based applications without hiding SDL itself.

⸻

## Features

### Application Framework

SDLCoreLib provides a central Application base class. Users create their own application type by inheriting from it and implementing lifecycle callbacks:
 - OnStart()
 - OnUpdate()
 - OnQuit()

The base class manages initialization, window lifetime, event processing, timing, and the main loop.

### Window System

The library offers simplified creation and handling of SDL windows through lightweight IDs. Windows expose standard operations such as resizing, renderer selection, and basic state queries (size, focus, visibility, etc.).

### Rendering Layer

A structured wrapper over SDL’s 2D rendering API:
 - Renderer binding to windows
 - Color management
 - Primitive drawing
 - Text rendering
 - Stroke and fill operations
 - Font utilities
 - Texture rendering

### Input System

Input is handled per-window via:
 - Keyboard (pressed, just-pressed, released)
 - Mouse buttons
 - Mouse position

### Timing

A unified time module providing:
 - Delta time
 - Run time
 - Frame rate calculation
 - Frame limiting

### Audio

A basic helper layer for loading and playing audio. Build on top of SDL_Mixer
