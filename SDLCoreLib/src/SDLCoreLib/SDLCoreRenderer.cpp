#include <vector>
#include <cmath>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <CoreLib/Log.h>

#include "SDLCoreTime.h"
#include "Application.h"
#include "types/Vertex.h"
#include "SDLCoreRenderer.h"

namespace SDLCore::Render {

    static std::weak_ptr<SDL_Renderer> s_renderer;
    static WindowID s_winID { SDLCORE_INVALID_ID };

    // ========== Primitives ========== 
    static constexpr bool SET_COLOR = true;
    static SDL_Color s_activeColor { 255, 255, 255, 255 };
    static float s_strokeWidth = 1;
    static bool s_innerStroke = true;

    // ========== Text ==========
    std::shared_ptr<SDLCore::Font> s_font = std::make_shared<SDLCore::Font>(true);// loads the default font
    float s_fontSize = s_font->GetSelectedSize();
    static Align s_textHorAlign = Align::START;
    static Align s_textVerAlign = Align::START;
    float s_textLineHeightMultiplier = 0.2f;

    size_t s_textMaxLines = 0;// < 0 = no limits
    UnitType s_textLimitType = UnitType::NONE;
    size_t s_textMaxLimit = 0;          // max characters or Pixel
    std::string s_textEllipsisDefault = "...";
    std::string s_textEllipsis = s_textEllipsisDefault;
    float s_textClipWidth = -1.0f;
    bool s_textCacheEnabled = false;

    constexpr bool CREATE_ON_NOT_FOUND = true;
    constexpr uint64_t TEXT_CACHE_TTL_FRAMES = 600; // ~10 sec
    struct TextCacheKey {
        const Font* font;
        std::string text;// final text after truncation
        float fontSize;
        float clipWidth;
        size_t maxLines;
        UnitType textLimitType;
        size_t textMaxLimit;
        float lineHeightMultiplier;

        bool operator==(const TextCacheKey& o) const {
            return font == o.font &&
                text == o.text &&
                fontSize == o.fontSize &&
                clipWidth == o.clipWidth &&
                maxLines == o.maxLines &&
                textLimitType == o.textLimitType &&
                textMaxLimit == o.textMaxLimit &&
                lineHeightMultiplier == o.lineHeightMultiplier;
        }
    };

    struct CachedText {
        std::vector<std::string> lines;
        std::vector<float> lineWidths;
        std::vector<float> lineHeightsIgnorBase;
        std::vector<float> lineHeights;

        float blockWidth = 0.0f;
        float blockHeightIgnorBase = 0.0f;
        float blockHeight = 0.0f;
        float textWidth = 0.0f;
        float textHeightIgnorBase = 0.0f;
        float textHeight = 0.0f;

        SDL_Texture* preRenderedTexture = nullptr;

        uint64_t lastUseFrame = 0;
    };

    inline void hashCombine(std::size_t& seed, std::size_t value) noexcept {
        // Combines hashes using a variant of boost::hash_combine
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }

    struct TextCacheKeyHash {
        size_t operator()(const TextCacheKey& k) const noexcept {
            size_t h = 0;

            hashCombine(h, std::hash<const Font*>{}(k.font));
            hashCombine(h, std::hash<std::string>{}(k.text));
            hashCombine(h, std::hash<float>{}(k.fontSize));
            hashCombine(h, std::hash<float>{}(k.clipWidth));
            hashCombine(h, std::hash<size_t>{}(k.maxLines));
            hashCombine(h, std::hash<int>{}(static_cast<int>(k.textLimitType)));
            hashCombine(h, std::hash<size_t>{}(k.textMaxLimit));
            hashCombine(h, std::hash<float>{}(k.lineHeightMultiplier));

            return h;
        }
    };

    std::unordered_map<TextCacheKey, CachedText, TextCacheKeyHash> s_textCache;

    SDL_Renderer* GetActiveRenderer() {
        auto rendererPtr = s_renderer.lock();
        if (!rendererPtr)
            return nullptr;
        return rendererPtr.get();
    }

    WindowID GetActiveWindowID() {
        return s_winID;
    }
    
    static void ConvertVertices(SDL_Vertex* dst,
        const Vertex* src,
        size_t count,
        float xOffset,
        float yOffset,
        float xScale,
        float yScale,
        bool setColor)
    {
        for (size_t i = 0; i < count; i++) {
            SDL_Vertex v = static_cast<SDL_Vertex>(src[i]);
            v.position.x = xOffset + v.position.x * xScale;
            v.position.y = yOffset + v.position.y * yScale;

            if (setColor) {
                v.color = SDL_FColor{
                    s_activeColor.r / 255.0f,
                    s_activeColor.g / 255.0f,
                    s_activeColor.b / 255.0f,
                    s_activeColor.a / 255.0f
                };
            }
            else {
                v.color.r /= 255.0f;
                v.color.g /= 255.0f;
                v.color.b /= 255.0f;
            }
            dst[i] = v;
        }
    }

    static std::shared_ptr<SDL_Renderer> GetActiveRenderer(const char* func) {
        auto rendererPtr = s_renderer.lock();
        if (!rendererPtr) {
            SetErrorF("SDLCore::Renderer::{}: Current renderer is null", func);
        }
        return rendererPtr;
    }

    void SetWindowRenderer(WindowID winID) {
        if (winID.value == SDLCORE_INVALID_ID) {
            s_winID.value = SDLCORE_INVALID_ID;
            s_renderer.reset();
            return;
        }

        s_winID = winID;
        auto app = Application::GetInstance();
        auto win = app->GetWindow(winID);

        if (!win) {
            s_winID.value = SDLCORE_INVALID_ID;
            s_renderer.reset();
            return;
        }

        if (!win->HasRenderer()) {
            Log::Error("SDLCore::Renderer::SetWindowRenderer: Renderer of window '{}' is null or destroyed!", win->GetName());
            s_winID.value = SDLCORE_INVALID_ID;
            s_renderer.reset();
            return;
        }

        std::weak_ptr<SDL_Renderer> rendererWeak = win->GetSDLRenderer();
        if (!rendererWeak.lock()) {
            Log::Error("SDLCore::Renderer::SetWindowRenderer: Renderer of window '{}' is null or destroyed!", win->GetName());
            s_winID.value = SDLCORE_INVALID_ID;
            s_renderer.reset();
            return;
        }

        s_renderer = rendererWeak;
    }

