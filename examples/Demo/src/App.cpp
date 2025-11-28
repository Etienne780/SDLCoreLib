#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Demo", SDLCore::Version(1, 0)) {
}

struct MovingRect {
    Vector4 rect; // x, y, w, h
    Vector2 velocity; // vx, vy
    
    MovingRect(Vector4 _rect, Vector2 _velocity) 
        : rect(_rect), velocity(_velocity) {
    }
};

static SDLCore::WindowID winFillID;
static SDLCore::WindowID winStrokeID;
static SDLCore::WindowID winPolygonID;
static SDLCore::WindowID winImageID;
static SDLCore::WindowID winTextID;
static SDLCore::WindowID winSoundID;

std::vector<MovingRect> movingRects;

std::vector<SDLCore::Vertex> vertices;
std::vector<Vector2> velocity;

SDLCore::InputAction testAction;
SDLCore::Texture exampleImage;

void InputActionTest() {
    using namespace SDLCore;
    static int type = 0;

    if (Input::KeyJustPressed(KeyCode::T)) {
        type = (type + 1) % 3;
        Log::Print();
        Log::Print("Changed Action type to '{}'", ((type == 0) ? "Pressed" : (type == 1) ? "Just pressed" : "Just released"));
    }

    switch (type)
    {
    case 0: {
        if (Input::ActionPressed(testAction))
            Log::Print("Action Pressed");
        break;
    }
    case 1: {
        if (Input::ActionJustPressed(testAction))
            Log::Print("Action Just Pressed");
        break;
    }
    case 2: {
        if (Input::ActionJustReleased(testAction))
            Log::Print("Action Just Released");
        break;
    }
    }

}

void InputTest() {
    using namespace SDLCore;
    static int g_inputTestType = 0;

    if (Input::KeyJustPressed(KeyCode::T)) {
        g_inputTestType = (g_inputTestType + 1) % 3;
        Log::Print();
        Log::Print("Switched input test mode to '{}'",
            (g_inputTestType == 0) ? "Keyboard" :
            (g_inputTestType == 1) ? "Mouse" :
            "Gamepad");
    }

    switch (g_inputTestType) {
    case 0: { // Keyboard
        for (int code = static_cast<int>(KeyCode::A); code <= static_cast<int>(KeyCode::Z); ++code) {
            KeyCode kc = static_cast<KeyCode>(code);
            if (Input::KeyPressed(kc))
                Log::Print("  KeyPressed: {}", kc);
            if (Input::KeyJustPressed(kc))
                Log::Print("  KeyJustPressed: {}", kc);
            if (Input::KeyJustReleased(kc))
                Log::Print("  KeyJustReleased: {}", kc);
        }
        break;
    }
    case 1: { // Mouse
        for (MouseButton mb : { MouseButton::LEFT, MouseButton::MIDDLE, MouseButton::RIGHT, MouseButton::X1, MouseButton::X2 }) {
            if (Input::MousePressed(mb))
                Log::Print("  MousePressed: {}", mb);
            if (Input::MouseJustPressed(mb))
                Log::Print("  MouseJustPressed: {}", mb);
            if (Input::MouseJustReleased(mb))
                Log::Print("  MouseJustReleased: {}", mb);
        }

        Vector2 pos = Input::GetMousePosition();
        Vector2 delta = Input::GetMouseDelta();
        int scroll = Input::GetScrollDir();
        Log::Print("  Position: ({}, {}), Delta: ({}, {}), Scroll: {}", pos.x, pos.y, delta.x, delta.y, scroll);
        break;
    }
    case 2: { // Gamepad
        Log::Print("  Gamepad input currently not implemented.");
        break;
    }
    }
}

SDLCore::SoundClip testSound;
SDLCore::SoundClip testSound2D_1;
SDLCore::SoundClip testSound2D_2;

