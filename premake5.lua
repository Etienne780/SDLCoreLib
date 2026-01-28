workspace "Project"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release",
        "Distribution"
    }

    startproject "Demo"

--------------------------------------------------------
-- Helper function for consistent directory structure
--------------------------------------------------------
function SetTargetAndObjDirs(projectName)
    local root = _MAIN_SCRIPT_DIR

    -- Executables and DLLs
    targetdir(root .. "/build/bin/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

    -- Intermediate build files
    objdir(root .. "/build/intermediates/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

    
    --------------------------------------------------------
    -- Automatically move all .lib files to centralized folder
    --------------------------------------------------------
    filter "kind:StaticLib"
        targetdir(root .. "/build/lib/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")
    filter {}
end

--------------------------------------------------------
-- Helper for common configuration settings
--------------------------------------------------------
function ApplyCommonConfigs()
    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        buildoptions { "/MTd" }
        kind "ConsoleApp"

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "Full"
        buildoptions { "/MT" }
        kind "ConsoleApp"

    filter "configurations:Distribution"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "Full"
        buildoptions { "/MT" }
        kind "WindowedApp"

    filter {} -- reset filter
end

------------------------------------
-- Libraries Includes
------------------------------------
group "Libraries"
    include "CoreLib"
    include "SDLCoreLib"

------------------------------------
-- Examples Includes
------------------------------------
group "Examples"
    include "Examples/Demo"
    include "Examples/Tetris"
    include "Examples/Pong"
    include "Examples/Template"

group "UIExamples"
    include "UIExamples/UITemplate"

-- Restore default group
group ""


--------------------------------------------------------
-- Custom clean action
--------------------------------------------------------
newaction {
    trigger = "clean",
    description = "Remove all binaries, intermediates, and VS files",
    execute = function()
        print("Removing binaries...")
        os.rmdir("./build/bin")

        print("Removing intermediates...")
        os.rmdir("./build/intermediates")

        print("Removing libraries...")
        os.rmdir("./build/lib")
        os.rmdir("./build")

        print("Removing Visual Studio files...")
        os.rmdir("./.vs")
        os.rmdir("./.vscode")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")

        print("Done.")
    end
}
