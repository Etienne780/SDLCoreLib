project "Demo"
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
    -- copys the SDL DLLs in to the build path of this project
    CopySDLDLLs()

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        buildoptions { "/MTd" }
    filter {}

    filter "configurations:Release"
        defines { "NDEBUG", "DISABLE_LOGS" }  -- optional für Log-Funktionen
        kind "WindowedApp"                     -- GUI-App without Console
        runtime "Release"
        optimize "On"
        buildoptions { "/MT" }
    filter {}