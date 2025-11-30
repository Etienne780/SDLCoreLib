#include <CoreLib/Log.h>
#include "CoreTime.h"
#include "Window.h"

namespace SDLCore {

	Window::Window(WindowID id)
		: m_id(id) {
	}

	Window::Window(WindowID id, const std::string& name, int width, int height)
		: m_id(id), m_name(name), m_width(width), m_height(height) {
	}

	Window::~Window() {
		// DestroyWindow gets called in Application::RemoveWindow before this destructor gets called
		// DestroyWindow();
		CallOnDestroy();
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
	
	Vector2 Window::GetPosition() const {
		PollPosition();
		return Vector2(static_cast<float>(m_positionX), static_cast<float>(m_positionY));
	}

	int Window::GetHorizontalPos() const {
		PollPosition();
		return m_positionX;
	}

	int Window::GetVerticalPos() const {
		PollPosition();
		return m_positionY;
	}

	Vector2 Window::GetSize() const {
		PollSize();
		return Vector2(static_cast<float>(m_width), static_cast<float>(m_height));
	}

	int Window::GetWidth() const {
		PollSize();
		return m_width;
	}

	int Window::GetHeight() const {
		PollSize();
		return m_height;
	}

	std::unique_ptr<Window> Window::CreateInstance(WindowID id, const std::string& name, int width, int height) {
		return std::unique_ptr<Window>(new Window(id, name, width, height));
	}

	void Window::CreateWindow() {
		if (m_sdlWindow) {
			DestroyWindow();
		}

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
		CallOnSDLWindowClose();
		DestroyRenderer();
		if (m_sdlWindow) {
			SDL_DestroyWindow(m_sdlWindow.get());
			m_sdlWindow.reset();
		}
	}

	void Window::CreateRenderer() {
		if (m_sdlRenderer) {
			DestroyRenderer();
		}
		
		if (!m_sdlWindow) {
			Log::Error("SDLCore::Window::CreateRenderer: Renderer creation failed on window '{}', SDL window dident exist to create a renderer for!", m_name);
			return;
		}

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
			CallOnSDLRendererDestroy();
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

	std::weak_ptr<SDL_Window> Window::GetSDLWindow() {
		return m_sdlWindow;
	}

	std::weak_ptr<SDL_Renderer> Window::GetSDLRenderer() {
		return m_sdlRenderer;
	}

	void Window::CallOnDestroy() {
		CallCallbacks(m_onDestroyCallbacks);
	}

	void Window::CallOnSDLWindowClose() {
		CallCallbacks(m_onSDLWindowCloseCallbacks);
	}

	void Window::CallOnSDLRendererDestroy() {
		CallCallbacks(m_onSDLRendererDestroyCallbacks);
	}

	void Window::CallOnWindowResize() {
		PollSize();
		CallCallbacks(m_onWinResizeCallbacks, *this);
	}

	SDL_WindowFlags Window::GetWindowFlags() {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;

		return flags;
	}

	void Window::SetWindowProperties() {
		if (!m_sdlWindow)
			return;

		SDL_SetWindowOpacity(m_sdlWindow.get(), m_opacity);
		SDL_SetWindowAspectRatio(m_sdlWindow.get(), m_minAspectRatio, m_maxAspectRatio);
		
		SDL_SetWindowPosition(m_sdlWindow.get(), 
			(m_positionX == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionX,
			(m_positionY == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionY);
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

		if (!SDL_SetRenderVSync(m_sdlRenderer.get(), value)) {
		
			if (value == -1) {
				Log::Warn("SDLCore::Window::SetVsync: Adaptive VSync not supported, falling back to normal VSync.");
				if (!SDL_SetRenderVSync(m_sdlRenderer.get(), 1)) {
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

	void Window::PollPosition() const {
		if (!m_sdlWindow)
			return;

		auto now = Time::GetTime();
		if (m_positionFetchedTime == now)
			return;

		m_positionFetchedTime = now;
		SDL_GetWindowPosition(m_sdlWindow.get(), &m_positionX, &m_positionY);
	}

	void Window::PollSize() const {
		if (!m_sdlWindow)
			return;

		auto now = Time::GetTime();
		if (m_sizeFetchedTime == now)
			return;

		m_sizeFetchedTime = now;
		SDL_GetWindowSize(m_sdlWindow.get(), &m_width, &m_height);
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

	Window* Window::SetPosition(int hor, int ver) {
		m_positionX = hor;
		m_positionY = ver;

		if (m_sdlWindow) {
			SDL_SetWindowPosition(m_sdlWindow.get(), m_positionX, m_positionY);
		}

		return this;
	}

	Window* Window::SetPosition(const Vector2& pos) {
		return SetPosition(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	Window* Window::SetHorizontalPos(int hor) {
		return SetPosition(hor, m_positionY);
	}

	Window* Window::SetVerticalPos(int ver) {
		return SetPosition(m_positionX, ver);
	}

	Window* Window::SetSize(int width, int height) {
		if (width <= 0)
			width = 1;
		if (height <= 0)
			height = 1;
		m_width = width;
		m_height = height;

		if (m_sdlWindow) {
			SDL_SetWindowPosition(m_sdlWindow.get(), m_width, m_height);
		}

		return this;
	}

	Window* Window::SetSize(const Vector2& size) {
		return SetSize(static_cast<int>(size.x), static_cast<int>(size.y));
	}

	Window* Window::SetWidth(int width) {
		return SetSize(width, m_height);
	}

	Window* Window::SetHeight(int height) {
		return SetSize(m_width, height);
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

	Window* Window::SetAspectRatio(float aspectRatio) {
		return SetAspectRatio(aspectRatio, aspectRatio);
	}

	Window* Window::SetAspectRatio(float minAspectRatio, float maxAspectRatio) {
		if (minAspectRatio < 0 && minAspectRatio)
			minAspectRatio = 1;
		if (maxAspectRatio < 0 && maxAspectRatio)
			maxAspectRatio = 1;

		m_minAspectRatio = minAspectRatio;
		m_maxAspectRatio = maxAspectRatio;

		if (m_sdlWindow) {
			SDL_SetWindowAspectRatio(m_sdlWindow.get(), minAspectRatio, maxAspectRatio);
		}
		return this;
	}

	Window* Window::SetBufferTransparent(bool value) {
		
		return this;
	}

	WindowCallbackID Window::AddOnDestroy(Callback&& cb) {
		return AddCallback<Callback>(m_onDestroyCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnDestroy(WindowCallbackID id) {
		if (!RemoveCallback<Callback>(m_onDestroyCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnDestroy: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnSDLWindowClose(Callback&& cb) {
		return AddCallback<Callback>(m_onSDLWindowCloseCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnSDLWindowClose(WindowCallbackID id) {
		if (!RemoveCallback<Callback>(m_onSDLWindowCloseCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnSDLWindowClose: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnSDLRendererDestroy(Callback&& cb) {
		return AddCallback<Callback>(m_onSDLRendererDestroyCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnSDLRendererDestroy(WindowCallbackID id) {
		if (!RemoveCallback<Callback>(m_onSDLRendererDestroyCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnSDLRendererDestroy: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnWindowResize(WinCallback&& cb) {
		return AddCallback<WinCallback>(m_onWinResizeCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnWindowResize(WindowCallbackID id) {
		if (!RemoveCallback<WinCallback>(m_onWinResizeCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnWindowResize: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

}