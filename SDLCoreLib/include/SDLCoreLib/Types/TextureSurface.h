#pragma once
#include <SDL3/SDL.h>
#include "Types.h"

namespace SDLCore {
	
	/*
	* Small wrapper around the SDL_Surface. Has internal ref counting throught the 
	* TextureManager. keeps the SDL_Texutre alvie as long as a ref exists
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

		bool IsInvalid();

		TextureID GetID() const;
		SDL_Surface* GetSurface() const;

	private:
		TextureID m_id;
		SDL_Surface* m_surface = nullptr;
	};

}