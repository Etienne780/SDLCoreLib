#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <CoreLib/Log.h>

#include "Application.h"
#include "Types/Font/FontAsset.h"

namespace SDLCore {

	FontAsset::FontAsset(TTF_Font* font, float size)
		: m_ttfFont(font), m_fontSize(size) {
        if (font) {
            m_ascent = TTF_GetFontAscent(font);
            m_descent = TTF_GetFontDescent(font);
            m_lineSkip = TTF_GetFontLineSkip(font);
            m_glyphAtlasSurf = GenerateGlypeAtlas(font, size);
            if (!m_glyphAtlasSurf) {
                Log::Error("SDLCore::FontAsset: Font asset of font '{}' with size '{}' can not be used, glyph atlas was not generated",
                    TTF_GetFontFamilyName(font), size);
            }
        }
        else {
            Log::Error("SDLCore::FontAsset:  Font asset with size '{}' can not be used, the given Font is a nullptr", size);
        }
	}

	FontAsset::~FontAsset() {
        if(!Application::IsQuit())
		    Cleanup();
	};

	FontAsset::FontAsset(FontAsset&& other) noexcept {
		MoveFrom(std::move(other));
	}

	FontAsset& FontAsset::operator=(FontAsset&& other) noexcept {
		MoveFrom(std::move(other));
		return *this;
	}

    GlyphMetrics* FontAsset::GetGlyphMetrics(uint32_t code) {
        if (code < 256 && m_asciiPresent[code])
            return &m_asciiGlyphs[code];

        auto it = m_charToGlyphMetrics.find(code);
        return (it == m_charToGlyphMetrics.end()) ? nullptr : &it->second;
    }

    SDL_Texture* FontAsset::GetGlyphAtlasTexture(WindowID winID) {
        auto it = m_winIDToGlyphAtlasTexture.find(winID);
        if (it != m_winIDToGlyphAtlasTexture.end())
            return it->second;

        SDL_Texture* tex = CreateTextureForWindow(winID);
        if (!tex) {
            auto* app = Application::GetInstance();
            if (app) {
                auto* win = app->GetWindow(winID);
                if (win) {
                    Log::Error("SDLCore::FontAsset::GetGlyphAtlasTexture: Could not create texture for window '{} ({})', Font: '{}' size: '{}'", 
                        win->GetName(), winID, TTF_GetFontFamilyName(m_ttfFont), m_fontSize);
                    return tex;
                }
            }

            Log::Error("SDLCore::FontAsset::GetGlyphAtlasTexture: Could not create texture for window '{}', Font: '{}' size: '{}'",
                winID, TTF_GetFontFamilyName(m_ttfFont), m_fontSize);
        }
        return tex;
    }

    void FontAsset::MoveFrom(FontAsset&& other) noexcept {
        if (this == &other) return;

        Cleanup();

        m_fontSize = other.m_fontSize;
        m_lastUseTick = other.m_lastUseTick;
        m_ttfFont = other.m_ttfFont;
        m_glyphAtlasSurf = other.m_glyphAtlasSurf;
        m_ascent = other.m_ascent;
        m_descent = other.m_descent;
        m_lineSkip = other.m_lineSkip;

        m_charToGlyphMetrics = std::move(other.m_charToGlyphMetrics);
        m_asciiGlyphs = std::move(other.m_asciiGlyphs);
        m_asciiPresent = std::move(other.m_asciiPresent);

        m_winIDToGlyphAtlasTexture = std::move(other.m_winIDToGlyphAtlasTexture);
        auto* app = Application::GetInstance();
        for (auto& [winID, cbID] : other.m_winIDToWinCallbackID) {
            if (auto* win = app ? app->GetWindow(winID) : nullptr) {
                win->RemoveOnSDLRendererDestroy(cbID);

                m_winIDToWinCallbackID[winID] = win->AddOnSDLRendererDestroy(
                    [this, winID]() { FreeTextureForWindow(winID); }
                );
            }
        }

        other.m_fontSize = 0;
        other.m_lastUseTick = 0;
        other.m_ttfFont = nullptr;
        other.m_glyphAtlasSurf = nullptr;
        other.m_ascent = 0;
        other.m_descent = 0;
        other.m_lineSkip = 0;
        other.m_charToGlyphMetrics.clear();
        other.m_asciiGlyphs.fill({});
        other.m_asciiPresent.fill(false);
        other.m_winIDToGlyphAtlasTexture.clear();
        other.m_winIDToWinCallbackID.clear();
    }

