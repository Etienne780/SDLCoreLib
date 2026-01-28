#pragma once
#include <string>
#include <memory>
#include <functional>
#include <SDL3/SDL.h>
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/FormatUtils.h>

#include "Types/TextureSurface.h"
#include "SDLCoreTypes.h"

namespace SDLCore {

	constexpr int WINDOWPOS_UNDEFINED = SDL_WINDOWPOS_UNDEFINED;
	constexpr int WINDOWPOS_CENTERED = SDL_WINDOWPOS_CENTERED;

	enum class WindowState {
		NORMAL,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN_EXCLUSIVE,
		FULLSCREEN_BORDERLESS
	};

	class Application;
	class Texture;

	/**
	* @brief Encapsulates an SDL window and its renderer, providing
	*        methods to create, destroy, and manipulate the window
	*        and renderer properties dynamically.
	*
	* This class manages an SDL_Window and SDL_Renderer pair, and
	* allows dynamic changes to the window's size, title, opacity,
	*/
	class Window {
	friend class Application;
	public:
		using VoidCallback = std::function<void()>;
		using WinCallback = std::function<void(Window& win)>;

		~Window();

		/**
		* @brief Creates the SDL window with current settings
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool CreateWindow();

		/**
		* @brief Destroys the associated SDL window and its renderer, releasing all related SDL resources.
		*
		* Note: This function does not delete or invalidate the Window object itself; it only destroys
		* the underlying SDL structures (SDL_Window and SDL_Renderer) managed by this object.
		*/
		void DestroyWindow();

		/**
		* @brief Creates the SDL renderer associated with this window
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool CreateRenderer();

		/**
		* @brief Destroys the SDL renderer and releases its resources
		*/
		void DestroyRenderer();

		/**
		* @brief Shows the window.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool Show();

		/**
		* @brief Hides the window.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool Hide();

		/**
		* @brief Enables or disables cursor locking for this window.
		* @param value When true, cursor locking is activated; when false, it is disabled.
		*
		* The lock state is forwarded to the Application instance, which manages the
		* underlying behavior (cursor grabbing, relative motion usage, and lock position).
		*/
		void LockCursor(bool value) const;

		/**
		* @brief Checks if the SDL window has been created and is valid
		* @return true if the window exists, false otherwise
		*/
		bool HasWindow() const;

		/**
		* @brief Checks if the SDL renderer has been created and is valid
		* @return true if the renderer exists, false otherwise
		*/
		bool HasRenderer() const;

		/**
		* @brief Checks whether the window is currently visible.
		* @return true if visible, false otherwise
		*/
		bool IsVisible() const;

		/**
		* @brief Checks whether the window currently has input focus.
		* @return true if focused, false otherwise
		*/
		bool IsFocused() const;

		/**
		* @brief Gets the internal ID of this window
		* @return The WindowID of this window
		*/
		WindowID GetID() const;

		/**
		* @brief Gets the SDL window ID
		* @return SDL_WindowID if the window exists, otherwise 0
		*/
		SDL_WindowID GetSDLID() const;

		/**
		* @brief Gets the SDL window
		* @return SDL_Window weak_ptr pointer
		*/
		std::weak_ptr<SDL_Window> GetSDLWindow();

		/**
		* @brief Gets the SDL Renderer of this window
		* @return SDL_Renderer weak_ptr pointer
		*/
		std::weak_ptr<SDL_Renderer> GetSDLRenderer();

		/**
		* @brief Gets the name/title of this window
		* @return Window name as std::string
		*/
		std::string GetName() const;

		/**
		* @brief Retrieves the current position of the window
		* @return Vector2 containing the X and Y position in pixels
		*
		* This function will poll the window position from SDL if it has not been fetched yet this frame.
		* Otherwise, it returns the cached position.
		*/
		Vector2 GetPosition() const;

		/**
		* @brief Retrieves the current horizontal position of the window.
		* @return The X coordinate in screen space, or -1 if the window handle is invalid.
		*
		* This function polls the window position for the current frame if needed.
		*/
		int GetHorizontalPos() const;

