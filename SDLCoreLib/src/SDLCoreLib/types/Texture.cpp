#include <SDL3_image/SDL_image.h>
#include <CoreLib/Log.h>

#include "Application.h"
#include "SDLCoreRenderer.h"
#include "SDLCoreError.h"
#include "types/Texture.h"

namespace SDLCore {

    /*
    * @return if null call SDLCore::GetError() for more information
    */
    static SDL_Surface* GenerateFallbackSurface(int width = 32, int height = 32) {
        SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
        if (!surface) {
            SetErrorF("Failed to create fallback surface: {}", SDL_GetError());
            return nullptr;
        }
        const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(surface->format);
        Uint32 colorA = SDL_MapRGBA(fmt, nullptr, 0x00, 0x00, 0x00, 0xFF);
        Uint32 colorB = SDL_MapRGBA(fmt, nullptr, 0xFB, 0x3E, 0xF9, 0xFF);

        Uint32* pixels = static_cast<Uint32*>(surface->pixels);
        int pitch = surface->pitch / sizeof(Uint32);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if ((x < width / 2 && y < height / 2) || (x >= width / 2 && y >= height / 2)) {
                    pixels[y * pitch + x] = colorA;
                }
                else {
                    pixels[y * pitch + x] = colorB;
                }
            }
        }

        return surface;
    }

    Texture::Texture(bool fallbackTexture) {
        m_type = Type::STATIC;
        if (fallbackTexture) {
            LoadFallback();
        }
    }

    Texture::Texture(const char* path, Type type) 
        : m_type(type) {

        if (File::Exists(path)) {
            m_surface = IMG_Load(path);
            if (!m_surface) {
                Log::Error("SDLCore::Texture: Failed to load '{}': {}", path, SDL_GetError());
                return;
            }

            m_width = m_surface->w;
            m_height = m_surface->h;
        }
        else {
            // fallback
            Log::Warn("SDLCore::Texture: File '{}' does not exist, using fallback texture", path);
            LoadFallback();
        }
    }

    Texture::Texture(const std::string& path, Type type)
        : Texture(path.c_str(), type){
    }

    Texture::Texture(const SystemFilePath& path, Type type)
        : Texture(path.string().c_str(), type) {
    }

    Texture::~Texture() {
        if (!Application::IsQuit())
            Cleanup();
    }

    Texture::Texture(Texture&& other) noexcept {
        MoveFrom(std::move(other));
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        MoveFrom(std::move(other));
        return *this;
    }

    bool Texture::CreateForWindow(WindowID winID) {
        if (!m_surface) {
            Log::Warn("SDLCore::Texture::CreateForWindow: Failed to create texture for window '{}', no valid texture available, using fallback texture!", winID);
            LoadFallback();
            return false;
        }

        Window* win = nullptr;
        auto renderer = GetRenderer(winID, win);
        if (!renderer) {
            SetErrorF("Renderer is null for window {}", winID.value);
            return false;
        }
        
        auto existing = m_textures.find(winID);
        if (existing != m_textures.end()) {
            SDL_DestroyTexture(existing->second.tex);
            m_textures.erase(existing);
        }
        
        // adds this texture to an event of the window
        if (win) {
            // remove old callback
            RemoveSDLRendererDestroyCallbackForWindow(winID);
            m_windowSDLRendererDestroyCallbacks[winID] = win->AddOnSDLRendererDestroy([this, winID]() { FreeForWindow(winID); });
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, m_surface);
        if (!tex) {
            SetErrorF("Failed to create texture for window {}: {}", winID.value, SDL_GetError());
            return false;
        }

        m_textures[winID] = SDLTexture(tex);
        return true;
    }

    bool Texture::Render(float x, float y, float w, float h, const FRect* src) {
        WindowID currentWinID = Render::GetActiveWindowID();
        SDLTexture* texture = GetTexture(currentWinID);;
        if (!texture)
            return false;

        if (!texture->tex) {
            SetErrorF("SDLCore::Texture::Render: SDL texture from texture of win '{}' is nullptr!", currentWinID);
            return false;
        }

        // Use original texture size if w/h are unspecified
        if (w <= 0) w = static_cast<float>(m_width);
        if (h <= 0) h = static_cast<float>(m_height);

        SDL_Renderer* renderer = GetRenderer(currentWinID);
        if (!renderer) {
            SetErrorF("SDLCore::Texture::Render: Failed to render texture, renderer of window '{}' is null!", currentWinID);
            return false;
        }

        if (texture->lastR != static_cast<Uint8>(m_colorTint.x) ||
            texture->lastG != static_cast<Uint8>(m_colorTint.y) ||
            texture->lastB != static_cast<Uint8>(m_colorTint.z) ||
            texture->lastA != static_cast<Uint8>(m_colorTint.w)) {

            texture->lastR = static_cast<Uint8>(m_colorTint.x);
            texture->lastG = static_cast<Uint8>(m_colorTint.y);
            texture->lastB = static_cast<Uint8>(m_colorTint.z);
            texture->lastA = static_cast<Uint8>(m_colorTint.w);

            SDL_SetTextureColorMod(texture->tex, texture->lastR, texture->lastG, texture->lastB);
            SDL_SetTextureAlphaMod(texture->tex, texture->lastA);
        }

        SDL_FRect dst{ x, y, w, h };
        SDL_FPoint center{
            dst.x + m_center.x * dst.w,
            dst.y + m_center.y * dst.h
        };

        SDL_FlipMode sdlFlip = static_cast<SDL_FlipMode>(m_flip);
        if (!SDL_RenderTextureRotated(renderer, texture->tex, src, &dst, m_rotation, &center, sdlFlip)) {
            SetErrorF("SDLCore::Texture::Render: Failed to render texture: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    bool Texture::Update(WindowID windowID, const void* pixels, int pitch, Rect* rect) {
        if (m_type != Type::DYNAMIC)
            return false;

        SDLTexture* texture = GetTexture(windowID);
        if (!texture)
            return false;

        if (!texture->tex) {
            SetErrorF("SDLCore::Texture::Update: SDL texture from texture of win '{}' is nullptr!", windowID);
            return false;
        }

        if (!SDL_UpdateTexture(texture->tex, rect, pixels, pitch)) {
            SetErrorF("SDLCore::Texture::Update: Failed to update texture for window {}: {}", windowID, SDL_GetError());
            return false;
        }
        return true;
    }

    void Texture::FreeForWindow(WindowID windowID) {
        auto it = m_textures.find(windowID);
        if (it != m_textures.end()) {
            SDL_DestroyTexture(it->second.tex);
            m_textures.erase(it);
        }

        RemoveSDLRendererDestroyCallbackForWindow(windowID);
    }


    Texture* Texture::SetRotation(float rotation) {
        m_rotation = rotation;
        return this;
    }

    Texture* Texture::SetCenter(const Vector2& center) {
        m_center = center;
        return this;
    }

    Texture* Texture::SetColorTint(float r, float g, float b, float a) {
        m_colorTint.Set(r, g, b, a);
        return this;
    }

    Texture* Texture::SetColorTint(float r, float g, float b) {
        return SetColorTint(r, g, b, 255.0f);
    }

    Texture* Texture::SetColorTint(float bright, float a) {
        return SetColorTint(bright, bright, bright, a);
    }

    Texture* Texture::SetColorTint(float bright) {
        return SetColorTint(bright, bright, bright, 255.0f);
    }

    Texture* Texture::SetColorTint(const Vector3& color) {
        return SetColorTint(color.x, color.y, color.z, 255.0f);
    }

    Texture* Texture::SetColorTint(const Vector4& color) {
        return SetColorTint(color.x, color.y, color.z, color.w);
    }

    Texture* Texture::SetFlip(Flip flip) {
        m_flip = flip;
        return this;
    }

    float Texture::GetRotation() const { 
        return m_rotation; 
    }

    Vector2 Texture::GetCenter() const { 
        return m_center; 
    }

    Vector3 Texture::GetColorTint() const { 
        return Vector3(m_colorTint);
    }

    Texture::Flip Texture::GetFlip() const { 
        return m_flip; 
    }

    SDL_Texture* Texture::GetSDLTexture(WindowID id) {
        SDLTexture* texture = GetTexture(id);
        if (!texture)
            return nullptr;
        return texture->tex;
    }

    SDL_Surface* Texture::GetSDLSurface() const {
        return m_surface;
    }

    Texture* Texture::Reset(TextureParams ignoreMask) {
        if (!(ignoreMask & TextureParams::ROTATION))
            m_rotation = 0.0f;

        if (!(ignoreMask & TextureParams::CENTER))
            m_center = Vector2(0.0f, 0.0f);

        if (!(ignoreMask & TextureParams::COLOR_TINT))
            m_colorTint = Vector4(1.0f);

        if (!(ignoreMask & TextureParams::FLIP))
            m_flip = Flip::NONE;

        if (!(ignoreMask & TextureParams::TYPE))
            m_type = Type::STATIC;

        return this;
    }

    SDL_Renderer* Texture::GetRenderer(WindowID winID) {
        Window* win = nullptr;
        return GetRenderer(winID, win);
    }

    SDL_Renderer* Texture::GetRenderer(WindowID winID, Window*& OutWin) {
        auto app = Application::GetInstance();
        if (!app)
            return nullptr;

        auto win = app->GetWindow(winID);
        OutWin = win;

        if (!win)
            return nullptr;

        auto renderer = win->GetSDLRenderer().lock();
        if (!renderer)
            return nullptr;

        return renderer.get();
    }

    void Texture::Cleanup() {
        for (auto& [_, texture] : m_textures)
            SDL_DestroyTexture(texture.tex);
        m_textures.clear();

        if (m_surface) {
            SDL_DestroySurface(m_surface);
            m_surface = nullptr;
        }

        // Remove all window callbacks safely
        std::vector<WindowID> windowIDs;
        windowIDs.reserve(m_windowSDLRendererDestroyCallbacks.size());
        for (auto& [winID, _] : m_windowSDLRendererDestroyCallbacks)
            windowIDs.push_back(winID);

        for (auto& winID : windowIDs)
            RemoveSDLRendererDestroyCallbackForWindow(winID);

        m_windowSDLRendererDestroyCallbacks.clear();
    }

    Texture::SDLTexture* Texture::GetTexture(WindowID id) {
        auto it = m_textures.find(id);
        if (it == m_textures.end()) {
            if (!CreateForWindow(id)) {
                SetErrorF("SDLCore::Texture::GetTexture: Failed to create texture for active window {}!", id);
                return nullptr;
            }
            it = m_textures.find(id);
            if (it == m_textures.end())
                return nullptr;
        }

        return &it->second;
    }

    void Texture::MoveFrom(Texture&& other) noexcept {
        if (this == &other) return;
        Cleanup();

        m_surface = other.m_surface;
        other.m_surface = nullptr;

        m_textures = std::move(other.m_textures);
        m_width = other.m_width;
        m_height = other.m_height;
        m_type = other.m_type;

        other.m_width = 0;
        other.m_height = 0;
        other.m_textures.clear();
    }

    void Texture::LoadFallback() {
        m_surface = GenerateFallbackSurface();
        m_width = m_surface->w;
        m_height = m_surface->h;
    }

    void Texture::RemoveSDLRendererDestroyCallbackForWindow(WindowID winID) {
        auto itCallback = m_windowSDLRendererDestroyCallbacks.find(winID);
        if (itCallback == m_windowSDLRendererDestroyCallbacks.end())
            return;

        if (auto* app = Application::GetInstance()) {
            if (auto* win = app->GetWindow(winID)) {
                win->RemoveOnSDLRendererDestroy(itCallback->second);
            }
        }

        m_windowSDLRendererDestroyCallbacks.erase(itCallback);
    }

}