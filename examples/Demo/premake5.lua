project "Demo"
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


    ApplyCommonConfigs()