		/**
		* @brief Retrieves the current vertical position of the window.
		* @return The Y coordinate in screen space, or -1 if the window handle is invalid.
		*
		* This function polls the window position for the current frame if needed.
		*/
		int GetVerticalPos() const;

		/**
		* @brief Retrieves the current size of the window
		* @return Vector2 containing the width and height in pixels
		*
		* This function will poll the window size from SDL if it has not been fetched yet this frame.
		* Otherwise, it returns the cached size.
		*/
		Vector2 GetSize() const;

		/**
		* @brief Gets the current width of the window.
		* @return Width in pixels.
		*
		* Will poll the window size if it has not been updated this frame.
		*/
		int GetWidth() const;

		/**
		* @brief Gets the current height of the window.
		* @return Height in pixels.
		*
		* Will poll the window size if it has not been updated this frame.
		*/
		int GetHeight() const;

		/**
		* @brief Gets the current VSync setting of the renderer
		* @return -1 = adaptive, 0 = off, 1 = on
		*/
		int GetVsync() const;

		/**
		* @brief Gets the minimum allowed aspect ratio of the window.
		* @return Minimum aspect ratio (width / height) as a float.
		*/
		float GetAspectRatioMin() const;

		/**
		* @brief Gets the maximum allowed aspect ratio of the window.
		* @return Maximum aspect ratio (width / height) as a float.
		*/
		float GetAspectRatioMax() const;

		/**
		* @brief Gets the minimum size of the window.
		* @return Minimum size as a Vector2 (x = width, y = height).
		*/
		Vector2 GetWindowMinSize() const;

		/**
		* @brief Gets the maximum size of the window.
		* @return Maximum size as a Vector2 (x = width, y = height).
		*/
		Vector2 GetWindowMaxSize() const;

		/**
		* @brief Returns the current window state (normal, minimized, maximized, fullscreen etc.).
		* @return Current window state
		*/
		WindowState GetState() const;

		/**
		* @brief Returns whether mouse grabbing is enabled for this window.
		*
		* Mouse grabbing confines the cursor to the window until released.
		*
		* @return True if cursor grabbing is active, otherwise false.
		*/
		bool GetCursorGrab() const;

		/**
		* @brief Returns the display ID the window is currently assigned to.
		* @return DisplayID of the window, or 0 on failure
		*/
		DisplayID GetDisplayID() const;

		/**
		* @brief Returns the effective UI content scale of the window.
		* @return The content scaling factor, where 1.0 represents the baseline resolution (1920x1080).
		*/
		float GetContentScale() const;

		/**
		* @brief Returns the name of the renderer currently associated with this window.
		*
		* This function queries SDL for the name of the active renderer used by the window.
		* The returned name is a low-ASCII identifier such as "opengl", "direct3d12" or "metal".
		*
		* @return Name of the renderer, or an empty string if no renderer exists or
		*         the name could not be queried.
		*/
		std::string GetRendererName() const;

		// ======= Dynamically modifiable properties =======

