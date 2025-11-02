#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
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

		virtual void OnStart() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnQuit() = 0;

	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };
		
		std::vector<Window> m_windows;

		bool m_closeApplication = false;

		void Init();
	};

}