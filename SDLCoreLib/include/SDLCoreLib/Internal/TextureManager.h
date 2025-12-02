#pragma once
#include <unordered_map>
#include <SDL3/SDL.h>

#include "Types/Types.h"
#include "IDManager.h"

namespace SDLCore {

	class TextureSurface;

	/*
	* Manages how many refs to an SDL_Suface exist and deletes it if none exist anymore
	*/
	class TextureManager {
	friend class TextureSurface;
	private:
		struct TextureAsset {
			SDL_Surface* surface = nullptr;
			uint32_t refCount = 0;
			TextureAsset(SDL_Surface* surfaceVal, uint32_t initRefCount) 
				:surface(surfaceVal), refCount(initRefCount) {
			}
		};

		TextureManager() = default;
		~TextureManager();

		IDManager m_idManager;
		std::unordered_map<TextureID, TextureAsset> m_textureAssets;

		static TextureManager& GetInstance();

		TextureID RegisterTexture(SDL_Surface* surface);
		void IncreaseRef(TextureID id);
		void DecreaseRef(TextureID id);
	};

}