		/**
		* @brief Sets the window title dynamically
		* @param name New window title
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetName(const std::string& name);

		/**
		* @brief Sets the window position using horizontal coordinates.
		* @param hor New horizontal position in screen coordinates.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetPositionHor(int hor);

		/**
		* @brief Sets the window position using vertical coordinates.
		* @param hor New horizontal position in screen coordinates.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetPositionVer(int ver);

		/**
		* @brief Sets the window position using horizontal and vertical coordinates.
		* @param hor New horizontal position in screen coordinates.
		* @param ver New vertical position in screen coordinates.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetPosition(int hor, int ver);

		/**
		* @brief Sets the window position using a 2D vector.
		* @param pos New position as a Vector2 (x = horizontal, y = vertical).
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetPosition(const Vector2& pos);

		/**
		* @brief Sets only the horizontal position of the window.
		* @param hor New horizontal position in screen coordinates.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetHorizontalPos(int hor);

		/**
		* @brief Sets only the vertical position of the window.
		* @param ver New vertical position in screen coordinates.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetVerticalPos(int ver);

		/**
		* @brief Sets both the window width and height.
		* @param width New window width in pixels (minimum 1).
		* @param height New window height in pixels (minimum 1).
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetSize(int width, int height);

		/**
		* @brief Sets both the window width and height using a 2D vector.
		* @param size New size as a Vector2 (x = width, y = height), each minimum 1.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetSize(const Vector2& size);

		/**
		* @brief Sets the window width dynamically
		* @param width New width in pixels (minimum 1)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWidth(int width);

		/**
		* @brief Sets the window height dynamically
		* @param height New height in pixels (minimum 1)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetHeight(int height);

		/**
		* @brief Sets whether the window is resizable
		* @param value true to make resizable, false otherwise
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetResizable(bool value);

		/**
		* @brief Sets whether the window should always stay on top
		* @param value true to keep on top, false otherwise
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetAlwaysOnTop(bool value);

		/**
		* @brief Sets the opacity of the window
		* @param opacity Value between 0.0 (transparent) and 1.0 (opaque)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetOpacity(float opacity);

		/**
		* @brief Sets a fixed aspect ratio for the window
		* @param aspectRatio Desired aspect ratio (width / height), 0 to disable
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetAspectRatio(float aspectRatio);

		/**
		* @brief Sets an aspect ratio range for the window
		* @param minAspectRatio Minimum allowed aspect ratio (0 to disable lower bound)
		* @param maxAspectRatio Maximum allowed aspect ratio (0 to disable upper bound)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetAspectRatio(float minAspectRatio, float maxAspectRatio);

		/**
		* @brief Sets both minimum and maximum allowed window size
		* @param minSizeX Minimum width in pixels (0 disables the limit)
		* @param minSizeY Minimum height in pixels (0 disables the limit)
		* @param maxSizeX Maximum width in pixels (0 disables the limit)
		* @param maxSizeY Maximum height in pixels (0 disables the limit)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWindowMinMaxSize(int minSizeX, int minSizeY, int maxSizeX, int maxSizeY);

		/**
		* @brief Sets the minimum allowed window size
		* @param minSizeX Minimum width in pixels (0 disables the limit)
		* @param minSizeY Minimum height in pixels (0 disables the limit)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWindowMinSize(int minSizeX, int minSizeY);

		/**
		* @brief Sets the maximum allowed window size
		* @param maxSizeX Maximum width in pixels (0 disables the limit)
		* @param maxSizeY Maximum height in pixels (0 disables the limit)
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWindowMaxSize(int maxSizeX, int maxSizeY);

		/**
		* @brief Requests a change of the window state.
		* @param state Desired window state
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetState(WindowState state);

		/**
		* @brief Sets the window icon using a Texture.

		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetIcon(const Texture& texture);

		/**
		* @brief Sets the window icon using a TextureSurface.
		*
		* This function sets the window's icon to the provided SDL surface.
		* If the surface contains alternate images for different display scales, they
		* will be used automatically.
		*
		* @param textureSurface The TextureSurface to use as the window icon.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetIcon(const TextureSurface& textureSurface);

		/**
		* @brief Enables or disables mouse grabbing for this window.
		*
		* When enabled, SDL confines the cursor to the window.
		* This requires a valid SDL window handle.
		* On failure, an internal error message is stored.
		*
		* @param value True to enable grabbing, false to disable.
		* @return true on success. Call SDLCore::GetError() for more information.
		*/
		bool SetCursorGrab(bool value);

		/**
		* @brief Sets the visibility state of the mouse cursor for this window.
		* @param visibility When true, the cursor will be hidden; when false, it will be shown.
		* @return True if the visibility update was successfully applied, false on SDL failure.
		*/
		bool SetCursorHidden(bool visibility);

		/**
		* @brief Subscribes a callback to be called when this window object is destryoed.
		*
		* The callback will be stored internally and invoked when the window object is destroyed
		* or explicitly closed. Multiple callbacks can be registered.
		*
		* @param cb The function or lambda to call on window close.
		* @return A unique WindowCallbackID that can be used to remove the callback later.
		*/
		WindowCallbackID AddOnDestroy(VoidCallback&& cb);

