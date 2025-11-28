#pragma once
#include <SDLCoreLib/SDLCore.h>

class Tetris : public SDLCore::Application {
public:
	Tetris();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;
	
private:
	SDLCore::WindowID m_winID;
};