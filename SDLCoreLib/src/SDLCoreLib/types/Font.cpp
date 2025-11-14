#include <SDL3_ttf/SDL_ttf.h>
#include <CoreLib/Log.h>
#include "types/Font.h"

namespace SDLCore {

	Font::Font(const SystemFilePath& path, std::vector<float> sizes = {})
		: m_path(path){

		for (float size : sizes) {
			if (!CreateFontAsset(size)) {
				
			}
		}
	}

	Font* Font::SelectSize(float size) {
		m_selectedSize = size;
	}

	Font* Font::Setpath(const SystemFilePath& path) {
		m_path = path;
		Clear();
	}

	Font* Font::Clear() {
		m_sizeToFontAsset.clear();
	}

	bool Font::CreateFontAsset(float size) {
		TTF_Font* font = TTF_OpenFont(m_path.u8string().c_str(), size);
		if (!font) {
			return false;
		}

		m_sizeToFontAsset[size] = FontAsset(font);
		return true;
	}

}