		/**
		* @brief Removes a previously registered close callback.
		*
		* Use the WindowCallbackID returned by AddOnDestroy to remove a specific callback.
		* If the ID is invalid or already removed, this function does nothing.
		*
		* @param id The unique ID of the callback to remove.
		* @return Pointer to this Window to allow method chaining.
		*/
		Window* RemoveOnDestroy(WindowCallbackID id);

		/**
		* @brief Subscribes a callback to be called when SDL window is closed.
		*
		* The callback will be stored internally and invoked when the SDL window is destroyed
		* or explicitly closed. Multiple callbacks can be registered.
		*
		* @param cb The function or lambda to call on window close.
		* @return A unique WindowCallbackID that can be used to remove the callback later.
		*/
		WindowCallbackID AddOnSDLWindowClose(VoidCallback&& cb);

		/**
		* @brief Removes a previously registered close callback.
		*
		* Use the WindowCallbackID returned by AddOnSDLWindowClose to remove a specific callback.
		* If the ID is invalid or already removed, this function does nothing.
		*
		* @param id The unique ID of the callback to remove.
		* @return Pointer to this Window to allow method chaining.
		*/
		Window* RemoveOnSDLWindowClose(WindowCallbackID id);

		/**
		* @brief Subscribes a callback triggered when the SDL renderer of this window is destroyed.
		*
		* The callback is invoked when the underlying SDL_Renderer is about to be released.
		* Multiple callbacks can be registered.
		*
		* @param cb The function or lambda to execute on SDL renderer destruction.
		* @return A unique WindowCallbackID used to remove the callback later.
		*/
		WindowCallbackID AddOnSDLRendererDestroy(VoidCallback&& cb);

		/**
		* @brief Removes a previously registered SDL-renderer-destroy callback.
		*
		* Use the WindowCallbackID returned from AddOnSDLRendererDestroy to remove a specific entry.
		* If the ID is invalid or the callback was already removed, no action is performed.
		*
		* @param id The unique identifier of the callback to remove.
		* @return Pointer to this Window for method chaining.
		*/
		Window* RemoveOnSDLRendererDestroy(WindowCallbackID id);

		/**
		* @brief Subscribes a callback triggered when the window is resized.
		*
		* The callback is called whenever the window's size changes, providing the ability
		* to react to layout adjustments, viewport updates, or related handling.
		* Multiple callbacks can be registered.
		*
		* @param cb The function or lambda to execute on window resize.
		* @return A unique WindowCallbackID used to remove the callback later.
		*/
		WindowCallbackID AddOnWindowResize(WinCallback&& cb);

		/**
		* @brief Removes a previously registered window-resize callback.
		*
		* Use the WindowCallbackID returned from AddOnWindowResize to remove a specific entry.
		* If the ID is invalid or already removed, the function performs no action.
		*
		* @param id The unique identifier of the callback to remove.
		* @return Pointer to this Window for method chaining.
		*/
		Window* RemoveOnWindowResize(WindowCallbackID id);

		/**
		* @brief Subscribes a callback triggered when the window gains input focus.
		*
		* This callback is invoked when the operating system notifies that this window
		* has become the active/focused window. Multiple callbacks may be registered.
		*
		* @param cb The function or lambda to execute when the window gains focus.
		* @return A unique WindowCallbackID that can be used to remove the callback later.
		*/
		WindowCallbackID AddOnWindowFocusGain(WinCallback&& cb);

		/**
		* @brief Removes a previously registered window-focus-gain callback.
		*
		* Use the WindowCallbackID returned from AddOnWindowFocusGain to remove a
		* specific entry. If the ID is invalid or the callback was already removed,
		* no action is taken.
		*
		* @param id The unique identifier of the callback to remove.
		* @return Pointer to this Window for method chaining.
		*/
		Window* RemoveOnWindowFocusGain(WindowCallbackID id);

