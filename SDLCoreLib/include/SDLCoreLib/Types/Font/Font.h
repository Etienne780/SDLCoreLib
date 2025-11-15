#pragma once
#include <vector>
#include <unordered_map>
#include "Types/Font/FontAsset.h"
#include "Types/Types.h"

namespace SDLCore {

	class Font {
	public:
		Font() = default;
		Font(const SystemFilePath& path, std::vector<float> sizes = {}, size_t cachedSizes = 10);

		Font* SelectSize(float size);
		Font* SetCachSize(size_t size);
		Font* Setpath(const SystemFilePath& path);
		Font* Clear();

		FontAsset* GetFontAsset();
		FontAsset* GetFontAsset(float size);

		float GetSelectedSize() const;
		SystemFilePath GetSystemPath() const;
		std::string GetFileName() const;
		size_t GetCachSize() const;
		size_t GetNumberOfCachedFontAssets() const;

	private:
		SystemFilePath m_path;
		std::vector<FontAsset> m_fontAssets;
		size_t m_maxFontSizesCached = 10;/**< number of font sizes that get stored at the same time */

		size_t m_globalAccessCounter = 0;
		float m_selectedSize = -1;

		bool CreateFontAsset(float size);
		void CalculateCachedFonts();
	};

}