	void FontAsset::Cleanup() {
        for (auto& [_, tex] : m_winIDToGlyphAtlasTexture)
            SDL_DestroyTexture(tex);

        RemoveAllWindowCloseCB();

        m_charToGlyphMetrics.clear();
        m_winIDToGlyphAtlasTexture.clear();

		if (m_glyphAtlasSurf)
			SDL_DestroySurface(m_glyphAtlasSurf);
		if (m_ttfFont)
			TTF_CloseFont(m_ttfFont);
		m_glyphAtlasSurf = nullptr;
		m_ttfFont = nullptr;

		m_lastUseTick = 0;
		m_fontSize = 0;
	}

    SDL_Surface* FontAsset::GenerateGlypeAtlas(TTF_Font* font, float size) {
        if (TTF_GetFontDirection(font) != TTF_DIRECTION_LTR && TTF_GetFontDirection(font) != TTF_DIRECTION_INVALID) {
            Log::Error("SDLCore::FontAsset::GenerateGlypeAtlas: Could not generate glyph atlas of font '{}', current font system only supports left-to-right fonts!",
                TTF_GetFontFamilyName(font));
            return nullptr;
        }

        /*
        * Current Atlas layout is horizontal (row) if more glyps are added
        * the layout should be changed to quadrat becaus of texture size limits
        */
        Uint32 firstChar = 32;
        Uint32 lastChar = 126;
        SDL_Color white = { 255, 255, 255, 255 };

        struct GlyphTemp {
            Uint32 code;
            SDL_Surface* surf;
            GlyphMetrics metrics;

            GlyphTemp(Uint32 _code, SDL_Surface* _surf, char _c) 
                : code(_code), surf(_surf), metrics(GlyphMetrics{_c}) {
            }

            ~GlyphTemp() {
                SDL_DestroySurface(surf);
                surf = nullptr;
            }
        };

        std::vector<GlyphTemp> glyphs;
        glyphs.reserve(lastChar - firstChar + 1);

        int atlasWidth = 0;
        int atlasHeight = 0;
        int spacingX = 2;// horizontal spacing between glyphs

        for (Uint32 c = firstChar; c <= lastChar; ++c) {
            SDL_Surface* glyphSurf = TTF_RenderGlyph_Blended(font, c, white);
            if (!glyphSurf) {
                Log::Error("SDLCore::FontAsset::GenerateGlypeAtlas: Could not render glyph '{}' for size '{}' of font '{}': {}",
                    c, size, TTF_GetFontFamilyName(font), SDL_GetError());
                continue;
            }

            glyphs.emplace_back(c, glyphSurf, static_cast<char>(c));
            GlyphMetrics& gm = glyphs.back().metrics;

            if (!TTF_GetGlyphMetrics(font, c, &gm.minX, &gm.maxX, &gm.minY, &gm.maxY, &gm.advance)) {
                Log::Error("SDLCore::FontAsset::GenerateGlypeAtlas: Could not get metrics for '{}' (size '{}', font '{}'): {}",
                    c, size, TTF_GetFontFamilyName(font), SDL_GetError());
                // delete the current faild glyph
                glyphs.pop_back();
                SDL_DestroySurface(glyphSurf);
                continue;
            }

            gm.atlasWidth = glyphSurf->w;
            gm.atlasHeight = glyphSurf->h;

            // Calculates the alts size
            atlasWidth += gm.atlasWidth + spacingX;
            atlasHeight = std::max(atlasHeight, glyphSurf->h);
        }

        if (glyphs.empty()) {
            Log::Error("SDLCore::FontAsset::GenerateGlypeAtlas: No glyphs generated for font '{}'", TTF_GetFontFamilyName(font));
            return nullptr;
        }
        //remove the last spacing
        atlasWidth -= spacingX;
        SDL_Surface* atlas = SDL_CreateSurface(atlasWidth, atlasHeight, SDL_PIXELFORMAT_RGBA32);
        if (!atlas) {
            Log::Error("SDLCore::FontAsset::GenerateGlypeAtlas: Failed to create atlas surface: {}", SDL_GetError());
            return nullptr;
        }

        int cursorX = 0;
        for (auto& g : glyphs) {
            if (!g.surf)
                continue;

            SDL_Rect dst;
            dst.x = cursorX;
            dst.y = 0;
            dst.w = g.surf->w;
            dst.h = g.surf->h;

            SDL_BlitSurface(g.surf, nullptr, atlas, &dst);

            g.metrics.atlasX = dst.x;
            g.metrics.atlasY = dst.y;

            m_charToGlyphMetrics[g.code] = g.metrics;
            if (g.code < 256) {
                m_asciiGlyphs[g.code] = g.metrics;
                m_asciiPresent[g.code] = true;
            }

            cursorX += g.surf->w + spacingX;

            SDL_DestroySurface(g.surf);
            g.surf = nullptr;
        }

        return atlas;
    }