		/**
		* @brief Subscribes a callback triggered when the window loses input focus.
		*
		* This callback is invoked when the operating system reports that this window
		* is no longer the focused/active window. Multiple callbacks may be registered.
		*
		* @param cb The function or lambda to execute when the window loses focus.
		* @return A unique WindowCallbackID that can be used to remove the callback later.
		*/
		WindowCallbackID AddOnWindowFocusLost(WinCallback&& cb);

		/**
		* @brief Removes a previously registered window-focus-lost callback.
		*
		* Use the WindowCallbackID returned from AddOnWindowFocusLost to remove a
		* specific entry. If the ID is invalid or the callback was already removed,
		* this function performs no action.
		*
		* @param id The unique identifier of the callback to remove.
		* @return Pointer to this Window for method chaining.
		*/
		Window* RemoveOnWindowFocusLost(WindowCallbackID id);

		// ======= Properties that require window recreation =======

		/**
		* @brief Sets whether the window is borderless (Require window recreation to take effect)
		* 
		* Note: Changing this setting requires the window to be recreated
		* for the effect to take place.
		* 
		* @param value true for borderless, false otherwise
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetBorderless(bool value);

		/**
		* @brief Sets whether the window buffer should be transparent.
		*
		* Note: Changing this setting requires the window to be recreated
		* for the effect to take place.
		*
		* @param value true to enable a transparent buffer, false to disable.
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetBufferTransparent(bool value);

	private:
		Window(WindowID id);
		Window(WindowID id, const std::string& name, int width, int height);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		
		template<typename CBType>
		struct WindowCallback {
			WindowCallbackID id;
			CBType cb;

			WindowCallback(WindowCallbackID _id, CBType _cb)
				: id(_id), cb(_cb) {
			}
		};

		/**
		* @brief Creates a new instance of Window
		* @param id Window ID
		* @param name Window title
		* @param width Window width
		* @param height Window height
		* @return Unique pointer to the new Window
		*/
		static std::unique_ptr<Window> CreateInstance(WindowID id, const std::string& name, int width, int height);

		// ======= Window properties =======
		WindowID m_id{ SDLCORE_INVALID_ID };
		SDL_DisplayID m_sdlDisplayID = 0;
		TextureSurface m_icon;
		std::string m_name = "Untitled";
		mutable int m_positionX = -1;	// < dosent get update automaticly
		mutable int m_positionY = -1;	// < dosent get update automaticly
		mutable int m_width = 1;		// < dosent get update automaticly
		mutable int m_height = 1;		// < dosent get update automaticly
		mutable uint64_t m_positionFetchedTime = 0;
		mutable uint64_t m_sizeFetchedTime = 0;

		WindowState m_state = WindowState::NORMAL;
		bool m_isFocused = false;
		bool m_isCursorHidden = false;
		bool m_resizable = true;
		bool m_alwaysOnTop = false;
		bool m_borderless = false;
		bool m_transparentBuffer = false;
		bool m_isVisible = true;
		bool m_cursorGrab = false;
		float m_contentScale = 1.0f; /**< UI scaling factor for this window. A value of 1.0 represents the default scale (1920x1080). */
		float m_opacity = 1.0f;/**< Is in the rang 0-1. Default is 1*/
		float m_minAspectRatio = 0.0f;/**< Is in width/height, 0 to disable. Default is 0*/
		float m_maxAspectRatio = 0.0f;/**< Is in width/height, 0 to disable. Default is 0*/
		Vector2 m_minSize{ 0.0f, 0.0f };/**< Is in px, 0 to disable. Default [0, 0]*/
		Vector2 m_maxSize{ 0.0f, 0.0f };/**< Is in px, 0 to disable. Default [0, 0]*/