bool MovePolygon();
void MoveRects();
void App::OnStart() {
    auto path = File::OpenFileDialog("Select a song", File::ConvertFilterString(".mp3, .wav"));
    testSound = SDLCore::SoundClip(path);
    testSound.SetVolume(0.2f);

    testSound2D_1 = testSound.CreateSubSound();
    testSound2D_2 = testSound.CreateSubSound();

    exampleImage = SDLCore::Texture("C:/Users/Admin/Pictures/Screenshots/Screenshot 2024-03-28 173226.png");

    {
        using namespace SDLCore;
        testAction.SetKeyAction({ KeyCode::W, KeyCode::Y, KeyCode::I, KeyCode::F2 });
        testAction.SetMouseAction({ MouseButton::LEFT, MouseButton::MIDDLE, MouseButton::RIGHT });

        Log::Print(testAction);
        Log::Print("Press 'T' to toggle");
    }
    
    CreateWindow(&winFillID, "FillRects", 800, 800);
    CreateWindow(&winStrokeID, "StrokeRects", 800, 800);
    CreateWindow(&winPolygonID, "Polygon", 800, 800);
    CreateWindow(&winImageID, "Image", 800, 800);
    CreateWindow(&winTextID, "Text", 800, 800);
    CreateWindow(&winSoundID, "Sound", 800, 800);

    exampleImage.CreateForWindow(winImageID);

    int count = 50;
    for (int i = 0; i < count; ++i) {
        Vector4 rect = Vector4{ float(10 + i * 30), float(10 + i * 20), float(17 + i * 3.234f), float(16 + i * 2.43f) };
        Vector2 velocity = Vector2{ float((i % 3) + 1), float((i % 2) + 1) };
        movingRects.emplace_back(rect, velocity);
    }

    {
        float x = 400.0f;
        float y = 400.0f;
        float size = 100.0f;

        vertices.emplace_back(Vector2{ x, y - size * 0.5f }, Vector4{ 0, 0, 0, 0 }, Vector2{ 0, 0 });
        vertices.emplace_back(Vector2{ x - size * 0.5f, y + size * 0.5f }, Vector4{ 0, 0, 0, 0 }, Vector2{ 0, 1 });
        vertices.emplace_back(Vector2{ x + size * 0.5f, y + size * 0.5f }, Vector4{ 0, 0, 0, 0 }, Vector2{ 1, 1 });

        velocity = { Vector2(1.0f, 0.4f), Vector2(0.25f, -0.8f), Vector2(-0.4f, 0.9f) };
    }

    SetFPSCap(APPLICATION_FPS_UNCAPPED);

    // SDLCore::Render::SetFont("C:/Users/Etienne Richter/AppData/Roaming/Opera Software/Opera GX Stable/Default/Extensions/igpdmclhhlcpoindmhkhillbfhdgoegm/6.12.0.7_0/assets/b25893558c7f1ad49e5e.ttf");
    SDLCore::Render::SetFontSize(120);
}

