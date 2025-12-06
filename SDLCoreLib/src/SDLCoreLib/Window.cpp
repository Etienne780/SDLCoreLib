#include <CoreLib/Log.h>

#include "Types/Input/InputManager.h"
#include "SDLCoreError.h"
#include "SDLCoreTime.h"
#include "Types/Texture.h"
#include "Application.h"
#include "Window.h"

namespace SDLCore {

	Window::Window(WindowID id)
		: m_id(id) {
	}

	Window::Window(WindowID id, const std::string& name, int width, int height)
		: m_id(id), m_name(name), m_width(width), m_height(height) {
	}

	Window::~Window() {
		// DestroyWindow gets called in Application::DeleteWindow before this destructor gets called
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

	bool Window::CreateWindow() {
		if (m_sdlWindow) {
			DestroyWindow();
		}

		if (m_width < 0) m_width = 0;
		if (m_height < 0) m_height = 0;

		SDL_Window* rawWindow = SDL_CreateWindow(m_name.c_str(), m_width, m_height, GetWindowFlags());
		if (!rawWindow) {
			SetErrorF("SDLCore::Window::CreateWindow: Failed to create window '{}': {}", m_name, SDL_GetError());
			return false;
		}

		m_sdlWindow = std::shared_ptr<SDL_Window>(rawWindow, [](SDL_Window*) {});

		if (!SetWindowProperties()) {
			AddErrorF("\nSDLCore::Window::CreateWindow: Failed to set window properties for '{}'", m_name);
			return false;
		}
		return true;
	}

	void Window::DestroyWindow() {
		CallOnSDLWindowClose();
		DestroyRenderer();
		if (m_sdlWindow) {
			SDL_DestroyWindow(m_sdlWindow.get());
			m_sdlWindow.reset();
		}
	}

	bool Window::CreateRenderer() {
		if (m_sdlRenderer) {
			DestroyRenderer();
		}

		if (!m_sdlWindow) {
			SetErrorF("SDLCore::Window::CreateRenderer: Renderer creation failed on window '{}', SDL window does not exist!", m_name);
			return false;
		}

		SDL_Renderer* rawRenderer = SDL_CreateRenderer(m_sdlWindow.get(), nullptr);
		if (!rawRenderer) {
			SetErrorF("SDLCore::Window::CreateRenderer: Renderer creation failed on window '{}': {}", m_name, SDL_GetError());
			return false;
		}

		m_sdlRenderer = std::shared_ptr<SDL_Renderer>(rawRenderer, [](SDL_Renderer*) {});

		if (!SetVsync(m_vsync)) {
			AddErrorF("\nSDLCore::Window::CreateRenderer: Failed to set VSync for window '{}'", m_name);
			return false;
		}

		return true;
	}

	void Window::DestroyRenderer() {
		if (m_sdlRenderer) {
			CallOnSDLRendererDestroy();
			SDL_DestroyRenderer(m_sdlRenderer.get());
			m_sdlRenderer = nullptr;
		}
	}

	bool Window::Show() {
		if (!m_sdlWindow) {
			SetError("SDLCore::Window::Show: SDL window is null, cannot show window!");
			return false;
		}

		if (!SDL_ShowWindow(m_sdlWindow.get())) {
			SetErrorF("SDLCore::Window::Show: Failed to show window '{}': {}", m_name, SDL_GetError());
			return false;
		}

		m_isVisible = true;
		return true;
	}

	bool Window::Hide() {
		if (!m_sdlWindow) {
			SetError("SDLCore::Window::Hide: SDL window is null, cannot hide window!");
			return false;
		}

		if (!SDL_HideWindow(m_sdlWindow.get())) {
			SetErrorF("SDLCore::Window::Hide: Failed to hide window '{}': {}", m_name, SDL_GetError());
			return false;
		}

		m_isVisible = false;
		return true;
	}

	void Window::LockCursor(bool value) const {
		auto* app = Application::GetInstance();
		if (!app)
			return;

		app->SetCursorLock(m_id, value);
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

		UpdateCursorVisibility();
	}

	bool Window::UpdateCursorVisibility() const {
		if (m_isFocused) {
			return (m_isCursorHiden) ?
				SDL_HideCursor() : SDL_ShowCursor();
		}
		else {
			SDL_ShowCursor();
		}
	}

	SDL_WindowFlags Window::GetWindowFlags() const {
		SDL_WindowFlags flags = 0;

		if (m_resizable) flags |= SDL_WINDOW_RESIZABLE;
		if (m_alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
		if (m_borderless) flags |= SDL_WINDOW_BORDERLESS;
		if (m_transparentBuffer) flags |= SDL_WINDOW_TRANSPARENT;

		return flags;
	}

	bool Window::SetWindowProperties() {
		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetWindowProperties: SDL window is null, cannot set properties!");
			return false;
		}
		SDL_Window* window = m_sdlWindow.get();

		// Show or hide the window
		if (m_isVisible) {
			SDL_ShowWindow(window);
		}
		else {
			SDL_HideWindow(window);
		}

		SDL_SetWindowResizable(window, m_resizable);
		SDL_SetWindowAlwaysOnTop(window, m_alwaysOnTop);
		SDL_SetWindowOpacity(window, m_opacity);
		SDL_SetWindowAspectRatio(window, m_minAspectRatio, m_maxAspectRatio);

		SDL_SetWindowMinimumSize(window,
			static_cast<int>(m_minSize.x),
			static_cast<int>(m_minSize.y));
		SDL_SetWindowMaximumSize(window,
			static_cast<int>(m_maxSize.x),
			static_cast<int>(m_maxSize.y));

		if (!m_icon.IsInvalid())
			SDL_SetWindowIcon(window, m_icon.GetSurface());
		SDL_SetWindowMouseGrab(window, m_cursorGrab);

		if (!SetWindowPosInternal()) {
			SetError("SDLCore::Window::SetWindowProperties: Failed to set window position!");
			return false;
		}

		return true;
	}

	bool Window::SetVsync(int value) {
		if (value > 1)
			value = 1;
		if (value < -1)
			value = -1;

		if (value == m_vsync)
			return true;

		m_vsync = value;

		if (!m_sdlRenderer) {
			SetError("SDLCore::Window::SetVsync: SDL renderer is null, cannot set VSync!");
			return false;
		}

		if (!SDL_SetRenderVSync(m_sdlRenderer.get(), value)) {
			if (value == -1) {
				Log::Warn("SDLCore::Window::SetVsync: Adaptive VSync not supported, falling back to normal VSync.");
				if (!SDL_SetRenderVSync(m_sdlRenderer.get(), 1)) {
					SetErrorF("SDLCore::Window::SetVsync: Failed to set normal VSync for window '{}': {}", m_name, SDL_GetError());
					m_vsync = 0;
					return false;
				}
				m_vsync = 1;
				return true;
			}

			SetErrorF("SDLCore::Window::SetVsync: Failed to set VSync mode {} for window '{}': {}", value, m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetWindowPosInternal() {
		if (!m_sdlWindow)
			return false;
		return SDL_SetWindowPosition(m_sdlWindow.get(),
			(m_positionX == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionX,
			(m_positionY == -1) ? SDL_WINDOWPOS_UNDEFINED : m_positionY);
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

	bool Window::GetCursorGrab() const {
		return m_cursorGrab;
	}

	DisplayID Window::GetDisplayID() const {
		return SDL_GetDisplayForWindow(m_sdlWindow.get());
	}

	bool Window::SetName(const std::string& name) {
		m_name = name;

		if (m_sdlWindow) {
			SDL_SetWindowTitle(m_sdlWindow.get(), m_name.c_str());
			return true;
		}
		else {
			SetError("SDLCore::Window::SetName: SDL window is null, cannot set window title!");
			return false;
		}
	}

	bool Window::SetPositionHor(int hor) {
		m_positionX = hor;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetPositionHor: SDL window is null, cannot set horizontal position!");
			return false;
		}

		if (!SetWindowPosInternal()) {
			SetErrorF("SDLCore::Window::SetPositionHor: Failed to set horizontal position for window '{}'", m_name);
			return false;
		}

		return true;
	}

	bool Window::SetPositionVer(int ver) {
		m_positionY = ver;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetPositionVer: SDL window is null, cannot set vertical position!");
			return false;
		}

		if (!SetWindowPosInternal()) {
			SetErrorF("SDLCore::Window::SetPositionVer: Failed to set vertical position for window '{}'", m_name);
			return false;
		}

		return true;
	}

	bool Window::SetPosition(int hor, int ver) {
		m_positionX = hor;
		m_positionY = ver;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetPosition: SDL window is null, cannot set position!");
			return false;
		}

		if (!SetWindowPosInternal()) {
			SetErrorF("SDLCore::Window::SetPosition: Failed to set position for window '{}'", m_name);
			return false;
		}

		return true;
	}

	bool Window::SetPosition(const Vector2& pos) {
		return SetPosition(static_cast<int>(pos.x), static_cast<int>(pos.y));
	}

	bool Window::SetHorizontalPos(int hor) {
		return SetPosition(hor, m_positionY);
	}

	bool Window::SetVerticalPos(int ver) {
		return SetPosition(m_positionX, ver);
	}

	bool Window::SetSize(int width, int height) {
		if (width <= 0) 
			width = 1;
		if (height <= 0) 
			height = 1;

		m_width = width;
		m_height = height;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetSize: SDL window is null, cannot set size!");
			return false;
		}

		if (!SDL_SetWindowSize(m_sdlWindow.get(), m_width, m_height)) {
			SetErrorF("SDLCore::Window::SetSize: Failed to set size for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetSize(const Vector2& size) {
		return SetSize(static_cast<int>(size.x), static_cast<int>(size.y));
	}

	bool Window::SetWidth(int width) {
		return SetSize(width, m_height);
	}

	bool Window::SetHeight(int height) {
		return SetSize(m_width, height);
	}

	bool Window::SetResizable(bool value) {
		m_resizable = value;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetResizable: SDL window is null, cannot change resizable state!");
			return false;
		}

		if (!SDL_SetWindowResizable(m_sdlWindow.get(), m_resizable)) {
			SetErrorF("SDLCore::Window::SetResizable: Failed to set resizable state for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetAlwaysOnTop(bool value) {
		m_alwaysOnTop = value;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetAlwaysOnTop: SDL window is null, cannot change always-on-top state!");
			return false;
		}

		if (!SDL_SetWindowAlwaysOnTop(m_sdlWindow.get(), m_alwaysOnTop)) {
			SetErrorF("SDLCore::Window::SetAlwaysOnTop: Failed to set always-on-top for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetOpacity(float opacity) {
		if (opacity < 0) 
			opacity = 0;
		if (opacity > 1) 
			opacity = 1;

		m_opacity = opacity;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetOpacity: SDL window is null, cannot set opacity!");
			return false;
		}

		if (!SDL_SetWindowOpacity(m_sdlWindow.get(), m_opacity)) {
			SetErrorF("SDLCore::Window::SetOpacity: Failed to set opacity for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetAspectRatio(float aspectRatio) {
		return SetAspectRatio(aspectRatio, aspectRatio);
	}

	bool Window::SetAspectRatio(float minAspectRatio, float maxAspectRatio) {
		if (minAspectRatio < 0) 
			minAspectRatio = 1;
		if (maxAspectRatio < 0) 
			maxAspectRatio = 1;

		m_minAspectRatio = minAspectRatio;
		m_maxAspectRatio = maxAspectRatio;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetAspectRatio: SDL window is null, cannot set aspect ratio!");
			return false;
		}

		if (!SDL_SetWindowAspectRatio(m_sdlWindow.get(), m_minAspectRatio, m_maxAspectRatio)) {
			SetErrorF("SDLCore::Window::SetAspectRatio: Failed to set aspect ratio for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetWindowMinSize(int minSizeX, int minSizeY) {
		if (minSizeX < 0) 
			minSizeX = 1;
		if (minSizeY < 0) 
			minSizeY = 1;

		m_minSize.Set(static_cast<float>(minSizeX), static_cast<float>(minSizeY));

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetWindowMinSize: SDL window is null, cannot set minimum size!");
			return false;
		}

		if (!SDL_SetWindowMinimumSize(m_sdlWindow.get(), minSizeX, minSizeY)) {
			SetErrorF("SDLCore::Window::SetWindowMinSize: Failed to set minimum size for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetWindowMaxSize(int maxSizeX, int maxSizeY) {
		if (maxSizeX < 0) 
			maxSizeX = 1;
		if (maxSizeY < 0) 
			maxSizeY = 1;

		m_maxSize.Set(static_cast<float>(maxSizeX), static_cast<float>(maxSizeY));

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetWindowMaxSize: SDL window is null, cannot set maximum size!");
			return false;
		}

		if (!SDL_SetWindowMaximumSize(m_sdlWindow.get(), maxSizeX, maxSizeY)) {
			SetErrorF("SDLCore::Window::SetWindowMaxSize: Failed to set maximum size for window '{}': {}",
				m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetWindowMinMaxSize(int minSizeX, int minSizeY, int maxSizeX, int maxSizeY) {
		bool success = true;
		if (!SetWindowMinSize(minSizeX, minSizeY)) success = false;
		if (!SetWindowMaxSize(maxSizeX, maxSizeY)) success = false;
		return success;
	}

	bool Window::SetState(WindowState state) {
		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetState: SDL window is null, cannot change window state!");
			return false;
		}

		switch (state) {
		case WindowState::NORMAL:
			SDL_SetWindowFullscreen(m_sdlWindow.get(), false);
			SDL_RestoreWindow(m_sdlWindow.get());
			SDL_ShowWindow(m_sdlWindow.get());
			break;

		case WindowState::MINIMIZED:
			SDL_MinimizeWindow(m_sdlWindow.get());
			break;

		case WindowState::MAXIMIZED:
			SDL_MaximizeWindow(m_sdlWindow.get());
			break;

		case WindowState::FULLSCREEN_EXCLUSIVE: {
			SDL_DisplayID display = GetDisplayID();
			int modeCount = 0;
			SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(display, &modeCount);

			if (!modes || modeCount == 0) {
				SetErrorF("SDLCore::Window::SetState: No fullscreen modes available for window '{}'", m_name);
				return false;
			}

			if (SDL_SetWindowFullscreenMode(m_sdlWindow.get(), modes[0]) != 0 ||
				SDL_SetWindowFullscreen(m_sdlWindow.get(), true) != 0) {
				SetErrorF("SDLCore::Window::SetState: Failed to set exclusive fullscreen for window '{}': {}",
					m_name, SDL_GetError());
				SDL_free(modes);
				return false;
			}

			SDL_free(modes);
			break;
		}

		case WindowState::FULLSCREEN_BORDERLESS:
			if (SDL_SetWindowFullscreenMode(m_sdlWindow.get(), nullptr) != 0 ||
				SDL_SetWindowFullscreen(m_sdlWindow.get(), true) != 0) {
				SetErrorF("SDLCore::Window::SetState: Failed to set borderless fullscreen for window '{}': {}",
					m_name, SDL_GetError());
				return false;
			}
			break;
		}

		m_state = state;
		return true;
	}

	bool Window::SetIcon(const Texture& texture) {
		return SetIcon(texture.GetSurface());
	}

	bool Window::SetIcon(const TextureSurface& textureSurface) {
		m_icon = textureSurface;
		// icon can be set before window creation
		if (!m_sdlWindow)
			return true;

		if (!m_icon.IsInvalid()) {
			if (!SDL_SetWindowIcon(m_sdlWindow.get(), m_icon.GetSurface())) {
				SetErrorF("SDLCore::Window::SetIcon: Failed to set window icon: {}", SDL_GetError());
				return false;
			}
		}

		return true;
	}

	bool Window::SetCursorGrab(bool value) {
		m_cursorGrab = value;
		if (!m_sdlWindow)
			return true;
	
		if (!SDL_SetWindowMouseGrab(m_sdlWindow.get(), value)) {
			SetErrorF("SDLCore::Window::SetCursorGrab: Failed to set window cursor grab: {}", SDL_GetError());
			return false;
		}
		return true;
	}

	bool Window::SetCursorHiden(bool visibility) {
		m_isCursorHiden = visibility;

		if (!UpdateCursorVisibility()) {
			SetErrorF("SDLCore::Window::SetCursorHidden: Failed to set cursor hidden: {}", SDL_GetError());
			return false;
		}
		return true;
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

	bool Window::SetBorderless(bool value) {
		m_borderless = value;

		if (!m_sdlWindow) {
			SetError("SDLCore::Window::SetBorderless: SDL window is null, cannot set borderless state!");
			return false;
		}

		if (!SDL_SetWindowBordered(m_sdlWindow.get(), !m_borderless)) {
			SetErrorF("SDLCore::Window::SetBorderless: Failed to set borderless mode for window '{}': {}", m_name, SDL_GetError());
			return false;
		}

		return true;
	}

	bool Window::SetBufferTransparent(bool value) {
		m_transparentBuffer = value;
		return true;
	}

}