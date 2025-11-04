#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
#include "CoreTime.h"
#include "IDManager.h"
#include "Version.h"
#include "Window.h"

namespace SDLCore {

	constexpr int APPLICATION_FPS_UNCAPPED = 0;
	constexpr int APPLICATION_FPS_VSYNC_ON = -1;
	constexpr int APPLICATION_FPS_VSYNC_ADAPTIVE_ON = -2;

	class Application {
	public:
		Application(std::string& name, const Version& version);
		Application(std::string&& name, const Version& version);

		static Application* GetInstance();

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
		* @brief Adds a Window
		* 
		* @param name The name of the window
		* @param width The width of the window
		* @param height The height of the window
		* 
		* @return Raw pointer of the newly created window
		*/
		Window* AddWindow(std::string name, int width, int height);

		/**
		* @brief Creates a window
		* 
		* @param name The name of the window
		* @param width The width of the window
		* @param height The height of the window
		* 
		* @return Raw pointer of the newly created window
		*/
		Window* CreateWindow(std::string name, int width, int height);

		/**
		* @brief Removes a Window
		* @param id Window to remove
		*/
		bool RemoveWindow(WindowID id);

		/**
		* @brief Gets a window with a given id
		* @param id to search for
		* @param raw ptr to the window or nullptr if not found
		*/
		Window* GetWindow(WindowID id);

		size_t GetWindowCount() const;

		void SetFPSCap(int value);

		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnQuit() = 0;
	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };

		// 0 = ok; 1 = SDL error;
		int cancelStart = 0;
		std::string m_sdlErrorMsg;
		
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