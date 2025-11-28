#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Pong", SDLCore::Version(1, 0)) {
    SetFPSCap(APPLICATION_FPS_VSYNC_ON);
}

void App::OnStart() {
    m_pong.Start(m_winWidth, m_winHeight);
    m_startGame = true;
    m_paddleLeftScore = 0;
    m_paddleRightScore = 0;
    SetCountDown(4);
    
    auto* win = CreateWindow(&m_winID, GetName(), m_winWidth, m_winHeight);
    win->SetResizable(false);
}

void App::OnUpdate() {
    if (m_winID != SDLCORE_INVALID_ID) {
        UpdateCountDown();
        Input();
        if (!m_pause && !IsCountDownActive())
            m_pong.Update(SDLCore::Time::GetDeltaTimeSec());
        Render();
    }

    if (GetWindowCount() <= 0)
        this->Quit();
}

void App::OnQuit() {
}

void App::SetCountDown(float time) {
    m_startCountDown = time;
    m_startCounter = time;
}

bool App::IsCountDownActive() const {
    return m_startCountDown > 0;
}

void App::UpdateCountDown() {
    if (m_startCountDown <= 0) {
        m_startGame = false;
        m_showScore = 0;// stop showing score when match begins
        return;
    }

    m_startCounter -= SDLCore::Time::GetDeltaTimeSec();
    if (m_startCounter <= 0) {
        m_startCountDown = 0;
    }
}

void App::Input() {
    using namespace SDLCore;
    Input::SetWindow(m_winID);

    if (!m_startGame && Input::KeyJustPressed(KeyCode::ESCAPE)) {
        m_pause = !m_pause;
        SetCountDown(3.5f);
    }

    // if game paused stop game input
    if (!m_pause && !IsCountDownActive()) {
        if (Input::KeyPressed(KeyCode::W)) {
            m_pong.SetPaddleMoveDir(Direction::LEFT, Direction::UP);
        }

        if (Input::KeyPressed(KeyCode::S)) {
            m_pong.SetPaddleMoveDir(Direction::LEFT, Direction::DOWN);
        }

        if (Input::KeyPressed(KeyCode::O)) {
            m_pong.SetPaddleMoveDir(Direction::RIGHT, Direction::UP);
        }

        if (Input::KeyPressed(KeyCode::L)) {
            m_pong.SetPaddleMoveDir(Direction::RIGHT, Direction::DOWN);
        }
    }
}

