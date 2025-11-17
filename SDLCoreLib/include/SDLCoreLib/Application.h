#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
#include "SDLCoreRenderer.h"
#include "CoreTime.h"
#include "CoreInput.h"
#include "IDManager.h"
#include "types/Version.h"
#include "Window.h"

namespace SDLCore {

	class Application {
	public:
		Application(std::string& name, const Version& version);
		Application(std::string&& name, const Version& version);

		static Application* GetInstance();

		/**
		* @brief Returns whether the entire application is scheduled to quit.
		* @return true if the application shutdown has been requested, false otherwise
		*/
		static bool IsQuit();

		/**
		* @brief Starts the main loop of the application
		* @return returns an error code or 0
		*/
		SDLResult Start();

		/**
		* @brief Quits the application
		*/
		void Quit();

		/**
		 * @brief Adds a new Window instance to the application without creating the underlying SDL window or renderer.
		 *
		 * This function allocates and registers a new Window object within the Application, but does not yet
		 * initialize the SDL window or renderer. It is typically used when the window setup should be controlled
		 * manually or deferred.
		 *
		 * @param idPtr Optional pointer to a WindowID variable that receives the unique ID of the new window.
		 *              If provided, the ID will automatically be set to SDLCORE_INVALID_ID when the window closes.
		 * @param name The name/title of the window.
		 * @param width Desired window width in pixels.
		 * @param height Desired window height in pixels.
		 *
		 * @return Raw pointer to the newly added Window instance, or nullptr if window creation failed.
		 */
		Window* AddWindow(WindowID* idPtr, std::string name, int width, int height);

		/**
		 * @brief Creates and fully initializes a new SDL window and renderer.
		 *
		 * This function creates a new Window object, generates a unique WindowID, and immediately initializes
		 * the underlying SDL window and renderer. It also automatically registers an on-close callback that
		 * invalidates the provided WindowID pointer when the window is closed.
		 *
		 * @param idPtr Optional pointer to a WindowID variable that receives the unique ID of the new window.
		 *              The ID will automatically be set to SDLCORE_INVALID_ID when the window is closed.
		 * @param name The name/title of the window.
		 * @param width Desired window width in pixels.
		 * @param height Desired window height in pixels.
		 *
		 * @return Raw pointer to the newly created Window instance, or nullptr if window creation failed.
		 */
		Window* CreateWindow(WindowID* idPtr, const std::string& name, int width, int height);

		/**
		* @brief Removes a Window
		* @param id Window to remove
		*/
		bool RemoveWindow(WindowID id);

		/*
		* @brief Removes all window that are currently open
		*/
		void RemoveAllWindows();

		/**
		* @brief Gets a window with a given id
		* @param id to search for
		* @param raw ptr to the window or nullptr if not found
		*/
		Window* GetWindow(WindowID id);

		size_t GetWindowCount() const;

		/**
		* @brief Sets the application's frame rate cap or VSync mode.
		*
		* This function configures how the application's rendering loop limits its frame rate.
		* Depending on the value, it either enables vertical synchronization (VSync),
		* adaptive VSync, or sets a custom FPS cap. Negative special values are used
		* to represent different synchronization modes.
		*
		* @param value Frame cap mode or FPS limit.
		* - APPLICATION_FPS_UNCAPPED: No frame rate limit.
		* - APPLICATION_FPS_VSYNC_ON: Enables standard VSync.
		* - APPLICATION_FPS_VSYNC_ADAPTIVE_ON: Enables adaptive VSync.
		* - Any positive integer: Caps the frame rate to the specified FPS value.
		*/
		void SetFPSCap(int value);

		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnQuit() = 0;
	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };

		// 0 = ok; 1 = SDL error;
		int cancelStart = 0;
		std::string m_cancelErrorMsg;
		
		SDL_Event m_sdlEvent;
		std::vector<std::unique_ptr<Window>> m_windows;
		std::vector<WindowID> m_windowsToClose;
		IDManager m_windowIDManager;

		bool m_closeApplication = false;
		int m_vsync = 0;
		int m_fpsCap = 0;

		void Init();
		void ProcessSDLPollEvents();
		void ProcessSDLPollEventWindow(const std::unique_ptr<Window>& window);
		void FPSCapDelay(uint64_t frameStartTime);

		/**
		* @brief Sets the given value to all windows 
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		*/
		void SetVsyncOnWindows(int value);
	};

}