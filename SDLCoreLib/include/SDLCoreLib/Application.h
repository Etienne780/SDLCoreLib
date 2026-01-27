#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
#include "SDLCoreRenderer.h"
#include "SDLCoreTime.h"
#include "SDLCoreInput.h"
#include "types/Version.h"
#include "Window.h"

namespace SDLCore {

	class Window;

	class Application {
	friend class Window;
	public:
		Application(std::string& name, const Version& version);
		Application(std::string&& name, const Version& version);

		static Application* GetInstance();

		/**
		* @brief Returns whether the entire application is scheduled to quit.
		* @return true if the application shutdown has been requested, false otherwise
		*/
		static bool IsQuit();

		static Platform GetPlatform();

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
		* @brief Delete a Window
		* @param id Window to Delete
		*/
		bool DeleteWindow(WindowID id);

		/*
		* @brief Delete all windows
		*/
		void DeleteAllWindows();

		/**
		* @brief Returns whether a window currently has cursor locking enabled.
		* @return True if cursor locking is active for a window, otherwise false.
		*/
		bool IsCursorLocked() const;

		/**
		* @brief Returns whether cursor locking is configured to auto-center on window resize.
		* @return True when lock position is automatically updated to the window center.
		*/
		bool IsCursorLockCenter() const;

		/**
		* @brief Gets a window with a given id
		* @param id to search for
		* @param raw ptr to the window or nullptr if not found
		*/
		Window* GetWindow(WindowID id);

		/*
		* @brief Gets the current amount of windows owned by this application
		* @return Amount of windows
		*/
		size_t GetWindowCount() const;

		/*
		* @brief Gets the name of the current application
		* @return Application name
		*/
		std::string GetName() const;

		/*
		* @brief Gets the Version of the current application
		* @return Application version
		*/
		Version GetVersion() const;

		/**
		* @brief Returns the window ID for which cursor locking is currently active.
		* @return The WindowID of the locked window, or an invalid ID if no lock is active.
		*/
		WindowID GetCursorLockWinID() const;

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

		/**
		* @brief Enables or disables cursor locking for a specific window.
		* 
		* When cursor locking is active, the cursor is repositioned according to the lock configuration.
		* For input processing while the cursor is locked, relative mouse movement should be used
		* (e.g. SDL relative motion or Input::GetRelativePosition()), as absolute coordinates are not meaningful
		* during cursor lock.
		* 
		* @param winID Identifier of the target window.
		* @param active Enables cursor locking when true, disables when false.
		* @param center When true, the lock position is automatically updated to the window center.
		* @return True on successful configuration, false if the window does not exist.
		*/
		bool SetCursorLock(WindowID winID, bool active, bool center = true);

		/**
		* @brief Activates or deactivates the internal cursor-lock behavior for the currently locked window.
		* 
		* For input processing while cursor locking is active, relative mouse movement should be used
		* (e.g. SDL relative motion or engine-provided relative delta). Absolute mouse positions do not reflect
		* real cursor motion when locking is enabled.
		* 
		* @param active When true, cursor grab is enforced; when false, the previous grab state is restored.
		*/
		void SetCursorLockActive(bool active);

		/**
		* @brief Sets the horizontal lock position for the cursor.
		* @param x Desired lock position on the x-axis (clamped to >= 0).
		*/
		void SetCursorLockPosX(float x);

		/**
		* @brief Sets the vertical lock position for the cursor.
		* @param y Desired lock position on the y-axis (clamped to >= 0).
		*/
		void SetCursorLockPosY(float y);

		/*
		* @brief Called on programm start
		*/
		virtual void OnStart() = 0;

		/*
		* @brief called every frame of the programm
		*/
		virtual void OnUpdate() = 0;

		/*
		* @brief called called on programm end
		*/
		virtual void OnQuit() = 0;
	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };

		// 0 = ok; < 0 error
		int cancelStart = 0;
		
		SDL_Event m_sdlEvent;
		std::vector<std::unique_ptr<Window>> m_windows;
		std::vector<WindowID> m_windowsToClose;
		SDLCoreIDManager m_windowIDManager;

		bool m_closeApplication = false;
		int m_vsync = 0;
		int m_fpsCap = 0;

		WindowID m_cursorLockWinID;
		WindowCallbackID m_cursorLockResizeCallbackID;
		WindowCallbackID m_cursorLockFocusGainCallbackID;
		WindowCallbackID m_cursorLockFocusLostCallbackID;
		std::weak_ptr<SDL_Window> m_cursorLockSDLWin;
		bool m_cursorLockWinActive = false;
		bool m_cursorLockLastWinCursorGrab = false;// stores the last state of cursor grab before window was locked
		float m_cursorLockPosX = 0;
		float m_cursorLockPosY = 0;
		float m_cursorLastTime = 0;
		const float m_cursorTick = 0.1f;// intervall speed of the cursorLock (in sec)

		void Init();
		void ProcessSDLPollEvents();
		void ProcessSDLPollEventWindow(const std::unique_ptr<Window>& window);
		void FPSCapDelay(uint64_t frameStartTime) const;
		void LockCursor();
		/*
		* @brief Sets every var that is used for cursorlock to its default values.
		* and unsubs to the old win resize if the cb exists
		*/
		void ResetCursorLockParams();

		/**
		* @brief Sets the given value to all windows 
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		*/
		void SetVsyncOnWindows(int value);
	};

}