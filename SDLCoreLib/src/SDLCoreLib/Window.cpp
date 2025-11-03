#include <CoreLib/Log.h>
#include "Window.h"

namespace SDLCore {

	Window::Window(WindowID id)
		: m_id(id) {
	}

	Window::Window(WindowID id, const std::string& name, int width, int height)
		: m_id(id), m_name(name), m_width(width), m_height(height) {
	}

	Window::~Window() {
		SDL_DestroyRenderer(m_sdlRenderer);
		SDL_DestroyWindow(m_sdlWindow);
	}

	WindowID Window::GetID() const {
		return m_id;
	}

	SDL_WindowID Window::GetSDLID() const {
		return m_sdlWindow ? SDL_GetWindowID(m_sdlWindow) : SDLCORE_INVALID_ID;
	}

	std::string Window::GetName() const {
		return m_name;
	}

	int Window::GetWidth() const {
		return m_width;
	}

	int Window::GetHeight() const {
		return m_height;
	}

	std::unique_ptr<Window> Window::CreateInstance(WindowID id) {
		return std::unique_ptr<Window>(new Window(id));
	}

	std::unique_ptr<Window> Window::CreateInstance(WindowID id, const std::string& name, int width, int height) {
		return std::unique_ptr<Window>(new Window(id, name, width, height));
	}

	void Window::Create() {
		if (m_width < 0) m_width = 0;
		if (m_height < 0) m_height = 0;
		m_sdlWindow = SDL_CreateWindow(m_name.c_str(), m_width, m_height, GetWindowFlags());

		if (!m_sdlWindow) {
			Log::Error("Failed to create window '{}': {}", m_name, SDL_GetError());
			return;
		}

		m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, nullptr);
		if (!m_sdlRenderer) {
			Log::Error("Renderer creation failed on window '{}': {}", m_name, SDL_GetError());
			return;
		}
	}

	void Window::Destroy() {
		if(m_sdlRenderer)
			SDL_DestroyRenderer(m_sdlRenderer);
		if(m_sdlWindow)
			SDL_DestroyWindow(m_sdlWindow);

		m_sdlRenderer = nullptr;
		m_sdlWindow = nullptr;
	}

	SDL_WindowFlags Window::GetWindowFlags() {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;
		
		return flags;
	}

}