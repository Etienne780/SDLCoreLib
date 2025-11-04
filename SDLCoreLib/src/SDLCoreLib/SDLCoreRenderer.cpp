#include <SDL3/SDL.h>
#include <CoreLib/Log.h>
#include <Application.h>


#include "SDLCoreRenderer.h"

namespace SDLCore::Renderer {

	static std::shared_ptr<SDL_Renderer> m_renderer;

    SDL_Renderer* GetActiveRenderer() {
        return m_renderer.get();
    }

    static std::shared_ptr<SDL_Renderer> GetActiveRenderer(const char* func) {
        if (!m_renderer) {
            Log::Error("SDLCore::Renderer::{}: Current renderer is null", func);
        }
        return m_renderer;
    }

    void SetWindowRenderer(WindowID winID) {
        if (winID.value == SDLCORE_INVALID_ID) {
            m_renderer.reset();
            return;
        }

        auto app = Application::GetInstance();
        auto win = app->GetWindow(winID);

        if (!win) {
            m_renderer.reset();
            return;
        }

        if (!win->HasRenderer()) {
            Log::Error("SDLCore::Renderer::SetWindowRenderer: Renderer of window '{}' is null or destroyed!", win->GetName());
            m_renderer.reset();
            return;
        }

        std::shared_ptr<SDL_Renderer> rendererShared = win->GetSDLRenderer();
        if (!rendererShared) {
            Log::Error("SDLCore::Renderer::SetWindowRenderer: Renderer of window '{}' is null or destroyed!", win->GetName());
            m_renderer.reset();
            return;
        }

        m_renderer = rendererShared;
    }

    void Clear() {
        auto renderer = GetActiveRenderer("Clear");
        if (!renderer) 
            return;
        SDL_RenderClear(renderer.get());
    }

    void Present() {
        auto renderer = GetActiveRenderer("Present");
        if (!renderer)
            return;
        SDL_RenderPresent(renderer.get());
    }

    #pragma region Color

    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        auto renderer = GetActiveRenderer("SetColor");
        if (!renderer)
            return;
        SDL_SetRenderDrawColor(renderer.get(), r, g, b, a);
    }

    void SetColor(Uint8 r, Uint8 g, Uint8 b) {
        SetColor(r, g, b, 255);
    }

    void SetColor(Uint8 brightness, Uint8 alpha) {
        SetColor(brightness, brightness, brightness, alpha);
    }

    void SetColor(Uint8 brightness) {
        SetColor(brightness, brightness, brightness, 255);
    }

    void SetColor(const Vector4& rgba) {
        SetColor(static_cast<Uint8>(rgba.x), static_cast<Uint8>(rgba.y), static_cast<Uint8>(rgba.z), static_cast<Uint8>(rgba.w));
    }

    void SetColor(const Vector3& rgb) {
        SetColor(static_cast<Uint8>(rgb.x), static_cast<Uint8>(rgb.y), static_cast<Uint8>(rgb.z), 255);
    }

    #pragma endregion

    #pragma region Primitives

    #pragma region Rectangle

    void FillRect(int x, int y, int w, int h) {
        auto renderer = GetActiveRenderer("FillRect");
        if (!renderer)
            return;
        SDL_FRect rect{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) };
        SDL_RenderFillRect(renderer.get(), &rect);
    }

    void FillRect(const Vector2& pos, int w, int h) {
        FillRect(static_cast<int>(pos.x), static_cast<int>(pos.y), w, h);
    }

    void FillRect(int x, int y, const Vector2& size) {
        FillRect(x, y, static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void FillRect(const Vector2& pos, const Vector2& size) {
        FillRect(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(size.x), static_cast<int>(size.y));
    }

    #pragma endregion

    #pragma endregion

}