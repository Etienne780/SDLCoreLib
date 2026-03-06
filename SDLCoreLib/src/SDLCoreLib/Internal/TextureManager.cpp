#include "Application.h"
#include "Internal/TextureManager.h"

namespace SDLCore {

	TextureManager::TextureEntry::TextureEntry(SDL_Surface* surfaceVal, uint32_t initRefCount)
		:surface(surfaceVal), refCount(initRefCount) {
	}

	TextureManager::TextureEntry::~TextureEntry() {
		if (!IsSDLQuit())
			SDL_DestroySurface(surface);
		surface = nullptr;
	}

	TextureManager::TextureEntry::TextureEntry(TextureEntry&& other) noexcept
		: surface(other.surface), refCount(other.refCount) {
		other.surface = nullptr;
		other.refCount = 0;
	}

	TextureManager::TextureEntry& TextureManager::TextureEntry::operator=(TextureEntry&& other) noexcept {
		if (this == &other)
			return *this;

		if (surface && !IsSDLQuit())
			SDL_DestroySurface(surface);

		surface = other.surface;
		refCount = other.refCount;
		other.surface = nullptr;
		other.refCount = 0;
		return *this;
	}

	TextureManager::~TextureManager() {
		if (!IsSDLQuit())
			ClearAllTexturesEntries();
	}

	TextureManager& TextureManager::GetInstance() {
		static TextureManager instance;
		return instance;
	}

	TextureID TextureManager::RegisterTexture(SDL_Surface* surface) {
		std::lock_guard lock(m_mutex);

		TextureID newID = TextureID(m_idManager.GetNewUniqueIdentifier());

		TextureEntry tmp(surface, 1);
		auto [it, inserted] = m_textureEntrys.emplace(
			newID, std::move(tmp)
		);

		if (!inserted) {
			SDL_DestroySurface(surface);
			return TextureID(SDLCORE_INVALID_ID);
		}
		return newID;
	}
	
	void TextureManager::IncreaseRef(TextureID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_textureEntrys.find(id);
		if (it == m_textureEntrys.end())
			return;

		it->second.refCount++;
	}

	void TextureManager::DecreaseRef(TextureID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_textureEntrys.find(id);
		if (it == m_textureEntrys.end())
			return;

		auto& asset = it->second;
		if (asset.refCount > 0)
			asset.refCount--;

		// delete texture if not used
		if (it->second.refCount == 0) {
			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_textureEntrys.erase(it);
		}
	}

	SDL_Surface* TextureManager::GetSurface(TextureID id) const {
		std::lock_guard lock(m_mutex);

		auto it = m_textureEntrys.find(id);
		if (it == m_textureEntrys.end())
			return nullptr;

		return it->second.surface;
	}

	void TextureManager::ClearAllTexturesEntries() {
		std::lock_guard lock(m_mutex);

		while (!m_textureEntrys.empty()) {
			auto it = m_textureEntrys.begin();

			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_textureEntrys.erase(it);
		}
	}

}