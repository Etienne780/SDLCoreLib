#include <array>

#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

void SetupUI();
void RenderUI(SDLCore::WindowID winID);

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);
    SetupUI();
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

        RenderUI(m_winID);

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);

        if (Time::GetFrameCount() % 240 == 0)
            Log::Info("FPS: {}", Time::GetFrameRate());
    }
}

void App::OnQuit() {
}

SDLCore::UI::UIContext* context = nullptr;

SDLCore::UI::UIStyle stretchHorStyle{ "Stretch-Hor" };
SDLCore::UI::UIStyle stretchVerStyle{ "Stretch-Ver" };
SDLCore::UI::UIStyle stretchStyle{ "Stretch" };

SDLCore::UI::UIStyle centerHorStyle{ "Center-Hor" };
SDLCore::UI::UIStyle centerVerStyle{ "Center-Ver" };
SDLCore::UI::UIStyle centerStyle{ "Center" };

SDLCore::UI::UIStyle rootStyle{ "Root" };
SDLCore::UI::UIStyle headerStyle{ "Header" };
SDLCore::UI::UIStyle headerContainerLeftStyle{ "HeaderContainer-Left" };
SDLCore::UI::UIStyle headerContainerRightStyle{ "HeaderContainer-Right" };

SDLCore::UI::UIStyle gridStyle{ "Grid" };
SDLCore::UI::UIStyle gridContainerStyle{ "Grid-container" };
SDLCore::UI::UIStyle gridRowStyle{ "Grid-row" };
SDLCore::UI::UIStyle gridCellStyle{ "Grid-cell" };
SDLCore::UI::UIStyle gridCellInnerStyle{ "Grid-cell-inner" };

SDLCore::UI::UIStyle keyboardStyle{ "Keyboard" };
SDLCore::UI::UIStyle keyboardContainerStyle{ "Keyboard-Container" };
SDLCore::UI::UIStyle keyboardRowStyle{ "Keyboard-Row" };
SDLCore::UI::UIStyle keyboardBTNStyle{ "Keyboard-BTN" };
SDLCore::UI::UIStyle keyboardBTNContainerStyle{ "Keyboard-BTN-Container" };
SDLCore::UI::UIStyle keyboardBTNTextStyle{ "Keyboard-BTN-Text" };

SDLCore::UI::UIColorID backgroundColor;

constexpr float cellSize = 75.0f;
constexpr float keyboardWidth = cellSize * 5 + 125.0f;
std::array<char, 26> letters = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M'
};

void SetupUI() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    context = UI::CreateContext();

    backgroundColor = UI::UIRegistry::RegisterColor(Vector4(25.0f, 25.0f, 25.0f, 255.0f));

    stretchHorStyle
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::width, 100.0f);

    stretchVerStyle
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::height, 100.0f);

    stretchStyle
        .Merge(stretchHorStyle)
        .Merge(stretchVerStyle);

    centerHorStyle
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER);

    centerVerStyle
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER);

    centerStyle
        .Merge(centerHorStyle)
        .Merge(centerVerStyle);

    rootStyle
        .Merge(centerStyle)
        .Merge(stretchStyle)
        .SetValue(Prop::alignVertical, UI::UIAlignment::START)
        .SetValue(Prop::padding, Vector4(18.0f))
        .SetValue(Prop::backgroundColor, backgroundColor)
        .SetValue(Prop::borderColor, Vector4(50.0f, 50.0f, 50.0f, 255.0f))
        .SetValue(Prop::borderInset, true)
        .SetValue(Prop::borderWidth, 10.0f);

    headerStyle
        .Merge(stretchHorStyle)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::height, 15.0f);

    headerContainerLeftStyle
        .Merge(stretchStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::START)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
        .SetValue(Prop::width, 50.0f);

    headerContainerRightStyle
        .Merge(stretchStyle)
        .Merge(headerContainerLeftStyle)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::END);

    gridStyle
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::START)
        .SetValue(Prop::alignVertical, UI::UIAlignment::START)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::width, cellSize * 5)
        .SetValue(Prop::height, cellSize * 6)
        .SetValue(Prop::margin, Vector4(0.0f, 0.0f, 8.0f, 0.0f));

    gridContainerStyle
        .Merge(stretchStyle)
        .Merge(centerHorStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignVertical, UI::UIAlignment::START)
        .SetValue(Prop::height, 65.0f);

    gridRowStyle
        .Merge(stretchHorStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::height, cellSize);

    gridCellStyle
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::width, cellSize)
        .SetValue(Prop::height, cellSize)
        .SetValue(Prop::padding, Vector4(4.0f));

    gridCellInnerStyle
        .SetValue(Prop::borderInset, true)
        .SetValue(Prop::borderWidth, 2.0f)
        .SetValue(Prop::borderColor, Vector4(100.0f, 100.0f, 100.0f, 255.0f));

    keyboardStyle
        .Merge(stretchVerStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::START)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PX)
        .SetValue(Prop::width, keyboardWidth)
        .SetValue(Prop::backgroundColor, Vector4(255.0f, 255.0f, 255.0f, 255.0f));

    keyboardContainerStyle
        .Merge(stretchStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
        .SetValue(Prop::alignVertical, UI::UIAlignment::END)
        .SetValue(Prop::margin, Vector4(0, 10.0f, 10.0f, 10.0f))
        .SetValue(Prop::padding, Vector4(8.0f))
        .SetValue(Prop::height, 20.0f);

    keyboardRowStyle
        .Merge(stretchHorStyle)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::height, 100.0f/3.0f);

    keyboardBTNStyle
        .Merge(stretchStyle)
        .SetValue(Prop::backgroundColor, Vector4(100, 100, 100, 255));

    keyboardBTNContainerStyle
        .Merge(stretchVerStyle)
        .SetValue(Prop::width, keyboardWidth / 10.0f)
        .SetValue(Prop::padding, Vector4(2.0f));

    keyboardBTNTextStyle
        .SetValue(Prop::textSize, 24.0f)
        .SetValue(Prop::textColor, Vector4(255.0f));
}

