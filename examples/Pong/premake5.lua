project "Pong"
    language "C++"
    cppdialect "C++17"

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
        "%{wks.location}/SDLCoreLib/include",
        "%{wks.location}/CoreLib/include"
    }

    links {
        "CoreLib",
        "SDLCoreLib"
    }
    
    IncludeSDLCoreLib()
    -- copys the SDL DLLs in to the build path of this project
    CopySDLDLLs()

    ApplyCommonConfigs()

    filter "configurations:Debug"
        kind "ConsoleApp"

    filter "configurations:Release"
        kind "ConsoleApp"

    filter "configurations:Distribution"
        kind "WindowedApp"

    filter {}