void App::OnUpdate() {
    if (GetWindowCount() <= 0) 
        Quit();

    using namespace SDLCore;
    namespace RE = SDLCore::Render;

    double timeSec = SDLCore::Time::GetTimeSec();
    float stroke = static_cast<float>(std::sin(timeSec * 2)) * 30.0f;

    MoveRects();

    if (winFillID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winFillID);

        InputActionTest();

        RE::SetWindowRenderer(winFillID);
        RE::SetColor(50, 50, 50);
        RE::Clear();

        RE::SetColor(0, 255, 0);
        std::vector<Vector4> fillRects;
        fillRects.reserve(movingRects.size());
        for (auto& mr : movingRects) fillRects.push_back(mr.rect);
        RE::FillRects(fillRects);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winFillID);
    }

    if (winStrokeID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winStrokeID);

        InputTest();

        RE::SetWindowRenderer(winStrokeID);
        RE::SetColor(0, 0, 50);
        RE::Clear();

        RE::SetStrokeWidth(std::abs(stroke));
        RE::SetInnerStroke(stroke > 0);
        RE::SetColor(255, 0, 0);
        std::vector<Vector4> strokeRects;
        strokeRects.reserve(movingRects.size());
        for (auto& mr : movingRects) strokeRects.push_back(mr.rect);
        RE::Rects(strokeRects);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winStrokeID);
    }

    if (winPolygonID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winPolygonID);
        RE::SetWindowRenderer(winPolygonID);
        RE::SetColor(10, 30, 70);
        RE::Clear();

        float time = Time::GetTimeSec() * 100;
        Vector3 color{ 
            static_cast<float>((std::sin(time * 1.3) * 0.5 + 0.5) * 255),
            static_cast<float>((std::sin(time * 0.9 + 2.0) * 0.5 + 0.5) * 255),
            static_cast<float>((std::sin(time * 1.7 + 4.0) * 0.5 + 0.5) * 255)
        };

        static Vector3 currentColor = color;

        if (MovePolygon()) {
            currentColor = color;
        }

        RE::SetColor(currentColor);
        RE::Polygon(vertices.data(), vertices.size());

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winPolygonID);
    }

    if (winImageID != SDLCORE_INVALID_ID) {
        // auto* win = GetWindow(winImageID);
        // float op = static_cast<float>((std::sin(Time::GetTimeSec()) + 1.0) * 0.5);
        // win->SetOpacity(op);
        
        Input::SetWindow(winImageID);
        RE::SetWindowRenderer(winImageID);
        RE::SetColor(120, 50, 70);
        RE::Clear();

       std::vector<Vertex> mrVertecics;
       std::vector<int> mrIndecies;
       mrVertecics.reserve(movingRects.size() * 4);
       mrIndecies.reserve(movingRects.size() * 6);
       int i = 0;
       for (auto& mr : movingRects) {
           mrVertecics.emplace_back(mr.rect.x, mr.rect.y, 255, 255, 255, 255, 0, 0);
           mrVertecics.emplace_back(mr.rect.x, mr.rect.y + mr.rect.w, 255, 255, 255, 255, 0, 1);
           mrVertecics.emplace_back(mr.rect.x + mr.rect.z, mr.rect.y + mr.rect.w, 255, 255, 255, 255, 1, 1);
           mrVertecics.emplace_back(mr.rect.x + mr.rect.z, mr.rect.y, 255, 255, 255, 255, 1, 0);

           int base = i;
           mrIndecies.push_back(base);
           mrIndecies.push_back(base + 1);
           mrIndecies.push_back(base + 2);

           mrIndecies.push_back(base);
           mrIndecies.push_back(base + 2);
           mrIndecies.push_back(base + 3);

           i += 4;
       }

       RE::Polygon(mrVertecics.data(), mrVertecics.size(), &exampleImage,
           mrIndecies.data(), mrIndecies.size());

        // for (auto& mr : movingRects)
        //     RE::Texture(exampleImage, mr.rect.x, mr.rect.y, mr.rect.z, mr.rect.w);
            // exampleImage.Render(mr.rect.x, mr.rect.y, mr.rect.z, mr.rect.w);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winImageID);
    }

    static float frameRate = 0;
    if (winTextID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winTextID);
        RE::SetWindowRenderer(winTextID);
        RE::SetColor(23, 43, 89);
        RE::Clear();

        int scrollDir = 0;
        static float currentTextSize = 32;
        if (Input::GetScrollDir(scrollDir)) {
            float step = 2.0f;
            float fontsize = currentTextSize + scrollDir * step;
            currentTextSize = std::clamp(fontsize, 6.0f, 144.0f);
            
        }
        RE::SetFontSize(currentTextSize);

        static Vector2 pos{ 20, 100 };
        float speed = 2;
        if (Input::KeyPressed(KeyCode::W)) pos.y += speed * Time::GetDeltaTime();
        if (Input::KeyPressed(KeyCode::S)) pos.y -= speed * Time::GetDeltaTime();
        if (Input::KeyPressed(KeyCode::A)) pos.x += speed * Time::GetDeltaTime();
        if (Input::KeyPressed(KeyCode::D)) pos.x -= speed * Time::GetDeltaTime();

        std::string msg = "Hello World!";
        float textHeight = RE::GetTextHeight(msg);
        RE::SetColor(0);
        RE::FillRect(pos.x, pos.y, RE::GetTextWidth(msg), textHeight);
        RE::SetColor(0, 255, 0);
        RE::Text(msg, pos.x, pos.y);

        msg = FormatUtils::toString(frameRate);
        RE::SetColor(0);
        RE::FillRect(pos.x, pos.y + textHeight + 10, RE::GetTextWidth(msg), RE::GetTextHeight(msg));
        RE::SetColor(255);
        RE::Text(msg, pos.x, pos.y + textHeight + 10);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winTextID);
    }

    if (winSoundID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winSoundID);
        RE::SetWindowRenderer(winSoundID);
        RE::SetColor(45, 87, 32);
        RE::Clear();

        auto PointInRect = [&](const Vector2& p, float x, float y, float w, float h) -> bool {
            return (p.x > x && p.x <= x + w) && (p.y > y && p.y <= y + h);
            };

        auto DrawButton = [&](float x, float y, float w, float h) -> bool {
            Vector4 col = RE::GetActiveColor();
            bool hover = PointInRect(Input::GetMousePosition(), x, y, w, h);
            bool pressed = false;

            if (hover) {
                pressed = Input::MouseJustPressed(MouseButton::LEFT);
                if (Input::MousePressed(MouseButton::LEFT))
                    RE::SetColor(col * 0.65f);
                else
                    RE::SetColor(col * 0.75f);
            }

            RE::FillRect(x, y, w, h);
            RE::SetColor(col);
            return pressed;
            };

        auto* win = GetWindow(winSoundID);
        static bool selectedMode = false; // false = Simple-test, true = 2D audio

        static Vector2 playerPos{ 0, 0 };
        static Vector2 soundPos_1{ 0, 0 };
        static Vector2 soundPos_2{ 0, 0 };

        static bool init2D = false;
        static bool playing = false;

        if (win) {
            float halfH = win->GetHeight() * 0.5f;
            float halfW = win->GetWidth() * 0.5f;
            float btnW = 200;
            float btnH = 50;

            // initialize sound source once
            if (!init2D) {
                playerPos = Vector2(0, 0);
                soundPos_1 = Vector2(500, 0);
                soundPos_2 = Vector2(- 500, 0);
                init2D = true;
            }

            // ============ SIMPLE TEST MODE ============
            if (!selectedMode) {
                float x = 20;
                float y = 80;
                float bw = 220;
                float bh = 45;

                RE::SetColor(255);
                if (DrawButton(x, y, bw, bh)) {
                    SoundManager::PlaySound(testSound);
                    playing = true;
                }
                RE::SetColor(0);
                RE::Text("Play sound", x + 10, y + 10);

                RE::SetColor(255);
                if (DrawButton(x, y + 55, bw, bh)) {
                    SoundManager::PauseSound(testSound);
                }
                RE::SetColor(0);
                RE::Text("Pause", x + 10, y + 10 + 55);

                RE::SetColor(255);
                if (DrawButton(x, y + 110, bw, bh)) {
                    SoundManager::StopSound(testSound);
                    playing = false;
                }
                RE::SetColor(0);
                RE::Text("Stop", x + 10, y + 10 + 110);

                RE::SetColor(255);
                RE::SetFontSize(24);
                RE::Text("Simple-test", 10, 10);
            }

            // ============ 2D AUDIO MODE ============
            else {
                // camera follows player
                Vector2 cam = playerPos;

                // movement
                float speed = 1.0f * Time::GetDeltaTime();
                if (Input::KeyPressed(KeyCode::W)) playerPos.y -= speed;
                if (Input::KeyPressed(KeyCode::S)) playerPos.y += speed;
                if (Input::KeyPressed(KeyCode::A)) playerPos.x -= speed;
                if (Input::KeyPressed(KeyCode::D)) playerPos.x += speed;

                Vector2 winSize = win->GetSize();
                Vector2 halfWinSize = winSize * 0.5f;
                float pw = 40, ph = 40;
                float sw = 50, sh = 50;

                Vector2 screenPlayer = playerPos - cam + halfWinSize;
                Vector2 screenSound_1 = soundPos_1 - cam + halfWinSize;
                Vector2 screenSound_2 = soundPos_2 - cam + halfWinSize;

                // draw sound source
                RE::SetColor(255, 200, 50);
                RE::FillRect(screenSound_1.x - sw/2, screenSound_1.y - sh/2, sw, sh);
                RE::FillRect(screenSound_2.x - sw/2, screenSound_2.y - sh/2, sw, sh);

                // draw player
                RE::SetColor(0, 200, 255);
                RE::FillRect(screenPlayer.x - pw/2, screenPlayer.y - ph/2, pw, ph);

                testSound2D_1.Set2D(soundPos_1, playerPos, 800.0f, 0.4f);
                testSound2D_2.Set2D(soundPos_2, playerPos, 800.0f, 0.4f);

                if (Input::MouseJustPressed(MouseButton::LEFT)) {
                    if (!SoundManager::IsPlaying(testSound2D_1)) {
                        SoundManager::StopSound(testSound2D_2);
                        SoundManager::PlaySound(testSound2D_1);
                    }
                    else {
                        SoundManager::StopSound(testSound2D_1);
                        SoundManager::PlaySound(testSound2D_2);
                    }
                }

                RE::SetColor(255);
                RE::SetFontSize(24);
                RE::Text("2D-Audio", 10, 10);
                RE::Text(Log::GetFormattedString("Current Song {}{}", (SoundManager::IsPlaying(testSound2D_1) ? "Sound 1" : ""), (SoundManager::IsPlaying(testSound2D_2) ? "Sound 2" : "")), 10, 10 + RE::GetTextHeight("2D-Audio") + 5);
            }

            // ========= MODE SELECTION =========
            RE::SetColor(255);
            if (DrawButton(halfW - btnW - 2.5f, 0, btnW, btnH))
                selectedMode = false;
            if (!selectedMode) {
                RE::SetColor(100, 150, 200);
                RE::SetInnerStroke(true);
                RE::SetStrokeWidth(3);
                RE::Rect(halfW - btnW - 2.5f, 0, btnW, btnH);
            }

            RE::SetColor(255);
            if (DrawButton(halfW + 2.5f, 0, btnW, btnH))
                selectedMode = true;
            if (selectedMode) {
                RE::SetColor(100, 150, 200);
                RE::SetInnerStroke(true);
                RE::SetStrokeWidth(3);
                RE::Rect(halfW + 2.5f, 0, btnW, btnH);
            }
        }

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winSoundID);
    }

    static double lastTime = -1;
    if (lastTime < Time::GetTimeSec()) {
        frameRate = SDLCore::Time::GetFrameRate();
        // Log::Print(frameRate);
        lastTime = Time::GetTimeSec() + 0.5;
    }
}

