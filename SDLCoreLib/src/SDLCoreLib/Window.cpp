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

		if (m_width < 0)
			m_width = 0;
		if (m_height < 0)
			m_height = 0;

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

	Window* Window::Show() {
		SDL_ShowWindow(m_sdlWindow.get());
		m_isVisible = true;
		return this;
	}

	Window* Window::Hide() {
		SDL_HideWindow(m_sdlWindow.get());
		m_isVisible = false;
		return this;
	}

	bool Window::HasWindow() const {
		return m_sdlWindow != nullptr;
	}

	bool Window::HasRenderer() const {
		return m_sdlRenderer != nullptr;
	}

	bool Window::IsVisible() const {
		return m_isVisible;
	}

	bool Window::IsFocused() const {
		return m_isFocused;
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

	void Window::CallOnWindowFocusGain() {
		CallCallbacks(m_onWinFocusGainCallbacks, *this);
	}

	void Window::CallOnWindowFocusLost() {
		CallCallbacks(m_onWinFocusLostCallbacks, *this);
	}

	SDL_WindowFlags Window::GetWindowFlags() const {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;
		if (m_transparentBuffer) flags |= SDL_WINDOW_TRANSPARENT;

		return flags;
	}

	void Window::SetWindowProperties() {
		if (!m_sdlWindow)
			return;

		(m_isVisible) ?
			SDL_ShowWindow(m_sdlWindow.get()) :
			SDL_HideWindow(m_sdlWindow.get());

		SDL_SetWindowResizable(m_sdlWindow.get(), m_resizable);
		SDL_SetWindowAlwaysOnTop(m_sdlWindow.get(), m_alwaysOnTop);
		SDL_SetWindowOpacity(m_sdlWindow.get(), m_opacity);
		SDL_SetWindowAspectRatio(m_sdlWindow.get(), m_minAspectRatio, m_maxAspectRatio);

		SDL_SetWindowMinimumSize(m_sdlWindow.get(), static_cast<int>(m_minSize.x), static_cast<int>(m_minSize.y));
		SDL_SetWindowMaximumSize(m_sdlWindow.get(), static_cast<int>(m_maxSize.x), static_cast<int>(m_maxSize.y));

		SetWindowPosInternal();
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

		auto now = Time::GetFrameCount();
		if (m_positionFetchedTime == now)
			return;

		m_positionFetchedTime = now;
		SDL_GetWindowPosition(m_sdlWindow.get(), &m_positionX, &m_positionY);
	}

	void Window::PollSize() const {
		if (!m_sdlWindow)
			return;

		auto now = Time::GetFrameCount();
		if (m_sizeFetchedTime == now)
			return;

		m_sizeFetchedTime = now;
		SDL_GetWindowSize(m_sdlWindow.get(), &m_width, &m_height);
	}

	void Window::SetWindowPosInternal() {
		if (!m_sdlWindow)
			return;
		SDL_SetWindowPosition(m_sdlWindow.get(),
			(m_positionX == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionX,
			(m_positionY == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionY);
	}

	void Window::UpdateWindowEvents(Uint32 type) {
		switch (type) {
		case SDL_EVENT_WINDOW_RESIZED:
			CallOnWindowResize();
			break;
		case SDL_EVENT_WINDOW_MINIMIZED:
			m_state = WindowState::MINIMIZED;
			break;
		case SDL_EVENT_WINDOW_MAXIMIZED:
			m_state = WindowState::MAXIMIZED;
			break;
		case SDL_EVENT_WINDOW_RESTORED:
			m_state = WindowState::NORMAL;
			break;
		case SDL_EVENT_WINDOW_SHOWN:
			m_isVisible = true;
			break;
		case SDL_EVENT_WINDOW_HIDDEN:
			m_isVisible = false;
			break;
		case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
			m_state = WindowState::FULLSCREEN_EXCLUSIVE;
			m_isVisible = true;
			break;
		case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
			m_state = WindowState::NORMAL;
			m_isVisible = true;
			break;
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			m_isFocused = true;
			CallOnWindowFocusGain();
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			m_isFocused = false;
			CallOnWindowFocusLost();
			break;
		default:
			break;
		}
	}

	int Window::GetVsync() const {
		return m_vsync;
	}

	float Window::GetAspectRatioMin() const {
		return m_minAspectRatio;
	}

	float Window::GetAspectRatioMax() const {
		return m_maxAspectRatio;
	}

	Vector2 Window::GetWindowMinSize() const {
		return m_minSize;
	}
	
	Vector2 Window::GetWindowMaxSize() const {
		return m_maxSize;
	}

	WindowState Window::GetState() const {
		return m_state;
	}

	DisplayID Window::GetDisplayID() const {
		return SDL_GetDisplayForWindow(m_sdlWindow.get());
	}

	Window* Window::SetName(const std::string& name) {
		m_name = name;

		if (m_sdlWindow) {
			SDL_SetWindowTitle(m_sdlWindow.get(), m_name.c_str());
		}
		return this;
	}

	Window* Window::SetPositionHor(int hor) {
		m_positionX = hor;

		if (m_sdlWindow) {
			SetWindowPosInternal();
		}
		return this;
	}

	Window* Window::SetPositionVer(int ver) {
		m_positionY = ver;

		if (m_sdlWindow) {
			SetWindowPosInternal();
		}
		return this;
	}

	Window* Window::SetPosition(int hor, int ver) {
		m_positionX = hor;
		m_positionY = ver;

		if (m_sdlWindow) {
			SetWindowPosInternal();
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
			SDL_SetWindowSize(m_sdlWindow.get(), m_width, m_height);
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
		if (minAspectRatio < 0)
			minAspectRatio = 1;
		if (maxAspectRatio < 0)
			maxAspectRatio = 1;

		m_minAspectRatio = minAspectRatio;
		m_maxAspectRatio = maxAspectRatio;

		if (m_sdlWindow) {
			SDL_SetWindowAspectRatio(m_sdlWindow.get(), m_minAspectRatio, m_maxAspectRatio);
		}
		return this;
	}

	Window* Window::SetWindowMinMaxSize(int minSizeX, int minSizeY, int maxSizeX, int maxSizeY) {
		SetWindowMinSize(minSizeX, minSizeY);
		return SetWindowMaxSize(maxSizeX, maxSizeY);
	}

	Window* Window::SetWindowMinSize(int minSizeX, int minSizeY) {
		if (minSizeX < 0)
			minSizeX = 1;
		if (minSizeY < 0)
			minSizeY = 1;

		m_minSize.Set(static_cast<float>(minSizeX), static_cast<float>(minSizeY));
		if (m_sdlWindow) {
			SDL_SetWindowMinimumSize(m_sdlWindow.get(), minSizeX, minSizeY);
		}
		return this;
	}
		
	Window* Window::SetWindowMaxSize(int maxSizeX, int maxSizeY) {
		if (maxSizeX < 0)
			maxSizeX = 1;
		if (maxSizeY < 0)
			maxSizeY = 1;

		m_maxSize.Set(static_cast<float>(maxSizeX), static_cast<float>(maxSizeY));
		if (m_sdlWindow) {
			SDL_SetWindowMaximumSize(m_sdlWindow.get(), maxSizeX, maxSizeY);
		}
		return this;
	}

	Window* Window::SetState(WindowState state)
	{
		switch (state)
		{
		case WindowState::NORMAL:
			// Exit fullscreen (both exclusive + borderless)
			SDL_SetWindowFullscreen(m_sdlWindow.get(), false);

			// Restore size (undo maximize/minimize)
			SDL_RestoreWindow(m_sdlWindow.get());

			// Make sure the window is visible
			SDL_ShowWindow(m_sdlWindow.get());
			break;

		case WindowState::MINIMIZED:
			SDL_MinimizeWindow(m_sdlWindow.get());
			break;

		case WindowState::MAXIMIZED:
			SDL_MaximizeWindow(m_sdlWindow.get());
			break;

		case WindowState::FULLSCREEN_EXCLUSIVE:
		{
			SDL_DisplayID display = GetDisplayID();
			int modeCount = 0;
			SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(display, &modeCount);

			if (modes && modeCount > 0)
			{
				SDL_SetWindowFullscreenMode(m_sdlWindow.get(), modes[0]);
				SDL_SetWindowFullscreen(m_sdlWindow.get(), true);
			}

			SDL_free(modes);
			break;
		}

		case WindowState::FULLSCREEN_BORDERLESS:
			SDL_SetWindowFullscreenMode(m_sdlWindow.get(), nullptr);
			SDL_SetWindowFullscreen(m_sdlWindow.get(), true);
			break;
		}

		m_state = state;
		return this;
	}

	Window* Window::SetBufferTransparent(bool value) {
		m_transparentBuffer = value;
		return this;
	}

	WindowCallbackID Window::AddOnDestroy(VoidCallback&& cb) {
		return AddCallback<VoidCallback>(m_onDestroyCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnDestroy(WindowCallbackID id) {
		if (!RemoveCallback<VoidCallback>(m_onDestroyCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnDestroy: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnSDLWindowClose(VoidCallback&& cb) {
		return AddCallback<VoidCallback>(m_onSDLWindowCloseCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnSDLWindowClose(WindowCallbackID id) {
		if (!RemoveCallback<VoidCallback>(m_onSDLWindowCloseCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnSDLWindowClose: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnSDLRendererDestroy(VoidCallback&& cb) {
		return AddCallback<VoidCallback>(m_onSDLRendererDestroyCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnSDLRendererDestroy(WindowCallbackID id) {
		if (!RemoveCallback<VoidCallback>(m_onSDLRendererDestroyCallbacks, id))
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

	WindowCallbackID Window::AddOnWindowFocusGain(WinCallback&& cb) {
		return AddCallback<WinCallback>(m_onWinFocusGainCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnWindowFocusGain(WindowCallbackID id) {
		if (!RemoveCallback<WinCallback>(m_onWinFocusGainCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnWindowFocusGain: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

	WindowCallbackID Window::AddOnWindowFocusLost(WinCallback&& cb) {
		return AddCallback<WinCallback>(m_onWinFocusLostCallbacks, std::move(cb));
	}

	Window* Window::RemoveOnWindowFocusLost(WindowCallbackID id) {
		if (!RemoveCallback<WinCallback>(m_onWinFocusLostCallbacks, id))
			Log::Warn("SDLCore::Window::RemoveOnWindowFocusLost: No callback found with ID '{}', nothing was removed.", id);
		return this;
	}

}