		SDLCoreIDManager m_callbackIDManager;
		std::vector<WindowCallback<VoidCallback>> m_onDestroyCallbacks;
		std::vector<WindowCallback<VoidCallback>> m_onSDLWindowCloseCallbacks;
		std::vector<WindowCallback<VoidCallback>> m_onSDLRendererDestroyCallbacks;
		std::vector<WindowCallback<WinCallback>> m_onWinResizeCallbacks;
		std::vector<WindowCallback<WinCallback>> m_onWinFocusGainCallbacks;
		std::vector<WindowCallback<WinCallback>> m_onWinFocusLostCallbacks;

		// ======= Renderer properties =======
		int m_vsync = 0;
		std::shared_ptr<SDL_Window> m_sdlWindow = nullptr;
		std::shared_ptr<SDL_Renderer> m_sdlRenderer = nullptr;

		// Adds a callback of arbitrary type CBType
		// CBType must match the type stored in the corresponding vector
		template<typename CBType>
		WindowCallbackID AddCallback(std::vector<WindowCallback<CBType>>& callbacks, CBType cb) {
			WindowCallbackID id{ m_callbackIDManager.GetNewUniqueIdentifier() };
			callbacks.emplace_back(id, std::move(cb));
			return id;
		}

		// Removes a callback by ID from a vector of WindowCallback<CBType>
		template<typename CBType>
		bool RemoveCallback(std::vector<WindowCallback<CBType>>& callbacks,
			WindowCallbackID id) {
			if (id.IsInvalid())
				return true;
			size_t preSize = callbacks.size();

			callbacks.erase(
				std::remove_if(callbacks.begin(), callbacks.end(),
					[id](const WindowCallback<CBType>& wc) {
						return wc.id == id;
					}),
				callbacks.end()
			);

			m_callbackIDManager.FreeUniqueIdentifier(id.value);
			return preSize != callbacks.size();
		}

		// Calls all callbacks and forwards arbitrary arguments to them
		template<typename CBType, typename... Args>
		void CallCallbacks(const std::vector<WindowCallback<CBType>>& callbacks, Args&&... args) {
			for (auto& windowCallback : callbacks) {
				// Check that the callback is valid
				if (windowCallback.cb) {
					// Forward all parameters to the callback
					windowCallback.cb(std::forward<Args>(args)...);
				}
			}
		}

		void CallOnDestroy();
		void CallOnSDLWindowClose();
		void CallOnSDLRendererDestroy();
		void CallOnWindowResize();
		void CallOnWindowFocusGain();
		void CallOnWindowFocusLost();
		
		/**
		* @brief Polls and updates the cached window position for the current frame
		*
		* Only queries SDL_GetWindowPosition if the position has not been fetched this frame.
		*/
		void PollPosition() const;

		/**
		* @brief Polls and updates the cached window size for the current frame
		*
		* Only queries SDL_GetWindowSize if the size has not been fetched this frame.
		*/
		void PollSize() const;

		/*
		* @brief called in application class
		*/
		void UpdateWindowEvents(Uint32 type);

		/*
		* @brief Updates Shows/hiddes the cursor
		*/
		bool UpdateCursorVisibility() const;

		/**
		* @brief Gets SDL window flags based on current settings
		* @return SDL_WindowFlags
		*/
		SDL_WindowFlags GetWindowFlags() const;

		/**
		* @brief Sets additional SDL window properties after creation
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWindowProperties();

		/**
		* @brief Sets the VSync mode of this window's renderer
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetVsync(int value);

		/*
		* @brief sets the window position with the current x and y pos
		* @return true on success. Call SDLCore::GetError() for more information
		*/
		bool SetWindowPosInternal();
	};

}

template<>
static inline std::string FormatUtils::toString<SDLCore::WindowState>(SDLCore::WindowState state) {
	switch (state) {
	case SDLCore::WindowState::NORMAL:					return "Normal";
	case SDLCore::WindowState::MINIMIZED:				return "Minimized";
	case SDLCore::WindowState::MAXIMIZED:				return "Maximized";
	case SDLCore::WindowState::FULLSCREEN_EXCLUSIVE:	return "FullscreenExclusive";
	case SDLCore::WindowState::FULLSCREEN_BORDERLESS:	return "FullscreenBorderless";
	default:											return "UNKOWN";
	}
}