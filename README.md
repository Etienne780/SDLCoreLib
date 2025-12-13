# SDLCoreLib

SDLCoreLib is a lightweight application framework built on top of SDL3. It provides a structured base application class and utility systems to simplify common tasks in 2D application and game development. The library abstracts low-level SDL components while keeping the API transparent and predictable.

SDLCoreLib focuses on:

- Window creation and management  
- Texture and rendering utilities  
- Text and font handling  
- Input handling  
- Timing utilities  
- Basic audio abstraction  
- A unified application lifecycle  

The goal is to reduce boilerplate and provide a clean starting point for SDL-based applications without hiding SDL itself.

---

## Features

### Application Framework

SDLCoreLib provides a central `Application` base class. Users create their own application by inheriting from it and implementing lifecycle callbacks:

- `OnStart()`  
- `OnUpdate()`  
- `OnQuit()`  

The base class manages initialization, window lifetime, event processing, timing, and the main loop.

### Window System

Simplified creation and handling of SDL windows via lightweight IDs. Windows expose standard operations such as resizing, renderer selection, and queries for size, focus, and visibility.

### Rendering Layer

A structured wrapper over SDL’s 2D rendering API, including:

- Renderer binding to windows  
- Color management  
- Primitive drawing  
- Text rendering  
- Stroke and fill operations  
- Font utilities  
- Texture rendering  

### Input System

Input handling per window:

- Keyboard (pressed, just-pressed, just-released)  
- Mouse buttons  
- Mouse position  

### Timing

A unified time module providing:

- Delta time  
- Runtime  
- Frame rate calculation  
- Frame limiting  

### Audio

A basic helper layer for loading and playing audio, built on SDL_Mixer.

---

## Building Projects

SDLCoreLib uses **Premake** for project generation. To build a project:

1. Open a command prompt in the root folder (where `build.bat` is located).  
2. Run the build script with an action, for example:

```bat
build.bat clean
```
### Available Actions
```bat
compile      Generate project files and build using Visual Studio 2022
clean        Remove all binaries, intermediate files, and generated projects
codelite     Generate CodeLite project files
gmake        Generate GNU Makefiles
vs2022       Generate Visual Studio 2022 project files
xcode4       Generate Xcode project files
help         Show this help message
```
The compile action will automatically call msbuild to build the solution.

## Adding New Projects
To add a new project, copy the `examples/Template` folder and rename it to your desired project name, e.g., `examples/Tetris`. Update the `premake5.lua` file inside the new project folder with the project-specific settings:
```lua
project "Tetris"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    SetTargetAndObjDirs("%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp",
        "main.cpp"
    }

    includedirs {
        "include",
        "include/%{prj.name}",
        "../SDLCoreLib/include",
        "../CoreLib/include"
    }

    links {
        "CoreLib",
        "SDLCoreLib"
    }
    
    IncludeSDLCoreLib()
    CopySDLDLLs()
```
Finally, register the project in your main workspace in the root `premake5.lua` file:
```lua
group "Examples"
    include "examples/Tetris"
```
The `group "Examples"` is optional. It is used to create a clear separation between example projects and libraries. `include` can also be called outside of the group if you prefer not to have the project grouped.