void App::Render() {
    namespace RE = SDLCore::Render;
    
    float fwinWidth = static_cast<float>(m_winWidth);
    float fwinHeight = static_cast<float>(m_winHeight);

    RE::SetWindowRenderer(m_winID);
    // Needs to be set becaus SDL renderer default is none
    SDLCore::Render::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
    RE::SetColor(0);
    RE::Clear();

    RE::SetColor(100);
    RE::SetStrokeWidth(2);
    RE::Line(fwinWidth/2, 0.0f, fwinWidth/2.0f, fwinHeight);

    RE::SetColor(255);
    float ballSize = m_pong.GetBallSize();
    Vector2 ballPos = m_pong.GetBallPose();
    RE::FillRect(ballPos, ballSize, ballSize);

    Vector2 paddleSize = m_pong.GetPaddleSize();
    Vector2 paddleLeftPos;
    if (m_pong.TryGetPaddlePos(paddleLeftPos, Direction::LEFT)) {
        RE::FillRect(paddleLeftPos, paddleSize);
    }

    Vector2 paddleRightPos;
    if (m_pong.TryGetPaddlePos(paddleRightPos, Direction::RIGHT)) {
        RE::FillRect(paddleRightPos, paddleSize);
    }

    // ================= UI =================
    // Border
    RE::SetColor(100);
    RE::SetInnerStroke(true);
    RE::SetStrokeWidth(2);
    RE::Rect(0, 0, fwinWidth, fwinHeight);

    // Score
    int pLScore = 0;
    int pRScore = 0;
    int scoredPoint = 0;
    if (m_pong.TryGetScore(pLScore, Direction::LEFT)) {
        // if point was scored
        if (m_paddleLeftScore != pLScore) {
            m_showScore = -1;
            m_paddleLeftScore = pLScore;
            SetCountDown(3.5f);
        }
    }

    if (m_pong.TryGetScore(pRScore, Direction::RIGHT)) {
        // if point was scored
        if (m_paddleRightScore != pRScore) {
            m_showScore = 1;
            m_paddleRightScore = pRScore;
            SetCountDown(3.5f);
        }
    }

    if (!m_pause && m_showScore != 0) {
        std::string strLScore = FormatUtils::toString(m_paddleLeftScore);
        std::string strRScore = FormatUtils::toString(m_paddleRightScore);
        bool scoreSideL = m_showScore == -1;

        // Left
        RE::SetColor((scoreSideL) ? 255 : 150);
        RE::SetFontSize((scoreSideL) ? 96.0f : 64.0f);
        RE::Text(strLScore, fwinWidth/4 - RE::GetTextWidth(strLScore)/2,
            fwinHeight/2 - RE::GetTextHeight(strLScore)/2);

        // Right
        RE::SetColor((!scoreSideL) ? 255 : 150);
        RE::SetFontSize((!scoreSideL) ? 96.0f : 64.0f);
        RE::Text(strRScore, fwinWidth - fwinWidth/4 - RE::GetTextWidth(strRScore)/2,
            fwinHeight/2 - RE::GetTextHeight(strRScore)/2);
    }

    // CountDown
    if (IsCountDownActive() && !m_pause) {
        int iCount = static_cast<int>(m_startCounter);
        std::string msg = FormatUtils::toString(iCount);
        float rectSize = 75;

        RE::SetColor(0, 200);
        RE::FillRect(fwinWidth/2 - rectSize/2, fwinHeight/2 - rectSize/2, rectSize, rectSize);

        RE::SetFontSize(46);
        RE::SetColor(255);
        RE::Text(msg, fwinWidth/2 - RE::GetTextWidth(msg)/2, fwinHeight/2 - RE::GetTextHeight(msg)/2);
    }

    // How to play info
    if (m_startGame && IsCountDownActive()) {
        float a = (255 / m_startCountDown) * m_startCounter;
        std::string msg = "W/S = Move left paddle";
        RE::SetFontSize(32);
        RE::SetColor(255, a);
        RE::Text(msg, 20, 20);
        RE::Text("O/L = Move right paddle", 20, 20 + RE::GetTextHeight(msg) + 7);
    }

    // Pause
    if (m_pause) {
        RE::SetColor(0, 200);
        RE::FillRect(0, 0, fwinWidth, fwinHeight);

        std::string msg = "Pause";
        RE::SetFontSize(46);
        float strPauseH = RE::GetTextWidth(msg);
        RE::SetColor(255);
        RE::Text(msg, fwinWidth/2 - strPauseH /2, 10);

        std::string strLScore = FormatUtils::toString(m_paddleLeftScore);
        std::string strRScore = FormatUtils::toString(m_paddleRightScore);

        // Left/Right score
        RE::SetColor(255);
        RE::Text(strLScore, fwinWidth/4 - RE::GetTextWidth(strLScore)/2, strPauseH + 15);
        RE::Text(strRScore, fwinWidth - fwinWidth/4 - RE::GetTextWidth(strRScore)/2, strPauseH + 15);

        auto DrawButton = [this](float x, float y, float w, float h) -> bool {
            bool pressed = SDLCore::Input::MouseJustPressed(SDLCore::MouseButton::LEFT);
            bool inRect = IsPointInRect(SDLCore::Input::GetMousePosition(), x, y, w, h);
            Vector4 activeC = RE::GetActiveColor();
            if (pressed && inRect)
                RE::SetColor(Vector3(activeC * 0.6f), activeC.w);
            else if(inRect)
                RE::SetColor(Vector3(activeC * 0.75f), activeC.w);

            RE::FillRect(x, y, w, h);
            RE::SetColor(activeC);
            return pressed && inRect;
        };

        std::string strResume = "Resume";
        std::string strQuit = "Quit";
        float yOffset = 150;
        float btnWidth = 200;
        float btnHeight = 60;
        float btnPadding = 10;
        
        // Resume
        RE::SetFontSize(46);
        RE::SetColor(255);
        if (DrawButton(fwinWidth/2 - btnWidth/2, strPauseH + yOffset, btnWidth, btnHeight)) {
            m_pause = false;
            SetCountDown(3.5f);
        }
        RE::SetColor(0);
        RE::Text(strResume, fwinWidth/2 - RE::GetTextWidth(strResume)/2, 
            strPauseH + yOffset + RE::GetTextHeight(strResume)/2);
        
        // Quit
        RE::SetColor(255);
        if (DrawButton(fwinWidth/2 - btnWidth/2, strPauseH + btnHeight + btnPadding + yOffset, btnWidth, btnHeight)) {
            this->Quit();
        }
        RE::SetColor(0);
        RE::Text(strQuit, fwinWidth/2 - RE::GetTextWidth(strQuit)/2, 
            strPauseH + btnHeight + btnPadding + yOffset + RE::GetTextHeight(strQuit)/2);
    }

    RE::Present();
}

bool App::IsPointInRect(Vector2 point, float x, float y, float w, float h) {
    return (point.x > x && point.x < x + w) && (point.y > y && point.y < y + h);
}