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

	void Window::CreateWindow() {
		if (m_width < 0) m_width = 0;
		if (m_height < 0) m_height = 0;
		m_sdlWindow = SDL_CreateWindow(m_name.c_str(), m_width, m_height, GetWindowFlags());

		if (!m_sdlWindow) {
			Log::Error("Failed to create window '{}': {}", m_name, SDL_GetError());
			return;
		}
	}

	void Window::DestroyWindow() {
		DestroyRenderer();
		if (m_sdlWindow)
			SDL_DestroyWindow(m_sdlWindow);
		m_sdlWindow = nullptr;
	}

	void Window::CreateRenderer() {
		m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, nullptr);
		if (!m_sdlRenderer) {
			Log::Error("Renderer creation failed on window '{}': {}", m_name, SDL_GetError());
			return;
		}

		SetVsync(m_vsync);
	}

	void Window::DestroyRenderer() {
		if (m_sdlRenderer)
			SDL_DestroyRenderer(m_sdlRenderer);
		m_sdlRenderer = nullptr;
	}

	bool Window::HasWindow() const {
		return m_sdlWindow;
	}

	bool Window::HasRenderer() const {
		return m_sdlRenderer;
	}

	SDL_WindowFlags Window::GetWindowFlags() {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;

		return flags;
	}

	bool Window::SetVsync(int value) {
		if (value > 1)
			value = 1;
		if (value < -1)
			value = -1;

		if (value == m_vsync)
			return true;
		
		m_vsync = value;

		if (!m_sdlRenderer)
			return false;

		if (SDL_SetRenderVSync(m_sdlRenderer, value) < 0) {
		
			if (value == -1) {
				Log::Warn("Adaptive VSync not supported, falling back to normal VSync.");
				if (SDL_SetRenderVSync(m_sdlRenderer, 1) < 0) {
					Log::Error("Failed to set normal VSync for window '{}': {}", m_name, SDL_GetError());
					m_vsync = 0;
					return false;
				}
				m_vsync = 1;
				return true;
			}

			Log::Error("Failed to set VSync mode {} for window '{}': {}", value, m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	int Window::GetVsync() const {
		return m_vsync;
	}

}