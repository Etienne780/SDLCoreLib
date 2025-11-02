workspace "Game"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release" 
    }

    startproject "Lunara"

--------------------------------------------------------
-- Helper function for consistent directory structure
--------------------------------------------------------
function SetTargetAndObjDirs(projectName)
    -- Executables and DLLs
    targetdir("../build/bin/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

    -- Intermediate build files
    objdir("../build/intermediates/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")

    
    --------------------------------------------------------
    -- Automatically move all .lib files to centralized folder
    --------------------------------------------------------
    filter "kind:StaticLib"
        targetdir("../build/lib/" .. projectName .. "/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}")
    filter {}
end

--------------------------------------------------------
-- Include projects
--------------------------------------------------------
include "CoreLib"
include "SDLCoreLib"
include "Lunara"

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
