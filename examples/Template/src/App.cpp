#include "App.h"
#include <CoreLib/OTNFile.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

template<>
void ToOTNDataType<Vector2>(OTNObjectBuilder& obj, const Vector2& value) {
    obj.SetObjectName("Vector2");
    obj.AddNames("x", "y");
    obj.AddData(value.x, value.y);
}

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);

    OTNObject weaponObj("weapon");
    weaponObj.SetNames("name", "ammo", "size");
    weaponObj.AddDataRow("SMG", 12.0f, Vector2(2.0f, 22.0f));

    OTNObject playerObj("player");
    playerObj.SetNames("name", "weapon");
    playerObj.AddDataRow("steve", weaponObj);

    int i;
}

void App::OnUpdate() {
    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        // =====================
        // Input/Calculations here ...
        // =====================

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        // =====================
        // Render code here ...
        // =====================

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
}