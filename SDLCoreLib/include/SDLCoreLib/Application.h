#pragma once
#include <string>
#include <vector>
#include <Window.h>

namespace SDLCore {

	class Version;
	class Application {
	public:
		Application(std::string& name, Version& version);
		Application(std::string&& name, Version&& version);

	private:
		std::string m_name;
		Version m_version;
		
		std::vector<Window> m_windows;
	};

}