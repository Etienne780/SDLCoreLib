#pragma once
#include <SDLCoreLib/SDLCore.h>

class Demo : public SDLCore::Application {
public:
	Demo();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;
	
private:
	
};