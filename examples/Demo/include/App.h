#pragma once
#include <SDLCoreLib/SDLCore.h>

class App : public SDLCore::Application {
public:
	App();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;
	
private:
	
};