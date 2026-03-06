#pragma once
#include <mutex>
#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>

#include "Types/Types.h"

namespace SDLCore {

	class Application;
	class FontResource;

	/*
	* Manages how many refs to an TTF_Font exist and deletes it if none exist anymore
	*/
	class FontManager {
	friend class Application;
	friend class FontResource;
	private:
		struct FontEntry {
			TTF_Font* font = nullptr;
			uint32_t refCount = 0;
			FontEntry(TTF_Font* font, uint32_t initRefCount);
			~FontEntry();

			FontEntry(const FontEntry&) = delete;
			FontEntry(FontEntry&&) noexcept;

			FontEntry& operator=(const FontEntry&) = delete;
			FontEntry& operator=(FontEntry&&) noexcept;
		};

		SDLCoreIDManager m_idManager;
		std::unordered_map<FontID, FontEntry> m_fontEntrys;
		mutable std::mutex m_mutex;

		FontManager() = default;
		~FontManager();

		static FontManager& GetInstance();

		FontID RegisterFont(TTF_Font* font);
		void IncreaseRef(FontID id);
		void DecreaseRef(FontID id);

		TTF_Font* GetFont(FontID id) const;

		// should only be called at programm end
		void ClearAllFontEntries();
	};

}