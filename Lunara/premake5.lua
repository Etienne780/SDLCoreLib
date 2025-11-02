project "Lunara"
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

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        buildoptions { "/MTd" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "On"
        buildoptions { "/MT" }