    void Clear() {
        auto renderer = GetActiveRenderer("Clear");
        if (!renderer)
            return;
        if (!SDL_RenderClear(renderer.get())) {
            Log::Error("SDLCore::Renderer::Clear: Failed to clear renderer: {}", SDL_GetError());
        }
    }

    void Present() {
        auto renderer = GetActiveRenderer("Present");
        if (!renderer)
            return;
        if (!SDL_RenderPresent(renderer.get())) {
            Log::Error("SDLCore::Renderer::Present: Failed to Present: {}", SDL_GetError());
        }
    }

    void SetRenderScale(float scaleX, float scaleY) {
        auto renderer = GetActiveRenderer("SetRenderScale");
        if (!renderer)
            return;
        if (!SDL_SetRenderScale(renderer.get(),scaleX, scaleY)) {
            Log::Error("SDLCore::Renderer::SetRenderScale: Failed to SetRenderScale: {}", SDL_GetError());
        }
    }

    void SetRenderScale(float scale) {
        SetRenderScale(scale, scale);
    }

    void SetRenderScale(const Vector2& scale) {
        SetRenderScale(scale.x, scale.y);
    }

    Vector2 GetRenderScale() {
        Vector2 scale{ 0, 0 };
        auto renderer = GetActiveRenderer("GetRenderScale");
        if (!renderer)
            return scale;
        if (!SDL_GetRenderScale(renderer.get(), &scale.x, &scale.y)) {
            Log::Error("SDLCore::Renderer::GetRenderScale: Failed to SetRenderScale: {}", SDL_GetError());
        }
        return scale;
    }

    SDLCore::Rect GetViewport() {
        SDL_Rect viewport{ 0, 0, 0, 0 };
        auto renderer = GetActiveRenderer("GetViewport");
        if (!renderer)
            return viewport;

        if (!SDL_GetRenderViewport(renderer.get(), &viewport)) {
            Log::Error("SDLCore::Renderer::GetViewport: Failed to get viewport: {}", SDL_GetError());
        }
        return viewport;
    }

    void SetViewport(int x, int y, int w, int h) {
        auto renderer = GetActiveRenderer("SetViewport");
        if (!renderer)
            return;

        SDL_Rect viewport{ x, y, w, h };
        if (!SDL_SetRenderViewport(renderer.get(), &viewport)) {
            Log::Error("SDLCore::Renderer::SetViewport: Failed to set viewport ({}, {}, {}, {}): {}",
                x, y, w, h, SDL_GetError());
        }
    }

    void SetViewport(const Vector2& pos, int w, int h) {
        SetViewport(static_cast<int>(pos.x), static_cast<int>(pos.y), w, h);
    }