void RenderUI(SDLCore::WindowID winID) {
    using namespace SDLCore;

    auto drawLetterBtns = [](int start, int end) {
        for (int i = start; i < end; i++) {
            if (i >= letters.size())
                continue;
            char c = letters[i];

            UI::BeginFrame("Container-" + std::string(1, c), centerStyle, keyboardBTNContainerStyle);
            {
                UI::BeginFrame(c, centerStyle, keyboardBTNStyle);
                {
                    UI::Text("letter", std::string(1, c), keyboardBTNTextStyle);
                }
                UI::EndFrame();
            }
            UI::EndFrame();
        }
    };

    UI::SetContextWindow(context, winID);
    UI::BindContext(context);

    UI::BeginFrame("Root", rootStyle);
    {
        UI::BeginFrame("Header", headerStyle);
        {
            UI::BeginFrame("HeaderContaienr-Left", headerContainerLeftStyle);
            {
                UI::Text("title", "The New York Times Games");
            }
            UI::EndFrame();

            UI::BeginFrame("HeaderContaienr-Right", headerContainerRightStyle);
            {
                UI::BeginFrame("box1");
                UI::EndFrame();

                UI::BeginFrame("box2");
                UI::EndFrame();
                
                UI::BeginFrame("box3");
                UI::EndFrame();
            }
            UI::EndFrame();
        }
        UI::EndFrame();

        UI::BeginFrame("Grid-container", gridContainerStyle);
        {
            UI::BeginFrame("Grid", gridStyle);
            {
                for (int i = 0; i < 6; i++) {
                    UI::BeginFrame("Grid-row-" + std::to_string(i), gridRowStyle);
                    {
                        for (int j = 0; j < 5; j++) {
                            UI::BeginFrame("Grid-cell-" + std::to_string(j), centerStyle, gridCellStyle);
                            {
                                UI::BeginFrame("Container", stretchStyle, gridCellInnerStyle);
                                {

                                }
                                UI::EndFrame();
                            }
                            UI::EndFrame();
                        }
                    }
                    UI::EndFrame();
                }
            }
            UI::EndFrame();
        }
        UI::EndFrame();

        UI::BeginFrame("Keyboard-Container", keyboardContainerStyle);
        {
            UI::BeginFrame("Keyboard", keyboardStyle);
            {
                UI::BeginFrame("Row-1", keyboardRowStyle);
                {
                    drawLetterBtns(0, 10);
                }
                UI::EndFrame();

                UI::BeginFrame("Row-2", keyboardRowStyle);
                {
                    drawLetterBtns(10, 19);
                }
                UI::EndFrame();

                UI::BeginFrame("Row-3", keyboardRowStyle);
                {
                    UI::BeginFrame("Enter", keyboardBTNContainerStyle);
                    {
                        UI::BeginFrame("container", centerStyle, keyboardBTNStyle);
                        {
                            UI::Text("text", "ENT", keyboardBTNTextStyle);
                        }
                        UI::EndFrame();
                    }
                    UI::EndFrame();
                    
                    drawLetterBtns(19, 26);

                    UI::BeginFrame("Delete", keyboardBTNContainerStyle);
                    {
                        UI::BeginFrame("container", centerStyle, keyboardBTNStyle);
                        {
                            UI::Text("text", "DEL", keyboardBTNTextStyle);
                        }
                        UI::EndFrame();
                    }
                    UI::EndFrame();
                }
                UI::EndFrame();
            }
            UI::EndFrame();
        }
        UI::EndFrame();
    }
    UI::EndFrame();
}