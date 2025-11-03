project "SDLCoreLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    SetTargetAndObjDirs("%{prj.name}")

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    includedirs {
        "include",
        "include/%{prj.name}",
        "%{wks.location}/CoreLib/include",
        "%{wks.location}/vendor/SDL3/include",
        "%{wks.location}/vendor/SDL3_image/include",
        "%{wks.location}/vendor/SDL3_mixer/include",
        "%{wks.location}/vendor/SDL3_net/include",
        "%{wks.location}/vendor/SDL3_ttf/include",
        "%{wks.location}/vendor/FreeType/include"
    }

    libdirs {
        "%{wks.location}/vendor/SDL3/lib/x64",
        "%{wks.location}/vendor/SDL3_image/lib/x64",
        "%{wks.location}/vendor/SDL3_mixer/lib/x64",
        "%{wks.location}/vendor/SDL3_net/lib/x64",
        "%{wks.location}/vendor/SDL3_ttf/lib/x64",
        "%{wks.location}/vendor/FreeType/lib-vc2022"
    }

    links {
        "CoreLib",
        "SDL3.lib",
        "SDL3_image.lib",
        "SDL3_mixer.lib",
        "SDL3_net.lib",
        "SDL3_ttf.lib",
        "freetype.lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "SDL_MAIN_HANDLED" }

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

function IncludeSDLCoreLib()
    includedirs {
        "%{wks.location}/SDLCoreLib/include",
        "%{wks.location}/CoreLib/include",
        "%{wks.location}/vendor/SDL3/include",
        "%{wks.location}/vendor/SDL3_image/include",
        "%{wks.location}/vendor/SDL3_mixer/include",
        "%{wks.location}/vendor/SDL3_net/include",
        "%{wks.location}/vendor/SDL3_ttf/include",
        "%{wks.location}/vendor/FreeType/include"
    }

    libdirs {
        "%{wks.location}/vendor/SDL3/lib/x64",
        "%{wks.location}/vendor/SDL3_image/lib/x64",
        "%{wks.location}/vendor/SDL3_mixer/lib/x64",
        "%{wks.location}/vendor/SDL3_net/lib/x64",
        "%{wks.location}/vendor/SDL3_ttf/lib/x64",
        "%{wks.location}/vendor/FreeType/lib-vc2022"
    }

    links {
        "SDLCoreLib",
        "CoreLib",
        "SDL3.lib",
        "SDL3_image.lib",
        "SDL3_mixer.lib",
        "SDL3_net.lib",
        "SDL3_ttf.lib",
        "freetype.lib"
    }
end

function CopySDLDLLs()
    postbuildcommands {
        '{MKDIR} "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3_image/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3_mixer/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3_net/lib/x64/*.dll" "%{cfg.targetdir}"',
        '{COPY} "%{wks.location}/vendor/SDL3_ttf/lib/x64/*.dll" "%{cfg.targetdir}"'
    }
end