    SDL_Texture* FontAsset::CreateTextureForWindow(WindowID winID) {
        auto* app = Application::GetInstance();
        if (!app)
            return nullptr;
        
        auto* win = app->GetWindow(winID);
        if (!win)
            return nullptr;

        auto renderer = win->GetSDLRenderer();
        if (!renderer)
            return nullptr;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, m_glyphAtlasSurf);
        if (texture) {
            m_winIDToGlyphAtlasTexture[winID] = texture;
            m_winIDToWinCallbackID[winID] = win->AddOnSDLRendererDestroy([this, winID]() { FreeTextureForWindow(winID); });
        }

        return texture;
    }

    void FontAsset::FreeTextureForWindow(WindowID winID) {
        // prevents from calling sdl funcs if app is closing
        if (Application::IsQuit())
            return;

        auto* app = Application::GetInstance();
        if (!app)
            return;

        auto* win = app->GetWindow(winID);
        if (!win)
            return;

        auto itAtlas = m_winIDToGlyphAtlasTexture.find(winID);
        if (itAtlas != m_winIDToGlyphAtlasTexture.end()) {
            SDL_DestroyTexture(itAtlas->second);
            m_winIDToGlyphAtlasTexture.erase(itAtlas);
        }

        auto itCallback = m_winIDToWinCallbackID.find(winID);
        if (itCallback != m_winIDToWinCallbackID.end()) {
            win->RemoveOnSDLRendererDestroy(itCallback->second);
            m_winIDToWinCallbackID.erase(itCallback);
        }
    }

    void FontAsset::RemoveAllWindowCloseCB() {
        auto* app = Application::GetInstance();
        if (!app) {
            m_winIDToWinCallbackID.clear();
            return;
        }

        for (auto [winID, cbID] : m_winIDToWinCallbackID) {
            if (auto* win = app->GetWindow(winID))
                win->RemoveOnSDLRendererDestroy(cbID);
        }
        m_winIDToWinCallbackID.clear();
    }

    void FontAsset::RemoveWindowCloseCB(WindowID winID) {
        auto itCallback = m_winIDToWinCallbackID.find(winID);
        if (itCallback == m_winIDToWinCallbackID.end())
            return;

        if (auto* app = Application::GetInstance()) {
            if (auto* win = app->GetWindow(winID)) {
                win->RemoveOnSDLRendererDestroy(itCallback->second);
            }
        }

        m_winIDToWinCallbackID.erase(itCallback);
    }
    
}