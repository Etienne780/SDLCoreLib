#include "Application.h"
#include "Internal/FontManager.h"

namespace SDLCore {

	FontManager::FontEntry::FontEntry(TTF_Font* fontVal, uint32_t initRefCount)
		: font(fontVal), refCount(initRefCount) {
	}

	FontManager::FontEntry::~FontEntry() {
		if (!IsSDLQuit())
			TTF_CloseFont(font);
		font = nullptr;
	}

	FontManager::FontEntry::FontEntry(FontEntry&& other) noexcept
		: font(other.font), refCount(other.refCount) {
		other.font = nullptr;
		other.refCount = 0;
	}

	FontManager::FontEntry& FontManager::FontEntry::operator=(FontEntry&& other) noexcept {
		if (this == &other)
			return *this;

		if (font && !IsSDLQuit())
			TTF_CloseFont(font);

		font = other.font;
		refCount = other.refCount;
		other.font = nullptr;
		other.refCount = 0;
		return *this;
	}

	FontManager::~FontManager() {
		if (!IsSDLQuit())
			ClearAllFontEntries();
	}

	FontManager& FontManager::GetInstance() {
		static FontManager instance;
		return instance;
	}

	FontID FontManager::RegisterFont(TTF_Font* font) {
		std::lock_guard lock(m_mutex);

		FontID newID = FontID(m_idManager.GetNewUniqueIdentifier());

		FontEntry tmp(font, 1);
		auto [it, inserted] = m_fontEntrys.emplace(
				newID, std::move(tmp)
		);

		if (!inserted) {
			TTF_CloseFont(font);
			return FontID(SDLCORE_INVALID_ID);
		}
		return newID;
	}

	void FontManager::IncreaseRef(FontID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_fontEntrys.find(id);
		if (it == m_fontEntrys.end())
			return;

		it->second.refCount++;
	}

	void FontManager::DecreaseRef(FontID id) {
		std::lock_guard lock(m_mutex);

		auto it = m_fontEntrys.find(id);
		if (it == m_fontEntrys.end())
			return;

		auto& entry = it->second;
		if (entry.refCount > 0)
			entry.refCount--;

		// delete font if not used
		if (it->second.refCount == 0) {
			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_fontEntrys.erase(it);
		}
	}

	TTF_Font* FontManager::GetFont(FontID id) const {
		std::lock_guard lock(m_mutex);

		auto it = m_fontEntrys.find(id);
		if (it == m_fontEntrys.end())
			return nullptr;

		return it->second.font;
	}

	void FontManager::ClearAllFontEntries() {
		std::lock_guard lock(m_mutex);

		while (!m_fontEntrys.empty()) {
			auto it = m_fontEntrys.begin();

			m_idManager.FreeUniqueIdentifier(it->first.value);
			m_fontEntrys.erase(it);
		}
	}

}