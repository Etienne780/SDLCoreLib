#pragma once
#include <SDLCoreLib/Application.h>

class Lunara : public SDLCore::Application {
public:
	Lunara();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;
	
private:
	
};