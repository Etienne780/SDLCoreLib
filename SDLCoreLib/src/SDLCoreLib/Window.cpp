#include "Window.h"

namespace SDLCore {

	Window::Window(WindowID id)
		: m_id(id) {
	}

	Window::Window(WindowID id, const std::string& name, int width, int height)
		: m_id(id), m_name(name), m_width(width), m_height(height) {
	}

	WindowID Window::GetID() const {
		return m_id;
	}

	void Window::Create() {
		if (m_width < 0) m_width = 0;
		if (m_height < 0) m_height = 0;
		m_sdlWindow = SDL_CreateWindow(m_name.c_str(), m_width, m_height, GetWindowFlags());
	}

	SDL_WindowFlags Window::GetWindowFlags() {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;
		
		return flags;
	}

}