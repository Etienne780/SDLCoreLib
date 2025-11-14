#pragma once
#include <vector>
#include <unordered_map>
#include "SDLCoreTypes.h"

namespace SDLCore {

	class Font {
	public:
		Font() = default;
		Font(const SystemFilePath& path, std::vector<float> sizes = {});

		Font* SelectSize(float size);
		Font* Setpath(const SystemFilePath& path);
		Font* Clear();
	private:
		class FontAsset {
		public:
			FontAsset(TTF_Font* font) : ttfFont(font) {}
			~FontAsset() { TTF_CloseFont(ttfFont); };
			int lastUseCount = 0;
			TTF_Font* ttfFont = nullptr;
		};

		SystemFilePath m_path;
		std::unordered_map<int, FontAsset> m_sizeToFontAsset;

		float m_selectedSize = -1;
		FontAsset* m_selectedFontAsset = nullptr;

		bool CreateFontAsset(float size);
	};

}