void App::OnQuit() {
    bool isGay = false;
    if (isGay)
        isGay = true;
}

bool MovePolygon() {
    using namespace SDLCore;
    auto* app = Application::GetInstance();

    float speed = 200;
    bool touch = false;
    Window* win = nullptr;
    if (winPolygonID != SDLCORE_INVALID_ID)
        win = app->GetWindow(winPolygonID);

    if (!win)
        return false;

    for (size_t i = 0; i < vertices.size(); i++) {
        Vertex vert = vertices[i];
        Vector2 vel = velocity[i];

        Vector2& pos = vert.position;
        pos.x += vel.x * static_cast<float>(Time::GetDeltaTimeSec()) * speed;
        pos.y += vel.y * static_cast<float>(Time::GetDeltaTimeSec()) * speed;

        if (pos.x < 0 || pos.x > win->GetWidth()) {
            vel.x *= -1;
            pos.x += vel.x * static_cast<float>(Time::GetDeltaTimeSec()) * speed;
            touch = true;
        }

        if (pos.y < 0 || pos.y > win->GetHeight()) {
            vel.y *= -1;
            pos.y += vel.y * static_cast<float>(Time::GetDeltaTimeSec()) * speed;
            touch = true;
        }

        vertices[i] = vert;
        velocity[i] = vel;
    }

    return touch;
}

