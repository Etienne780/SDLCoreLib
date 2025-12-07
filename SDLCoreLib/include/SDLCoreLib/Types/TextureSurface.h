#pragma once
#include <SDL3/SDL.h>
#include "Types.h"

namespace SDLCore {
	
	/*
	* @brief Small wrapper around the SDL_Surface. Has internal ref counting throught the
	*
	* TextureManager. keeps the SDL_Texutre alive as long as a ref exists
	*/
	class TextureSurface {
	public:
		TextureSurface() = default;
		TextureSurface(SDL_Surface* surface);
		TextureSurface(const TextureSurface& other);
		TextureSurface(TextureSurface&& other) noexcept;
		~TextureSurface();

		TextureSurface& operator=(const TextureSurface& other);
		TextureSurface& operator=(TextureSurface&& other) noexcept;
		
		/**
		* @brief Checks whether this texture surface is invalid.
		* @return True if the texture ID is invalid or the internal SDL_Surface pointer is null; otherwise false.
		*/
		bool IsInvalid();

		/**
		* @brief Returns the unique ID of this texture surface.
		* @return TextureID assigned by the TextureManager.
		*/
		TextureID GetID() const;

		/**
		* @brief Returns the underlying SDL_Surface pointer.
		* @return Pointer to the SDL_Surface managed by this object, or nullptr if invalid.
		*/
		SDL_Surface* GetSurface() const;


	private:
		TextureID m_id;
		SDL_Surface* m_surface = nullptr;
	};

}