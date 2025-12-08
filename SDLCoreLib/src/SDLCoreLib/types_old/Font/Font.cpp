#include <SDL3_ttf/SDL_ttf.h>
#include <CoreLib/Log.h>
#include <CoreLib/File.h>

#include "Types/Font/Nurom_Bold_ttf.h"
#include "Types/Font/Font.h"

namespace SDLCore {

	Font::Font(bool useDefaultFont) {
		SetDefaultFont(useDefaultFont);
	}

	Font::Font(const SystemFilePath& path, std::vector<float> sizes, size_t cachedSizes)
		: m_path(path), m_maxFontSizesCached(cachedSizes) {
		CheckFilePath(path);
		if (m_maxFontSizesCached < sizes.size()) {
			Log::Warn("SDLCore::Font: The number of predefined font sizes '{}' exceeds the configured cache size '{}' for '{}'. Some sizes may not be cached.",
				sizes.size(), m_maxFontSizesCached, path.filename());
		}

		for (float size : sizes) {
			if (!CreateFontAsset(size)) {
				Log::Error("SDLCore::Font: Could not create Font with size {}!", size);
			}
		}

		CalculateCachedFonts();
	}

	Font::Font(const unsigned char* data, size_t dataSize, std::vector<float> fontSizes, size_t cachedSizes)
		: m_maxFontSizesCached(cachedSizes), m_loadedFromMem(true) {

		if (m_maxFontSizesCached < fontSizes.size()) {
			Log::Warn("SDLCore::Font(FromMem): The number of predefined font sizes '{}' exceeds the configured cache size '{}'. Some sizes may not be cached.",
				fontSizes.size(), m_maxFontSizesCached);
		}

		for (float size : fontSizes) {
			if (!CreateFontAssetFromMem(data, dataSize, size)) {
				Log::Error("SDLCore::Font: Could not create Font with size {}!", size);
			}
		}

		CalculateCachedFonts();
	}

	Font* Font::SelectSize(float size) {
		if (size <= 0) {
			Log::Warn("SDLCore::Font::SelectSize: Cant select font size '{}', font path is empty!", size);
			return this;
		}

		if (m_selectedSize == size)
			return this;

		FontAsset* fontAsset = GetFontAsset(size);
		if (!fontAsset) {
			Log::Error("SDLCore::Font::SelectSize: Could not get Font with size '{}'!", size);
			return this;
		}

		// is for font caching (LRU)
		fontAsset->lastUseTick = m_globalAccessCounter;
		m_globalAccessCounter++;
		CalculateCachedFonts();

		m_selectedSize = size;
		return this;
	}

	Font* Font::SetCachSize(size_t size) {
		m_maxFontSizesCached = size;
		CalculateCachedFonts();
		return this;
	}

	Font* Font::SetFontData(const unsigned char* data, size_t dataSize) {
		m_fontData = data;
		m_fontDataSize = dataSize;
		m_path = "";
		m_loadedFromMem = true;
		MarkAsValid();
		return Clear();
	}

	Font* Font::Setpath(const SystemFilePath& path) {
		m_fontData = nullptr;
		m_fontDataSize = 0;

		m_path = path;
		CheckFilePath(path);
		m_useDefault = false;
		m_loadedFromMem = false;
		MarkAsValid();
		return Clear();
	}

	Font* Font::SetDefaultFont(bool active) {
		m_useDefault = active;
		if (active) {
			SetFontData(StaticFont::Fallback_ttf, 
				StaticFont::Fallback_ttf_len);
		}
		else {
			Setpath("");
		}
		return Clear();
	}

	Font* Font::Clear() {
		m_fontAssets.clear();
		m_selectedSize = -1;
		m_globalAccessCounter = 0;
		return this;
	}

	FontAsset* Font::GetFontAsset() {
		return GetFontAsset(m_selectedSize);
	}

	FontAsset* Font::GetFontAsset(float size) {
		auto it = std::find_if(m_fontAssets.begin(), m_fontAssets.end(),
			[size](const FontAsset& fontAsset) { return fontAsset.fontSize == size; });

		if (it != m_fontAssets.end())
			return &(*it);

		if (!CreateFontAsset(size))
			return nullptr;

		return &m_fontAssets.back();
	}

