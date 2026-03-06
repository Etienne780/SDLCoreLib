#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Types/Types.h"

namespace SDLCore {

	/*
	* @brief Small wrapper around the TTF_Font. Has internal ref counting throught the
	*
	* FontManager. keeps the TTF_Font alive as long as a ref exists
	*/
	class FontResource {
	public:
		FontResource() = default;
		FontResource(TTF_Font* font);
		FontResource(const FontResource& other);
		FontResource(FontResource&& other) noexcept;
		~FontResource();

		FontResource& operator=(const FontResource& other);
		FontResource& operator=(FontResource&& other) noexcept;

		/**
		* @brief Checks whether this texture surface is invalid.
		* @return True if the texture ID is invalid or the internal SDL_Surface pointer is null; otherwise false.
		*/
		bool IsInvalid();

		/**
		* @brief Returns the unique ID of this font.
		* @return FontID assigned by the FontManager.
		*/
		FontID GetID() const;

		/**
		* @brief Returns the underlying TTF_Font pointer.
		* @return Pointer to the TTF_Font managed by this object, or nullptr if invalid.
		*/
		TTF_Font* GetFont() const;


	private:
		FontID m_id;
	};

}