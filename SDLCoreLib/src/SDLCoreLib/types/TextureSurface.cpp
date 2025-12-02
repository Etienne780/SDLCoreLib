#include "Internal/TextureManager.h"
#include "Types/TextureSurface.h"

namespace SDLCore {

	TextureSurface::TextureSurface(SDL_Surface* surface) 
		: m_surface(surface) {
		m_id = TextureManager::GetInstance().RegisterTexture(surface);
	}

	TextureSurface::TextureSurface(const TextureSurface& other) {
		m_id = other.m_id;
		m_surface = other.m_surface;
		TextureManager::GetInstance().IncreaseRef(m_id);
	}

	TextureSurface::TextureSurface(TextureSurface&& other) noexcept {
		m_id = other.m_id;
		m_surface = other.m_surface;

		other.m_id.value = SDLCORE_INVALID_ID;
		other.m_surface = nullptr;
	}

	TextureSurface::~TextureSurface() {
		TextureManager::GetInstance().DecreaseRef(m_id);
	}

	TextureSurface& TextureSurface::operator=(const TextureSurface& other) {
		TextureSurface ts;
	}

	TextureSurface& TextureSurface::operator=(TextureSurface&& other) noexcept {
		
	}

	bool TextureSurface::IsInvalid() {
		return (m_id == SDLCORE_INVALID_ID || m_surface == nullptr);
	}

	TextureID TextureSurface::GetID() const {
		return m_id;
	}
	
	SDL_Surface* TextureSurface::GetSurface() const {
		return m_surface;
	}

}