	float Font::GetSelectedSize() const {
		return m_selectedSize;
	}

	SystemFilePath Font::GetSystemPath() const {
		return m_path;
	}

	std::string Font::GetFileName() const {
		return (m_path.has_filename()) ? m_path.filename().string() : "";
	}

	size_t Font::GetCachSize() const {
		return m_maxFontSizesCached;
	}

	size_t Font::GetNumberOfCachedFontAssets() const {
		return m_fontAssets.size();
	}

	// could create multiple fontassets with the same size
	bool Font::CreateFontAsset(float size) {
		if (!m_loadedFromMem && m_isFilePathInvalidValid) {
			if (MarkAsInvalid()) {
				Log::Warn("SDLCore::Font::CreateFontAsset: Could not create font asset for size '{}', used fallback font!", size);
			}
			CreateFallbackFontAsset(size);
			return true;
		}

		if (size <= 0) {
			Log::Error("SDLCore::Font::CreateFontAsset: Could not create FontAsset with size '{}', size is equal or less than zero!", size);
			return false;
		}

		if (m_loadedFromMem) {
			return CreateFontAssetFromMem(m_fontData, m_fontDataSize, size);
		}
		
		TTF_Font* font = TTF_OpenFont(m_path.string().c_str(), size);
		if (!font) {
			Log::Error("SDLCore::Font::CreateFontAsset: Could not create FontAsset with size '{}': {}", size, SDL_GetError());
			return false;
		}

		m_fontAssets.emplace_back(font, size);
		return true;
	}

	bool Font::CreateFontAssetFromMem(const unsigned char* data, size_t dataSize, float fontSize) {
		SDL_IOStream* io = SDL_IOFromMem(
			const_cast<unsigned char*>(data), dataSize
		);

		if (!io) {
			Log::Error("SDLCore::Font::CreateFontAssetFromMem: Could not create FontAsset with size '{}', io is nullptr: {}", fontSize, SDL_GetError());
			return false;
		}

		TTF_Font* font = TTF_OpenFontIO(io, 1, fontSize);
		if (!font) {
			Log::Error("SDLCore::Font::CreateFontAssetFromMem: Could not create FontAsset with size '{}': {}", fontSize, SDL_GetError());
			return false;
		}

		m_fontAssets.emplace_back(font, fontSize);
		return true;
	}

	bool Font::CreateFallbackFontAsset(float size) {
		return CreateFontAssetFromMem(
			StaticFont::Fallback_ttf,
			StaticFont::Fallback_ttf_len, size);
	}

	void Font::CalculateCachedFonts() {
		if (m_fontAssets.size() <= m_maxFontSizesCached)
			return;

		size_t diff = m_fontAssets.size() - m_maxFontSizesCached;
		std::sort(m_fontAssets.begin(), m_fontAssets.end(),
			[](const FontAsset& a, const FontAsset& b) { return a.lastUseTick > b.lastUseTick; });

		m_fontAssets.erase(m_fontAssets.end() - diff, m_fontAssets.end());

		float selectedSize = m_selectedSize;
		if (std::find_if(m_fontAssets.begin(), m_fontAssets.end(),
			[selectedSize](const FontAsset& asset) { return asset.fontSize == selectedSize; }) == m_fontAssets.end()) {
			m_selectedSize = -1;
		}
	}

	void Font::CheckFilePath(const SystemFilePath& path) {
		m_isFilePathInvalidValid = (path.empty()|| !File::Exists(path));
	}

	bool Font::MarkAsInvalid() {
		bool result = false;
		if (!m_isInvalid) {
			if(m_path.empty())
				Log::Warn("SDLCore::Font: Font is invalid, path was empty!");
			else
				Log::Warn("SDLCore::Font: Font '{}' is invalid!", m_path);
			result = true;
		}
		m_isInvalid = true;
		return result;
	}
	
	void Font::MarkAsValid() {
		m_isInvalid = false;
	}

}