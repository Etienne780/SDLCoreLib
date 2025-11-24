#pragma once
#include <vector>
#include <unordered_map>
#include "Types/Font/FontAsset.h"
#include "Types/Types.h"

namespace SDLCore {

	class Font {
	public:
		Font(bool useDefaultFont = false);
		Font(const SystemFilePath& path, std::vector<float> sizes = {}, size_t cachedSizes = 10);
		Font(const unsigned char* data, size_t dataSize, std::vector<float> fontSizes = {}, size_t cachedSizes = 10);

		Font* SelectSize(float size);
		Font* SetCachSize(size_t size);
		/*
		* @brief sets the data from this font object
		* @param data has to be alive for as long as this object uses this data
		* @param dataSize is the size of the data in bytes
		*/
		Font* SetFontData(const unsigned char* data, size_t dataSize);
		Font* Setpath(const SystemFilePath& path);
		Font* SetDefaultFont(bool active);
		Font* Clear();

		FontAsset* GetFontAsset();
		FontAsset* GetFontAsset(float size);

		float GetSelectedSize() const;
		SystemFilePath GetSystemPath() const;
		std::string GetFileName() const;
		size_t GetCachSize() const;
		size_t GetNumberOfCachedFontAssets() const;

	private:
		bool m_isInvalid = false;// if the font is invalid. example path is not valid
		bool m_loadedFromMem = false;
		bool m_useDefault = false;// if the default font is active
		bool m_isFilePathInvalidValid = false;
		SystemFilePath m_path;
		const unsigned char* m_fontData = nullptr;
		size_t m_fontDataSize = 0;
		std::vector<FontAsset> m_fontAssets;
		size_t m_maxFontSizesCached = 10;/**< number of font sizes that get stored at the same time */

		size_t m_globalAccessCounter = 0;
		float m_selectedSize = -1;

		bool CreateFontAsset(float size);
		bool CreateFontAssetFromMem(const unsigned char* data, size_t dataSize, float fontSize);
		bool CreateFallbackFontAsset(float size);
		void CalculateCachedFonts();
		void CheckFilePath(const SystemFilePath& path);

		/*
		* @brief Marks this font object as invalid
		* @return is true when newly set to invalid
		*/
		bool MarkAsInvalid();
		void MarkAsValid();
	};

}
