#pragma once
#include <array>
#include <unordered_map>

#include "Types/Types.h"
#include "FontGlyphMetrics.h"

struct SDL_Surface;
struct TTF_Font;
namespace SDLCore {
	/*
	* @brief is a helper class used for the Font class. Should not be used
	*/
	class FontAsset {
	public:
		FontAsset(TTF_Font* font, float size);
		~FontAsset();

		FontAsset(const FontAsset&) = delete;
		FontAsset& operator=(const FontAsset&) = delete;

		FontAsset(FontAsset&& other) noexcept;
		FontAsset& operator=(FontAsset&& other) noexcept;

		float fontSize = -1;
		size_t lastUseTick = 0;
		TTF_Font* ttfFont = nullptr;
		SDL_Surface* glyphAtlasSurf = nullptr;

		
		GlyphMetrics* GetGlyphMetrics(uint32_t code);
		SDL_Texture* GetGlyphAtlasTexture(WindowID winID);

	private:
		std::array<GlyphMetrics, 256> m_asciiGlyphs;
		std::array<bool, 256> m_asciiPresent;

		std::unordered_map<Uint32, GlyphMetrics> m_charToGlyphMetrics;
		std::unordered_map<WindowID, SDL_Texture*> m_winIDToGlyphAtlasTexture;
		std::unordered_map<WindowID, WindowCallbackID> m_winIDToWinCallbackID;

		void MoveFrom(FontAsset&& other) noexcept;
		void Cleanup();

		SDL_Surface* GenerateGlypeAtlas(TTF_Font* font, float size);
		SDL_Texture* CreateTextureForWindow(WindowID winID);
		void FreeTextureForWindow(WindowID winID);

		void RemoveAllWindowCloseCB();
		void RemoveWindowCloseCB(WindowID winID);
	};

}