void MoveRects() {
    using namespace SDLCore;
    auto* app = Application::GetInstance();

    Window* win = nullptr;
    if (winStrokeID != SDLCORE_INVALID_ID)
        win = app->GetWindow(winStrokeID);
    else if (winFillID != SDLCORE_INVALID_ID)
        win = app->GetWindow(winFillID);
    else if (winImageID != SDLCORE_INVALID_ID)
        win = app->GetWindow(winImageID);

    if (!win)
        return;

    for (auto& mr : movingRects) {
        mr.rect.x += mr.velocity.x * static_cast<float>(Time::GetDeltaTimeSec()) * 500.0f;
        mr.rect.y += mr.velocity.y * static_cast<float>(Time::GetDeltaTimeSec()) * 500.0f;

        if (mr.rect.x <= 0 || mr.rect.x + mr.rect.z >= win->GetWidth()) mr.velocity.x *= -1;
        if (mr.rect.y <= 0 || mr.rect.y + mr.rect.w >= win->GetHeight()) mr.velocity.y *= -1;

        if (mr.rect.x + mr.rect.z < 0 || mr.rect.x > win->GetWidth() ||
            mr.rect.y + mr.rect.w < 0 || mr.rect.y > win->GetHeight()) {
            mr.rect.x = win->GetWidth() * 0.5f;
            mr.rect.y = win->GetHeight() * 0.5f;
        }
    }
}