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
		SDL_DestroyRenderer(m_sdlRenderer.get());
		SDL_DestroyWindow(m_sdlWindow.get());

		m_sdlRenderer.reset();
		m_sdlWindow.reset();
	}

	WindowID Window::GetID() const {
		return m_id;
	}

	SDL_WindowID Window::GetSDLID() const {
		return m_sdlWindow ? SDL_GetWindowID(m_sdlWindow.get()) : SDLCORE_INVALID_ID;
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

	std::unique_ptr<Window> Window::CreateInstance(WindowID id, const std::string& name, int width, int height) {
		return std::unique_ptr<Window>(new Window(id, name, width, height));
	}

	void Window::CreateWindow() {
		if (m_width < 0) m_width = 0;
		if (m_height < 0) m_height = 0;

		SDL_Window* rawWindow = SDL_CreateWindow(m_name.c_str(), m_width, m_height, GetWindowFlags());

		if (!rawWindow) {
			Log::Error("SDLCore::Window::CreateWindow: Failed to create window '{}': {}", m_name, SDL_GetError());
			return;
		}

		m_sdlWindow = std::shared_ptr<SDL_Window>(rawWindow, [](SDL_Window*) {});
		SetWindowProperties();
	}

	void Window::DestroyWindow() {
		CallOnClose();
		DestroyRenderer();
		if (m_sdlWindow) {
			SDL_DestroyWindow(m_sdlWindow.get());
			m_sdlWindow.reset();
		}
	}

	void Window::CreateRenderer() {
		SDL_Renderer* rawRenderer = SDL_CreateRenderer(m_sdlWindow.get(), nullptr);
		if (!rawRenderer) {
			Log::Error("SDLCore::Window::CreateRenderer: Renderer creation failed on window '{}': {}", m_name, SDL_GetError());
			return;
		}

		m_sdlRenderer = std::shared_ptr<SDL_Renderer>(rawRenderer, [](SDL_Renderer*) {});
		SetVsync(m_vsync);
	}

	void Window::DestroyRenderer() {
		if (m_sdlRenderer) {
			SDL_DestroyRenderer(m_sdlRenderer.get());
			m_sdlRenderer.reset();
		}
	}


	bool Window::HasWindow() const {
		return m_sdlWindow != nullptr;
	}

	bool Window::HasRenderer() const {
		return m_sdlRenderer != nullptr;
	}

	std::shared_ptr<SDL_Window> Window::GetSDLWindow() {
		if (!m_sdlWindow) {
			return nullptr;
		}
		return m_sdlWindow;
	}

	std::shared_ptr<SDL_Renderer> Window::GetSDLRenderer() {
		if (!m_sdlRenderer) {
			return nullptr;
		}
		return m_sdlRenderer;
	}

	void Window::CallOnClose() {
		if (m_onClose)
			m_onClose();
	}

	SDL_WindowFlags Window::GetWindowFlags() {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;

		return flags;
	}

	void Window::SetWindowProperties() {
		SDL_SetWindowOpacity(m_sdlWindow.get(), m_opacity);
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

		if (SDL_SetRenderVSync(m_sdlRenderer.get(), value) < 0) {
		
			if (value == -1) {
				Log::Warn("SDLCore::Window::SetVsync: Adaptive VSync not supported, falling back to normal VSync.");
				if (SDL_SetRenderVSync(m_sdlRenderer.get(), 1) < 0) {
					Log::Error("SDLCore::Window::SetVsync: Failed to set normal VSync for window '{}': {}", m_name, SDL_GetError());
					m_vsync = 0;
					return false;
				}
				m_vsync = 1;
				return true;
			}

			Log::Error("SDLCore::Window::SetVsync: Failed to set VSync mode {} for window '{}': {}", value, m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	int Window::GetVsync() const {
		return m_vsync;
	}

	Window* Window::SetName(const std::string& name) {
		m_name = name;

		if (m_sdlWindow) {
			SDL_SetWindowTitle(m_sdlWindow.get(), m_name.c_str());
		}

		return this;
	}

	Window* Window::SetWidth(int width) {
		if (width <= 0)
			width = 1;
		m_width = width;

		if (m_sdlWindow) {
			SDL_SetWindowSize(m_sdlWindow.get(), m_width, m_height);
		}

		return this;
	}

	Window* Window::SetHeight(int height) {
		if (height <= 0)
			height = 1;
		m_height = height;

		if (m_sdlWindow) {
			SDL_SetWindowSize(m_sdlWindow.get(), m_width, m_height);
		}

		return this;
	}

	Window* Window::SetResizable(bool value) {
		m_resizable = value;

		if (m_sdlWindow) {
			SDL_SetWindowResizable(m_sdlWindow.get(), m_resizable);
		}

		return this;
	}

	Window* Window::SetAlwaysOnTop(bool value) {
		m_alwaysOnTop = value;

		if (m_sdlWindow) {
			SDL_SetWindowAlwaysOnTop(m_sdlWindow.get(), m_alwaysOnTop);
		}

		return this;
	}

	Window* Window::SetBorderless(bool value) {
		m_borderless = value;

		if (m_sdlWindow) {
			SDL_SetWindowBordered(m_sdlWindow.get(), m_borderless);
		}

		return this;
	}

	Window* Window::SetOpacity(float opacity) {
		if (opacity < 0)
			opacity = 0;
		if (opacity > 1)
			opacity = 1;

		m_opacity = opacity;

		if (m_sdlWindow) {
			SDL_SetWindowOpacity(m_sdlWindow.get(), m_opacity);
		}

		return this;
	}

	Window* Window::SetOnClose(Callback cb) {
		m_onClose = std::move(cb);
		return this;
	}

}