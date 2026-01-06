#include "App.h"
#include <CoreLib/OTNFile.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

template<>
void OTN::ToOTNDataType<Vector2>(OTN::OTNObjectBuilder& obj, const Vector2& value) {
    obj.SetObjectName("Vector2");
    obj.AddNames("x", "y");
    obj.AddData(value.x, value.y);
}

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);

    OTN::OTNObject weaponObj("weapon");
    weaponObj.SetNames("name", "ammo", "size");
    weaponObj.AddDataRow("SMG", 12.0f, Vector2(2.0f, 22.0f));
    weaponObj.AddDataRow("SMG", 2.0f, Vector2(4.0f, 34.0f));
    weaponObj.AddDataRow("SMG", 4.0f, Vector2(2.0f, 22.0f));

    OTN::OTNObject floatTest("float");
    floatTest.SetNames("val1", "val2", "val3", "val4", "val5", "val6", "val7");
    floatTest.AddDataRow(22.0f, 324.0f, 3245.0f, 22.0f, 56.0f, 876.0f, 54.0f);

    OTN::OTNWriter writer;
    writer.UseDefName(true);
    writer.UseDefType(true);
    writer.UseOptimizations(false);

    writer.AppendObject(weaponObj);
    writer.AppendObject(floatTest);
    if (!writer.Save("J:/file")) {
        Log::Error(writer.GetError());
    }

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