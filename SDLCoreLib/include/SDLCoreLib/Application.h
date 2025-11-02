#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
#include "IDManager.h"
#include "Version.h"
#include "Window.h"

namespace SDLCore {

	class Application {
	public:
		Application(std::string& name, const Version& version);
		Application(std::string&& name, const Version& version);

		/**
		* @brief Starts the main loop of the application
		* @return returns an error code or 0
		*/
		SDLResult Start();

		/**
		* @brief Adds a Window
		*/
		WindowID AddWindow();

		/**
		* @brief Creates a window
		*/
		WindowID CreateWindow(std::string name, int width, int height);

		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnQuit() = 0;

	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };

		// 0 = ok; 1 = SDL error;
		int cancelStart = 0;
		std::string m_sdlErrorMsg;
		
		std::vector<std::unique_ptr<Window>> m_windows;
		IDManager m_windowIDManager;

		bool m_closeApplication = false;

		void Init();
	};

}