#pragma once
#include <SDLCoreLib/SDLCore.h>
#include "Pong.h"

class App : public SDLCore::Application {
public:
	App();
	
	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;

	void SetCountDown(float time);
	bool IsCountDownActive() const;
	void UpdateCountDown();
	void Input();
	void Render();
	
private:
	Pong m_pong;
	SDLCore::WindowID m_winID;
	int m_winWidth = 1280;
	int m_winHeight = 720;

	bool m_startGame = false;
	int m_showScore = 0;// 0 = no score; 1 = right scored; -1 = left scored
	bool m_pause = false;
	float m_startCountDown = 0;
	float m_startCounter = 0; 
	
	int m_paddleLeftScore = 0;
	int m_paddleRightScore = 0;

	float m_fpsCounter = 0;
	int m_fps = 0;

	bool IsPointInRect(Vector2 point, float x, float y, float w, float h);
};