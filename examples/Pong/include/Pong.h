#pragma once
#include <SDLCoreLib/SDLCore.h>

class Pong : public SDLCore::Application {
public:
	Pong();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;
	
private:
	SDLCore::WindowID m_winID;
};