    void SetViewport(int x, int y, const Vector2& size) {
        SetViewport(x, y, static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void SetViewport(const Vector2& pos, const Vector2& size) {
        SetViewport(static_cast<int>(pos.x), static_cast<int>(pos.y), 
            static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void SetViewport(const Vector4& trans) {
        SetViewport(static_cast<int>(trans.x), static_cast<int>(trans.y),
            static_cast<int>(trans.z), static_cast<int>(trans.w));
    }

    void SetViewport(const SDLCore::Rect& rect) {
        SetViewport(rect.x, rect.y, rect.w, rect.h);
    }

    void ResetViewport() {
        auto renderer = GetActiveRenderer("ResetViewport");
        if (!renderer)
            return;

        if (!SDL_SetRenderViewport(renderer.get(), nullptr)) {
            Log::Error("SDLCore::Renderer::ResetViewport: Failed to reset viewport: {}", SDL_GetError());
        }
    }

    SDLCore::Rect GetClipRect() {
        SDL_Rect clipRect{ 0, 0, 0, 0 };
        auto renderer = GetActiveRenderer("GetClipRect");
        if (!renderer)
            return clipRect;

        if (!SDL_GetRenderClipRect(renderer.get(), &clipRect)) {
            Log::Error("SDLCore::Renderer::GetClipRect: Failed to get clipRect: {}", SDL_GetError());
        }
        return clipRect;
    }

    void SetClipRect(int x, int y, int w, int h) {
        auto renderer = GetActiveRenderer("SetClipRect");
        if (!renderer)
            return;

        SDL_Rect clipRect{ x, y, w, h };
        if (!SDL_SetRenderClipRect(renderer.get(), &clipRect)) {
            Log::Error("SDLCore::Renderer::SetClipRect: Failed to set clipRect ({}, {}, {}, {}): {}",
                x, y, w, h, SDL_GetError());
        }
    }

    void SetClipRect(const Vector2& pos, int w, int h) {
        SetClipRect(static_cast<int>(pos.x), static_cast<int>(pos.y), w, h);
    }

    void SetClipRect(int x, int y, const Vector2& size) {
        SetClipRect(x, y, static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void SetClipRect(const Vector2& pos, const Vector2& size) {
        SetClipRect(static_cast<int>(pos.x), static_cast<int>(pos.y),
            static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void SetClipRect(const Vector4& trans) {
        SetClipRect(static_cast<int>(trans.x), static_cast<int>(trans.y),
            static_cast<int>(trans.z), static_cast<int>(trans.w));
    }

    void SetClipRect(const SDLCore::Rect& rect) {
        SetClipRect(rect.x, rect.y, rect.w, rect.h);
    }

    void ResetClipRect() {
        auto renderer = GetActiveRenderer("ResetClipRect");
        if (!renderer)
            return;

        if (!SDL_SetRenderClipRect(renderer.get(), nullptr)) {
            Log::Error("SDLCore::Renderer::ResetClipRect: Failed to reset clipRect: {}", SDL_GetError());
        }
    }

    void SetBlendMode(bool enabled) {
        auto renderer = GetActiveRenderer("SetBlendMode");
        if (!renderer)
            return;

        SDL_BlendMode mode = enabled ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE;
        if (!SDL_SetRenderDrawBlendMode(renderer.get(), mode)) {
            Log::Error("SDLCore::Renderer::SetBlendMode: Failed to set blend mode {}: {}",
                enabled ? "BLEND" : "NONE", SDL_GetError());
        }
    }

    void SetBlendMode(BlendMode mode) {
        auto renderer = GetActiveRenderer("SetBlendMode");
        if (!renderer)
            return;
        if (!SDL_SetRenderDrawBlendMode(renderer.get(), static_cast<SDL_BlendMode>(mode))) {
            Log::Error("SDLCore::Renderer::SetBlendMode: Failed to set blend mode {}: {}", static_cast<int>(mode), SDL_GetError());
        }
    }

    #pragma region Color

    Vector4 GetActiveColor() {
        return Vector4(s_activeColor.r, s_activeColor.g, s_activeColor.b, s_activeColor.a);
    }

    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        auto renderer = GetActiveRenderer("SetColor");
        if (!renderer)
            return;
        s_activeColor = { r, g, b, a};
        if (!SDL_SetRenderDrawColor(renderer.get(), r, g, b, a)) {
            Log::Error("SDLCore::Renderer::SetColor: Failed to set color ({}, {}, {}, {}): {}", r, g, b, a, SDL_GetError());
        }
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

    void SetColor(const Vector3& rgb, float a) {
        SetColor(static_cast<Uint8>(rgb.x), static_cast<Uint8>(rgb.y), static_cast<Uint8>(rgb.z), static_cast<Uint8>(a));
    }

    void SetColor(const Vector3& rgb) {
        SetColor(static_cast<Uint8>(rgb.x), static_cast<Uint8>(rgb.y), static_cast<Uint8>(rgb.z), 255);
    }

#pragma endregion

    void SetStrokeWidth(float width) {
        if (width <= 1)
            width = 1;
        s_strokeWidth = width;
    }

    void SetInnerStroke(bool value) {
        s_innerStroke = value;
    }

    #pragma region Primitives

    #pragma region Rectangle

    void FillRect(float x, float y, float w, float h) {
        auto renderer = GetActiveRenderer("FillRect");
        if (!renderer)
            return;
        SDL_FRect rect{ x, y, w, h };
        if (!SDL_RenderFillRect(renderer.get(), &rect)) {
            Log::Error("SDLCore::Renderer::FillRect: Failed to fill rect ({}, {}, {}, {}): {}", x, y, w, h, SDL_GetError());
        }
    }

    void FillRect(const Vector2& pos, float w, float h) {
        FillRect(pos.x, pos.y, w, h);
    }

    void FillRect(float x, float y, const Vector2& size) {
        FillRect(x, y, size.x, size.y);
    }

    void FillRect(const Vector2& pos, const Vector2& size) {
        FillRect(pos.x, pos.y, size.x, size.y);
    }

    void FillRect(const Vector4& trans) {
        FillRect(trans.x, trans.y, trans.z, trans.w);
    }

    void FillRects(const Vector4* transforms, size_t count) {
        auto renderer = GetActiveRenderer("FillRects");
        if (!renderer || count == 0)
            return;

        std::vector<SDL_FRect> rects(count);
        for (size_t i = 0; i < count; i++) {
            const Vector4& trans = transforms[i];
            rects[i] = SDL_FRect{ trans.x, trans.y, trans.z, trans.w };
        }

        if (!rects.empty()) {
            if (!SDL_RenderFillRects(renderer.get(), rects.data(), static_cast<int>(rects.size()))) {
                Log::Error("SDLCore::Renderer::FillRects: Failed to fill rects count '{}', {}", count, SDL_GetError());
            }
        }
    }

    void FillRects(std::vector<Vector4>& transforms) {
        FillRects(transforms.data(), transforms.size());
    }

    void Rect(float x, float y, float w, float h) {
        auto renderer = GetActiveRenderer("Rect");
        if (!renderer)
            return;

        if (s_strokeWidth == 1) {
            SDL_FRect rect{ x, y, w, h };
            if (!SDL_RenderRect(renderer.get(), &rect)) {
                Log::Error("SDLCore::Renderer::Rect: Failed to draw rect ({}, {}, {}, {}): {}",
                    rect.x, rect.y, rect.w, rect.h, SDL_GetError());
            }
            return;
        }

        std::vector<SDL_FRect> rects(4);
        if (s_innerStroke) {
            float sX = (w < s_strokeWidth) ? w : s_strokeWidth;
            float sY = (h < s_strokeWidth) ? h : s_strokeWidth;

            rects[0] = { x, y, w, sY };          // top
            rects[1] = { x, y + h - sY, w, sY };  // bottom
            rects[2] = { x, y, sX, h };          // left
            rects[3] = { x + w - sX, y, sX, h };  // right
        }
        else {
            float s = s_strokeWidth;

            rects[0] = { x - s, y - s, w + s * 2, s };  // top
            rects[1] = { x - s, y + h, w + s * 2, s };  // bottom
            rects[2] = { x - s, y - s, s, h + s * 2 };  // left
            rects[3] = { x + w, y - s, s, h + s * 2 };  // right
        }

        for (auto& rect : rects) {
            if (!SDL_RenderFillRect(renderer.get(), &rect)) {
                Log::Error("SDLCore::Renderer::Rect: Failed to draw rect ({}, {}, {}, {}): {}",
                    rect.x, rect.y, rect.w, rect.h, SDL_GetError());
                break;
            }
        }
    }

    void Rect(const Vector2& pos, float w, float h) {
        Rect(pos.x, pos.y, w, h);
    }

    void Rect(float x, float y, const Vector2& size) {
        Rect(x, y, size.x, size.y);
    }

    void Rect(const Vector2& pos, const Vector2& size) {
        Rect(pos.x, pos.y, size.x, size.y);
    }

    void Rect(const Vector4& trans) {
        Rect(trans.x, trans.y, trans.z, trans.w);
    }

    void Rects(const Vector4* transforms, size_t count) {
        auto renderer = GetActiveRenderer("Rects");
        if (!renderer || count == 0)
            return;

        float s = s_strokeWidth;
        std::vector<SDL_FRect> rects;
        rects.reserve((s_strokeWidth != 1) ? count * 4 : count);

        for (size_t i = 0; i < count; i++) {
            const Vector4& t = transforms[i];

            if (s_strokeWidth == 1) {
                rects.push_back({ t.x, t.y, t.z, t.w });
            }
            else {
                if (s_innerStroke) {
                    float sX = (t.z < s_strokeWidth) ? t.z : s_strokeWidth;
                    float sY = (t.w < s_strokeWidth) ? t.w : s_strokeWidth;

                    rects.push_back({ t.x, t.y, t.z, sY }); ;// top
                    rects.push_back({ t.x, t.y + t.w - sY, t.z, sY });// bottom
                    rects.push_back({ t.x, t.y, sX, t.w }); ;// left
                    rects.push_back({ t.x + t.z - sX, t.y, sX, t.w });// right
                }
                else {
                    rects.push_back({ t.x - s, t.y - s, t.z + s * 2, s });    // top
                    rects.push_back({ t.x - s, t.y + t.w, t.z + s * 2, s });  // bottom
                    rects.push_back({ t.x - s, t.y - s, s, t.w + s * 2 });    // left
                    rects.push_back({ t.x + t.z, t.y - s, s, t.w + s * 2 });  // right
                }
            }
        }

        if (!rects.empty()) {
            bool result = true;
            if (s_strokeWidth == 1)
                result = SDL_RenderRects(renderer.get(), rects.data(), static_cast<int>(rects.size()));
            else
                result = SDL_RenderFillRects(renderer.get(), rects.data(), static_cast<int>(rects.size()));

            if (!result)
                Log::Error("SDLCore::Renderer::Rects: Failed to draw rects count '{}', {}", count, SDL_GetError());
        }
    }

    void Rects(const std::vector<Vector4>& transforms) {
        Rects(transforms.data(), transforms.size());
    }

#pragma endregion

#pragma region Line

    void Line(float x1, float y1, float x2, float y2) {
        auto renderer = GetActiveRenderer("Line");
        if (!renderer)
            return;

        if (s_strokeWidth <= 1) {
            SDL_RenderLine(renderer.get(), x1, y1, x2, y2);
            return;
        }

        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        if (length == 0.0f)
            return;

        dx /= length;
        dy /= length;

        float nx = -dy;
        float ny = dx;

        float halfStroke = s_strokeWidth / 2.0f;

        SDL_FPoint verts[4] = {
            { x1 + nx * halfStroke, y1 + ny * halfStroke },
            { x2 + nx * halfStroke, y2 + ny * halfStroke },
            { x2 - nx * halfStroke, y2 - ny * halfStroke },
            { x1 - nx * halfStroke, y1 - ny * halfStroke }
        };
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer.get(), &r, &g, &b, &a);
        SDL_Vertex quad[4];
        for (int i = 0; i < 4; ++i) {
            quad[i].position.x = verts[i].x;
            quad[i].position.y = verts[i].y;
            quad[i].color = SDL_FColor{ static_cast<float>(r) / 255, 
                static_cast<float>(g) / 255, 
                static_cast<float>(b) / 255, 
                static_cast<float>(a) / 255 };
        }

        int indices[6] = { 0, 1, 2, 2, 3, 0 };
        if (!SDL_RenderGeometry(renderer.get(), nullptr, quad, 4, indices, 6)) {
            Log::Error("SDLCore::Renderer::Line: Failed to draw thick line ({}, {}, {}, {}): {}", x1, y1, x2, y2, SDL_GetError());
        }
    }

    void Line(const Vector2& p1, float x2, float y2) {
        Line(p1.x, p1.y, x2, y2);
    }

    void Line(float x1, float y1, const Vector2& p2) {
        Line(x1, y1, p2.x, p2.y);
    }

    void Line(const Vector2& p1, const Vector2& p2) {
        Line(p1.x, p1.y, p2.x, p2.y);
    }

    void Line(const Vector4& poins) {
        Line(poins.x, poins.y, poins.z, poins.w);
    }

#pragma endregion

    void Point(float x, float y) {
        auto renderer = GetActiveRenderer("Point");
        if (!renderer)
            return;

        if (!SDL_RenderPoint(renderer.get(), x, y)) {
            Log::Error("SDLCore::Renderer::Point: Failed to draw point ({}, {}): {}", x, y, SDL_GetError());
        }
    }

    bool Polygon(const Vertex* vertices,
        size_t vertexCount,
        SDLCore::Texture* texture,
        const int* indices,
        size_t indexCount,
        float xOffset,
        float yOffset,
        float scaleX,
        float scaleY)
    {
        auto renderer = GetActiveRenderer("Polygon");
        if (!renderer)
            return false;

        if (!vertices || vertexCount == 0)
            return true;

        // Local stack buffer for small meshes (no heap allocation)
        constexpr size_t STACK_LIMIT = 64;
        SDL_Vertex stackBuffer[STACK_LIMIT];
        SDL_Vertex* out = stackBuffer;

        std::vector<SDL_Vertex> heapVertices;
        if (vertexCount > STACK_LIMIT) {
            heapVertices.resize(vertexCount);
            out = heapVertices.data();
        }

        ConvertVertices(out,
            vertices,
            vertexCount,
            xOffset,
            yOffset,
            scaleX,
            scaleY,
            /*setColor*/ texture == nullptr);

        SDL_Texture* tex = nullptr;
        if (texture) {
            tex = texture->GetSDLTexture(s_winID);
        }

        // Index pointer only if valid
        const int* idx = (indexCount > 0) ? indices : nullptr;

        // Submit geometry to SDL
        if (!SDL_RenderGeometry(renderer.get(),
            tex,
            out,
            static_cast<int>(vertexCount),
            idx,
            static_cast<int>(indexCount)))
        {
            if (idx) {
                SetErrorF("SDLCore::Renderer::Polygon: Failed to draw polygon (vertices={}, indices={}): {}",
                    vertexCount, indexCount, SDL_GetError());
            }
            else {
                SetErrorF("SDLCore::Renderer::Polygon: Failed to draw polygon (vertices={}): {}",
                    vertexCount, SDL_GetError());
            }
            return false;
        }

        return true;
    }

    #pragma endregion

    #pragma region Texture

    void Texture(SDLCore::Texture& texture, float x, float y, float w, float h, const FRect* src) {
        texture.Render(x, y, w, h, src);
    }

    void Texture(SDLCore::Texture& texture, const Vector2& pos, float w, float h, const FRect* src) {
        texture.Render(pos.x, pos.y, w, h, src);
    }

    void Texture(SDLCore::Texture& texture, float x, float y, const Vector2& size, const FRect* src) {
        texture.Render(x, y, size.x, size.y, src);
    }

    void Texture(SDLCore::Texture& texture, const Vector2& pos, const Vector2& size, const FRect* src) {
        texture.Render(pos.x, pos.y, size.x, size.y, src);
    }

    void Texture(SDLCore::Texture& texture, const Vector4& transform, const FRect* src) {
        texture.Render(transform.x, transform.y, transform.z, transform.w, src);
    }

    void Texture(SDLCore::Texture& texture, const FRect& transform, const FRect* src) {
        texture.Render(transform.x, transform.y, transform.w, transform.h, src);
    }
    
    #pragma endregion

    #pragma region Text

    // Helper function to calculate the horizontal offset of the text
    static inline float CalculateHorOffset(const std::string& text, Align align) {
        switch (align) {
        case SDLCore::Align::START:     return 0;
        case SDLCore::Align::CENTER:    return GetTextWidth(text) * 0.5f;
        case SDLCore::Align::END:       return GetTextWidth(text);
        default:                                return 0;
        }
    }

    // Helper function to calculate the vertical offset of the text
    static inline float CalculateVerOffset(const std::vector<std::string>& lines, Align align) {
        switch (align) {
        case SDLCore::Align::START:     return 0;
        case SDLCore::Align::CENTER:    return GetTextBlockHeight(lines, true) * 0.5f;
        case SDLCore::Align::END:       return GetTextBlockHeight(lines, true);
        default:                                return 0;
        }
    }

    static inline float CalcOffsetCached(float blockSize, Align align) {
        switch (align) {
        case Align::START:  return 0.0f;
        case Align::CENTER: return blockSize * 0.5f;
        case Align::END:    return blockSize;
        default:            return 0.0f;
        }
    }

    static inline std::vector<std::string> BuildLines(const std::string& text) {
        std::vector<std::string> lines;
        lines.reserve(text.size() / 10);

        if (s_textClipWidth == -1) {
            std::istringstream stream(text);
            std::string line;
            line.reserve(32);

            while (std::getline(stream, line)) {
                lines.push_back(line);
                if (s_textMaxLines != 0 && lines.size() >= s_textMaxLines)
                    break;
            }

            return lines;
        }

        if (!s_font)
            return lines;

        auto* asset = s_font->GetFontAsset();
        if (!asset)
            return lines;

        std::string currentLine;
        float currentLineWidth = 0.0f;

        std::string currentWord;
        float currentWordWidth = 0.0f;

        currentLine.reserve(32);
        currentWord.reserve(16);

        auto flushLine = [&]() {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
                currentLineWidth = 0.0f;
            }
        };

        auto flushWord = [&]() {
            if (currentWord.empty())
                return;

            if (currentLineWidth + currentWordWidth <= s_textClipWidth) {
                currentLine += currentWord;
                currentLineWidth += currentWordWidth;
            }
            else {
                flushLine();

                if (currentWordWidth > s_textClipWidth) {
                    for (char c : currentWord) {
                        auto* m = asset->GetGlyphMetrics(c);
                        if (!m)
                            continue;

                        float cw = static_cast<float>(m->advance);
                        if (currentLineWidth + cw > s_textClipWidth)
                            flushLine();

                        currentLine += c;
                        currentLineWidth += cw;
                    }
                }
                else {
                    currentLine = currentWord;
                    currentLineWidth = currentWordWidth;
                }
            }

            currentWord.clear();
            currentWordWidth = 0.0f;
        };

        for (char c : text) {
            if (c == '\n') {
                flushWord();
                flushLine();
                continue;
            }

            auto* m = asset->GetGlyphMetrics(c);
            if (!m)
                continue;

            float charWidth = static_cast<float>(m->advance);

            if (c == ' ' || c == '\t') {
                currentWord += c;
                currentWordWidth += charWidth;
                flushWord();
                continue;
            }

            currentWord += c;
            currentWordWidth += charWidth;
        }

        flushWord();
        flushLine();

        if (s_textMaxLines != 0 && lines.size() > s_textMaxLines)
            lines.resize(s_textMaxLines);

        return lines;
    }

    // text musst be in finale version. Truncated applyed, ...
    CachedText* GetCachedText(const std::string& text, bool createOnNotFound = false) {
        TextCacheKey key{
            s_font.get(),
            text,
            s_fontSize,
            s_textClipWidth,
            s_textMaxLines,
            s_textLimitType,
            s_textMaxLimit,
            s_textLineHeightMultiplier
        };

        auto it = s_textCache.find(key);
        if (it == s_textCache.end() && !createOnNotFound)
            return nullptr;

        auto [it2, inserted] = s_textCache.try_emplace(key);
        CachedText& ct = it2->second;

        if (inserted || ct.lines.empty()) {
            ct.lines = BuildLines(text);

            ct.lineWidths.clear();
            ct.lineHeights.clear();
            ct.lineHeightsIgnorBase.clear();

            ct.lineWidths.reserve(ct.lines.size());
            ct.lineHeights.reserve(ct.lines.size());
            ct.lineHeightsIgnorBase.reserve(ct.lines.size());

            for (const auto& line : ct.lines) {
                ct.lineWidths.push_back(GetTextWidth(line));
                ct.lineHeightsIgnorBase.push_back(GetLineHeight(line, true));
                ct.lineHeights.push_back(GetLineHeight(line, false));
            }

            ct.blockWidth = GetTextBlockWidth(ct.lines);
            ct.blockHeightIgnorBase = GetTextBlockHeight(ct.lines, true);
            ct.blockHeight = GetTextBlockHeight(ct.lines, false);
            ct.textWidth = GetTextWidth(text);
            ct.textHeightIgnorBase = GetTextHeight(text, true);
            ct.textHeight = GetTextHeight(text, false);

            if (ct.preRenderedTexture) {
                SDL_DestroyTexture(ct.preRenderedTexture);
                ct.preRenderedTexture = nullptr;
            }

            auto renderer = GetActiveRenderer("PreRenderText");
            if (renderer) {
                ct.preRenderedTexture = SDL_CreateTexture(
                    renderer.get(),
                    SDL_PIXELFORMAT_RGBA8888,
                    SDL_TEXTUREACCESS_TARGET,
                    static_cast<int>(ct.blockWidth),
                    static_cast<int>(ct.blockHeight)
                );

                if (ct.preRenderedTexture) {
                    // Save old render target
                    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer.get());
                    SDL_SetRenderTarget(renderer.get(), ct.preRenderedTexture);
                    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
                    SDL_RenderClear(renderer.get());

                    float penY = 0.0f;
                    for (size_t i = 0; i < ct.lines.size(); ++i) {
                        float lineWidth = GetTextWidth(ct.lines[i]);
                        float penX = 0.0f;

                        switch (s_textHorAlign) {
                        case Align::START:  penX = 0.0f; break;
                        case Align::CENTER: penX = (ct.blockWidth - lineWidth) * 0.5f; break;
                        case Align::END:    penX = ct.blockWidth - lineWidth; break;
                        default:            penX = 0.0f; break;
                        }

                        for (char c : ct.lines[i]) {
                            auto* m = s_font->GetFontAsset()->GetGlyphMetrics(c);
                            if (!m) continue;

                            SDL_FRect dst{
                                penX,
                                penY,
                                static_cast<float>(m->atlasWidth),
                                static_cast<float>(m->atlasHeight)
                            };

                            SDL_FRect src{
                                static_cast<float>(m->atlasX),
                                static_cast<float>(m->atlasY),
                                static_cast<float>(m->atlasWidth),
                                static_cast<float>(m->atlasHeight)
                            };

                            SDL_RenderTexture(renderer.get(), s_font->GetFontAsset()->GetGlyphAtlasTexture(s_winID), &src, &dst);
                            penX += m->advance;
                        }

                        penY += ct.lineHeights[i];
                    }

                    // Restore old render target
                    SDL_SetRenderTarget(renderer.get(), oldTarget);
                }
            }
        }

        ct.lastUseFrame = Time::GetFrameCount();
        return &ct;
    }

    void RenderCachedText(const std::string& text, float x, float y) {
        auto renderer = GetActiveRenderer("RenderCachedText");
        if (!renderer) return;

        std::string currentText = (s_textMaxLimit != 0 && s_textLimitType != UnitType::NONE)
            ? GetTruncatedText(text)
            : text;

        CachedText* ct = GetCachedText(currentText, CREATE_ON_NOT_FOUND);
        if (!ct || !ct->preRenderedTexture)
            return;

        SDL_FRect dst{
            x - CalcOffsetCached(ct->blockWidth, s_textHorAlign),
            y - CalcOffsetCached(ct->blockHeightIgnorBase, s_textVerAlign),
            ct->blockWidth,
            ct->blockHeight
        };

        SDL_SetTextureColorMod(ct->preRenderedTexture, s_activeColor.r, s_activeColor.g, s_activeColor.b);
        SDL_SetTextureAlphaMod(ct->preRenderedTexture, s_activeColor.a);

        SDL_RenderTexture(renderer.get(), ct->preRenderedTexture, nullptr, &dst);
    }

    static inline void EvictOldTextCache(uint64_t currentFrame) {
        for (auto it = s_textCache.begin(); it != s_textCache.end(); ) {
            if (currentFrame - it->second.lastUseFrame > TEXT_CACHE_TTL_FRAMES) {
                if (it->second.preRenderedTexture) {
                    SDL_DestroyTexture(it->second.preRenderedTexture);
                    it->second.preRenderedTexture = nullptr;
                }
                it = s_textCache.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void Text(const std::string& text, float x, float y) {
        EvictOldTextCache(Time::GetFrameCount());

        std::string finalText = (s_textMaxLimit != 0 && s_textLimitType != UnitType::NONE)
            ? GetTruncatedText(text)
            : text;

        if (s_textCacheEnabled) {
            RenderCachedText(finalText, x, y);
            s_textCacheEnabled = false;
            return;
        }

        auto renderer = GetActiveRenderer("Text");
        if (!renderer)
            return;

        if (!s_font) {
            Log::Error("SDLCore::Renderer::Text: Faild to render text for text'{}', no font was set", text);
            return;
        }
        
        auto* asset = s_font->GetFontAsset();
        if (!asset)
            return;

        SDL_Texture* atlas = asset->GetGlyphAtlasTexture(s_winID);
        if (!atlas)
            return;

        SDL_SetTextureColorMod(atlas, s_activeColor.r, s_activeColor.g, s_activeColor.b);
        SDL_SetTextureAlphaMod(atlas, s_activeColor.a);

        std::vector<std::string> lines = BuildLines(finalText);
        if (lines.empty())
            return;

        float blockOffsetY = CalculateVerOffset(lines, s_textVerAlign);

        float penY = y;
        size_t currentLine = 0;

        for (const auto& line : lines) {
            if (s_textMaxLines != 0 && currentLine >= s_textMaxLines)
                break;

            float blockOffsetX = CalculateHorOffset(line, s_textHorAlign);
            float lineH = GetLineHeight(line, true);
            float penX = x;

            for (char c : line) {
                auto* m = asset->GetGlyphMetrics(c);
                if (!m)
                    continue;

                SDL_FRect dst{
                    penX - blockOffsetX,
                    penY - blockOffsetY,
                    static_cast<float>(m->atlasWidth),
                    static_cast<float>(m->atlasHeight)
                };

                SDL_FRect src{
                    static_cast<float>(m->atlasX),
                    static_cast<float>(m->atlasY),
                    static_cast<float>(m->atlasWidth),
                    static_cast<float>(m->atlasHeight)
                };

                SDL_RenderTexture(renderer.get(), atlas, &src, &dst);
                penX += m->advance;
            }

            penY += lineH;
            currentLine++;
        }
    }

    void Text(const std::string& text, const Vector2& pos) {
        Text(text, pos.x, pos.y);
    }

    void CachText(bool value) {
        s_textCacheEnabled = value;
    }

    void ClearTextCache() {
        for (auto& [key, ct] : s_textCache) {
            if (ct.preRenderedTexture) {
                SDL_DestroyTexture(ct.preRenderedTexture);
                ct.preRenderedTexture = nullptr;
            }
        }
        s_textCache.clear();
    }

    void ClearTextCache(const Font* font) {
        for (auto it = s_textCache.begin(); it != s_textCache.end(); ) {
            if (it->first.font == font) {
                if (it->second.preRenderedTexture) {
                    SDL_DestroyTexture(it->second.preRenderedTexture);
                    it->second.preRenderedTexture = nullptr;
                }
                it = s_textCache.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    size_t GetNumberOfCachedTexts() {
        return s_textCache.size();
    }

    void ResetTextParams() {
        SetFontSize(16.0f);
        SetTextAlign(Align::START);
        SetTextEllipsis(s_textEllipsisDefault);
        SetMaxLines(0);
        SetTextLimit(0, UnitType::NONE);
        SetTextClipWidth(-1);
    }

    void SetFont(std::shared_ptr<Font> font) {
        s_font = font;
        s_font->SelectSize(s_fontSize);
    }

    void SetFont(const SystemFilePath& path) {
        s_font = std::make_shared<Font>(path);
        s_font->SelectSize(s_fontSize);
    }

    void SetFontSize(float size) {
        s_fontSize = std::max(size, 0.0f);
        if (!s_font)
            s_font = std::make_shared<SDLCore::Font>(true);// loads the default font
        s_font->SelectSize(s_fontSize);
    }

    float GetActiveFontSize() {
        return (s_font) ? s_font->GetSelectedSize() : s_fontSize;
    }

    std::shared_ptr<Font> GetActiveFont() {
        return s_font;
    }

    void SetTextAlignHor(Align hor) {
        s_textHorAlign = hor;
    }

    void SetTextAlignVer(Align ver) {
        s_textVerAlign = ver;
    }

    void SetTextAlign(Align hor, Align ver) {
        SetTextAlignHor(hor);
        SetTextAlignVer(ver);
    }

    void SetTextAlign(Align align) {
        SetTextAlignHor(align);
        SetTextAlignVer(align);
    }

    Align GetTextAlignHor() {
        return s_textHorAlign;
    }

    Align GetTextAlignVer() {
        return s_textVerAlign;
    }

    void SetLineHeightMultiplier(float multiplier) {
        s_textLineHeightMultiplier = multiplier;
    }

    float GetLineHeightMultiplier() {
        return s_textLineHeightMultiplier; 
    }

    void SetMaxLines(size_t lines) {
        s_textMaxLines = lines;
    }

    size_t GetMaxLines() {
        return s_textMaxLines;
    }

    void SetTextEllipsis(const std::string& ellipsis) {
        s_textEllipsis = ellipsis;
    }

    std::string GetEllipsis() {
        return s_textEllipsis;
    }

    void SetTextLimit(size_t amount, UnitType type) {
        s_textMaxLimit = amount;
        s_textLimitType = type;
    }

    size_t GetTextLimitAmount() {
        return s_textMaxLimit;
    }

    UnitType GetTextLimitType() {
        return s_textLimitType;
    }

    std::string GetTruncatedText(const std::string& text) {
        if (s_textMaxLimit == 0)
            return text;

        switch (s_textLimitType) {
        case SDLCore::UnitType::CHARACTERS: {
            if (text.size() <= s_textMaxLimit)
                return text;
            return text.substr(0, s_textMaxLimit) + s_textEllipsis;
        }

        case SDLCore::UnitType::PIXELS: {
            if (!s_font) {
                Log::Error("SDLCore::Renderer::GetTruncatedText: Failed to get truncated text for '{}', no font set", text);
                return text;
            }

            auto* asset = s_font->GetFontAsset();
            if (!asset)
                return text;

            float width = 0.0f;
            std::string result;

            for (char c : text) {
                //add line break chars
                if (c == '\n')
                    result += c;

                auto* metric = asset->GetGlyphMetrics(c);
                if (!metric)
                    continue;

                float charWidth = static_cast<float>(metric->advance);
                if (width + charWidth > s_textMaxLimit) {
                    if (!result.empty())
                        result += s_textEllipsis;
                    return result;
                }

                result += c;
                width += charWidth;
            }

            return result;
        }

        case SDLCore::UnitType::NONE:
        default:
            return text;
        }
    }

    void SetTextClipWidth(float w) {
        s_textClipWidth = w;
    }

    float GetTextClipWidth() {
        return s_textClipWidth; 
    }
    
    void ResetTextClipWidth() {
        s_textClipWidth = -1.0f;
    }
    
    float CalculateFontSizeForBounds(const std::string& text, float targetW, float targetH) {
        if (text.empty() || targetW <= 1.0f || targetH <= 1.0f)
            return 1.0f;

        float baseSize = (s_fontSize > 0.0f) ? s_fontSize : 16.0f;

        float baseW = GetTextBlockWidth(text);
        float baseH = GetTextBlockHeight(text, true);

        if (baseW <= 0.0f || baseH <= 0.0f)
            return baseSize;

        float scale = std::min(targetW / baseW, targetH / baseH);
        return baseSize * scale;
    }

    float CalculateFontSizeForBounds(const std::string& text, const Vector2& targetSize) {
        return CalculateFontSizeForBounds(text, targetSize.x, targetSize.x);
    }

    float GetCharWidth(char c) {
        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetCharWidth: Faild to get char width for char'{}', no font was set", c);
            return 0.0f;
        }

        auto* asset = s_font->GetFontAsset();
        if (!asset)
            return 0.0f;

        auto* m = asset->GetGlyphMetrics(c);
        return (m) ? m->advance : 0.0f;
    }

    float GetCharHeight(char c, bool ignoreBelowBaseline) {
        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetCharHeight: Faild to get char height for char'{}', no font was set", c);
            return 0.0f;
        }

        auto* asset = s_font->GetFontAsset();
        if (!asset)
            return 0.0f;

        auto* metric = asset->GetGlyphMetrics(c);
        if (!metric)
            return 0.0f;

        int result = (ignoreBelowBaseline) ? metric->AscenderHeight() : metric->MetricsHeight();
        return static_cast<float>(result);
    }

    float GetTextWidth(const std::string& text) {
        if (auto* ct = GetCachedText(text, false)) {
            return ct->textWidth; // use cached width
        }

        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetTextWidth: Failed to get text width for text '{}', no font was set", text);
            return 0.0f;
        }

        float width = 0.0f;
        auto* asset = s_font->GetFontAsset();
        if (!asset) 
            return 0.0f;

        for (char c : text) {
            if (auto* m = asset->GetGlyphMetrics(c))
                width += m->advance;
        }
        return width;
    }

    float GetTextHeight(const std::string& text, bool ignoreBelowBaseline) {
        if (auto* ct = GetCachedText(text, false)) {
            return ignoreBelowBaseline ? ct->textHeightIgnorBase : ct->textHeight;
        }

        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetTextHeight: Faild to get text height for text'{}', no font was set", text);
            return 0.0f;
        }

        auto* asset = s_font->GetFontAsset();
        if (!asset) 
            return 0.0f;

        int maxAscender = 0;
        int minDescender = 0;

        for (char c : text) {
            if (auto* m = asset->GetGlyphMetrics(c)) {
                maxAscender = std::max(maxAscender, m->AscenderHeight());
                minDescender = std::min(minDescender, m->DescenderHeight());
            }
        }

        return ignoreBelowBaseline ? static_cast<float>(maxAscender)
            : static_cast<float>(maxAscender - minDescender);
    }

    float GetTextBlockWidth(const std::string& text) {
        if (auto* ct = GetCachedText(text, false)) {
            return ct->blockWidth; // cached block width
        }

        auto lines = BuildLines(text);
        return GetTextBlockWidth(lines);
    }

    float GetTextBlockWidth(const std::vector<std::string>& lines) {
        if (auto* ct = GetCachedText(lines.empty() ? "" : lines[0], false)) {
            return ct->blockWidth; // approximate cache, fallback if lines are from a single text
        }

        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetTextBlockWidth: Faild to get block width for lines'{}', no font was set", lines);
            return 0.0f;
        }

        float maxWidth = 0.0f;
        for (const auto& line : lines)
            maxWidth = std::max(maxWidth, GetTextWidth(line));
        return maxWidth;
    }

    float GetTextBlockHeight(const std::string& text, bool ignoreBelowBaseline) {
        if (auto* ct = GetCachedText(text, false)) {
            return ignoreBelowBaseline ? ct->blockHeightIgnorBase : ct->blockHeight;
        }

        auto lines = BuildLines(text);
        return GetTextBlockHeight(lines, ignoreBelowBaseline);
    }

    float GetTextBlockHeight(const std::vector<std::string>& lines, bool ignoreBelowBaseline) {
        if (auto* ct = GetCachedText(lines.empty() ? "" : lines[0], false)) {
            return ignoreBelowBaseline ? ct->blockHeightIgnorBase : ct->blockHeight;
        }

        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetTextBlockHeight: Faild to get block height for lines'{}', no font was set", lines);
            return 0.0f;
        }

        if (lines.empty()) 
            return 0.0f;

        float totalHeight = 0.0f;
        for (const auto& line : lines)
            totalHeight += GetLineHeight(line, ignoreBelowBaseline);

        totalHeight -= s_textLineHeightMultiplier * s_fontSize;
        return totalHeight;
    }

    float GetLineHeight(const std::string& line, bool ignoreBelowBaseline) {
        if (auto* ct = GetCachedText(line, false)) {
            if (ignoreBelowBaseline) {
                return ct->lineHeightsIgnorBase.empty() ? 0.0f :
                    *std::max_element(ct->lineHeightsIgnorBase.begin(), ct->lineHeightsIgnorBase.end());
            }
            else {
                return ct->lineHeights.empty() ? 0.0f :
                    *std::max_element(ct->lineHeights.begin(), ct->lineHeights.end());
            }
        }

        if (!s_font) {
            Log::Error("SDLCore::Renderer::GetLineHeight: Failed to get line height for line '{}', no font was set", line);
            return 0.0f;
        }

        auto* asset = s_font->GetFontAsset();
        if (!asset)
            return 0.0f;

        int maxH = 0;
        for (char c : line) {
            if (auto* m = asset->GetGlyphMetrics(c)) {
                maxH = std::max(maxH, ignoreBelowBaseline ? m->AscenderHeight() : m->MetricsHeight());
            }
        }
        return static_cast<float>(maxH) + s_textLineHeightMultiplier * s_fontSize;
    }

    #pragma endregion

}