workspace "Game"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release" 
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
    include "examples/Demo"
    include "examples/Tetris"
    include "examples/Pong"


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
