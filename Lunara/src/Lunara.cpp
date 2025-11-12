#include <cmath>
#include <vector>
#include <CoreLib/Math/Vector4.h>
#include "Lunara.h"

Lunara::Lunara()
    : Application("Lunara", SDLCore::Version(1, 0)) {
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

bool MovePolygon();
void MoveRects();
void Lunara::OnStart() {
    // exampleImage = SDLCore::Texture(SystemFilePath("C:/Users/Admin/Pictures/Screenshots/Screenshot 2024-03-28 173226.png"));

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

        velocity = { Vector2(1.0f, 0.4f), Vector2(0.25f, -0.8f), Vector2(-0.4f, 0.9) };
    }

    SetFPSCap(APPLICATION_FPS_UNCAPPED);
}

void Lunara::OnUpdate() {
    if (GetWindowCount() <= 0) 
        Quit();

    using namespace SDLCore;
    namespace RE = SDLCore::Renderer;

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
        RE::Polygon(vertices);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winPolygonID);
    }

    if (winImageID != SDLCORE_INVALID_ID) {
        Input::SetWindow(winImageID);
        RE::SetWindowRenderer(winImageID);
        RE::SetColor(120, 50, 70);
        RE::Clear();

        for (auto& mr : movingRects)
            exampleImage.Render(mr.rect.x, mr.rect.y, mr.rect.z, mr.rect.w);

        RE::Present();

        // needs to be called after using the window. becaus nullptr and so ...
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            RemoveWindow(winImageID);
    }
    static double lastTime = -1;
    if (lastTime < Time::GetTimeSec()) {
        Log::Print(SDLCore::Time::GetFrameRate());
        lastTime = Time::GetTimeSec() + 0.5;
    }
}

void Lunara::OnQuit() {
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