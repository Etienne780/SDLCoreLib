#include <SDL3_ttf/SDL_ttf.h>
#include <CoreLib/Log.h>
#include <CoreLib/File.h>

#include "Types/Font/Font.h"

namespace SDLCore {

	Font::Font(const SystemFilePath& path, std::vector<float> sizes, size_t cachedSizes)
		: m_path(path), m_maxFontSizesCached(cachedSizes) {

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

	Font* Font::SelectSize(float size) {
		if (m_path.empty()) {
			Log::Warn("SDLCore::Font::SelectSize: Cant select font size '{}', font path is empty!", size);
			return this;
		}

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

	Font* Font::Setpath(const SystemFilePath& path) {
		m_path = path;
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
		if (m_path.empty()) 
			return false;

		if (size <= 0) {
			Log::Error("SDLCore::Font::CreateFontAsset: Could not create FontAsset with size '{}', size is equal or less than zero!", size);
			return false;
		}

		TTF_Font* font = TTF_OpenFont(m_path.u8string().c_str(), size);
		if (!font) {
			Log::Error("SDLCore::Font::CreateFontAsset: Could not create FontAsset with size '{}': {}", size, SDL_GetError());
			return false;
		}

		m_fontAssets.emplace_back(font, size);
		return true;
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

}