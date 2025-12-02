#include "Internal/TextureManager.h"

namespace SDLCore {

	TextureManager::~TextureManager() {
		while (!m_textureAssets.empty()) {
			auto it = m_textureAssets.begin();

			auto& asset = it->second;
			SDL_DestroySurface(asset.surface);
			asset.surface = nullptr;
			m_idManager.FreeUniqueIdentifier(it->first.value);

			m_textureAssets.erase(it);
		}
	}

	TextureManager& TextureManager::GetInstance() {
		static TextureManager instance;
		return instance;
	}

	TextureID TextureManager::RegisterTexture(SDL_Surface* surface) {
		TextureID newID = TextureID(m_idManager.GetNewUniqueIdentifier());
		m_textureAssets.emplace(newID, TextureAsset(surface, 1));
		return newID;
	}
	
	void TextureManager::IncreaseRef(TextureID id) {
		auto it = m_textureAssets.find(id);
		if (it == m_textureAssets.end())
			return;

		it->second.refCount++;
	}

	void TextureManager::DecreaseRef(TextureID id) {
		auto it = m_textureAssets.find(id);
		if (it == m_textureAssets.end())
			return;

		auto& asset = it->second;
		asset.refCount--;

		// delete texture if not used
		if (it->second.refCount <= 0) {
			SDL_DestroySurface(asset.surface);
			asset.surface = nullptr;
			m_idManager.FreeUniqueIdentifier(it->first.value);

			m_textureAssets.erase(it);
		}
	}

}