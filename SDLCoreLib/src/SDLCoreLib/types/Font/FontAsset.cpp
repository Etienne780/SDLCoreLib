#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <CoreLib/Log.h>

#include "Application.h"
#include "Types/Font/FontAsset.h"

namespace SDLCore {

	FontAsset::FontAsset(TTF_Font* font, float size)
		: ttfFont(font), fontSize(size) {
        if (font) {
            glyphAtlasSurf = GenerateGlypeAtlas(font, size);
            if (!glyphAtlasSurf) {
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

    GlypeMetrics* FontAsset::GetGlyphMetrics(char code) {
        auto it = m_charToGlypeMetrics.find(code);
        return (it == m_charToGlypeMetrics.end()) ? nullptr : &it->second;
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
                        win->GetName(), winID, TTF_GetFontFamilyName(ttfFont), fontSize);
                    return tex;
                }
            }

            Log::Error("SDLCore::FontAsset::GetGlyphAtlasTexture: Could not create texture for window '{}', Font: '{}' size: '{}'",
                winID, TTF_GetFontFamilyName(ttfFont), fontSize);
        }
        return tex;
    }

    void FontAsset::MoveFrom(FontAsset&& other) noexcept {
        if (this == &other) return;

        Cleanup();

        fontSize = other.fontSize;
        lastUseTick = other.lastUseTick;
        ttfFont = other.ttfFont;
        glyphAtlasSurf = other.glyphAtlasSurf;
        m_charToGlypeMetrics = std::move(other.m_charToGlypeMetrics);

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

        other.fontSize = 0;
        other.lastUseTick = 0;
        other.ttfFont = nullptr;
        other.glyphAtlasSurf = nullptr;
        other.m_charToGlypeMetrics.clear();
        other.m_winIDToGlyphAtlasTexture.clear();
        other.m_winIDToWinCallbackID.clear();
    }

	void FontAsset::Cleanup() {
        for (auto& [_, tex] : m_winIDToGlyphAtlasTexture)
            SDL_DestroyTexture(tex);

        RemoveAllWindowCloseCB();

        m_charToGlypeMetrics.clear();
        m_winIDToGlyphAtlasTexture.clear();

		if (glyphAtlasSurf)
			SDL_DestroySurface(glyphAtlasSurf);
		if (ttfFont)
			TTF_CloseFont(ttfFont);
		glyphAtlasSurf = nullptr;
		ttfFont = nullptr;

		lastUseTick = 0;
		fontSize = 0;
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
            GlypeMetrics metrics;

            GlyphTemp(Uint32 _code, SDL_Surface* _surf, char _c) 
                : code(_code), surf(_surf), metrics(GlypeMetrics{_c}) {
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
            GlypeMetrics& gm = glyphs.back().metrics;

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

            m_charToGlypeMetrics[g.code] = g.metrics;
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

        auto renderer = win->GetSDLRenderer().lock();
        if (!renderer)
            return nullptr;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get(), glyphAtlasSurf);
        if (texture) {
            m_winIDToGlyphAtlasTexture[winID] = texture;
            m_winIDToWinCallbackID[winID] = win->AddOnSDLRendererDestroy([this, winID]() { FreeTextureForWindow(winID); });
        }

        Log::Debug("winIDCB: {};", m_winIDToWinCallbackID);

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