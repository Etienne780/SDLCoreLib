#pragma once
#include <string>
#include <memory>
#include <functional>
#include <SDL3/SDL.h>
#include <CoreLib/Math/Vector2.h>

#include "IDManager.h"
#include "SDLCoreTypes.h"

namespace SDLCore {

	class Application;

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
		using Callback = std::function<void()>;
		using WinCallback = std::function<void(Window& win)>;

		~Window();

		/**
		* @brief Creates the SDL window with current settings
		*/
		void CreateWindow();

		/**
		* @brief Destroys the associated SDL window and its renderer, releasing all related SDL resources.
		*
		* Note: This function does not delete or invalidate the Window object itself; it only destroys
		* the underlying SDL structures (SDL_Window and SDL_Renderer) managed by this object.
		*/
		void DestroyWindow();

		/**
		* @brief Creates the SDL renderer associated with this window
		*/
		void CreateRenderer();

		/**
		* @brief Destroys the SDL renderer and releases its resources
		*/
		void DestroyRenderer();

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
		* @brief Gets the internal ID of this window
		* @return The WindowID of this window
		*/
		WindowID GetID() const;

		/**
		* @brief Gets the SDL window ID
		* @return SDL_WindowID if the window exists, otherwise SDLCORE_INVALID_ID
		*/
		SDL_WindowID GetSDLID() const;

		/**
		* @brief Gets the SDL window
		* @return SDL_Window weak pointer
		*/
		std::weak_ptr<SDL_Window> GetSDLWindow();

		/**
		* @brief Gets the SDL Renderer of this window
		* @return SDL_Renderer weak pointer
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

		// ======= Dynamically modifiable properties =======

		/**
		* @brief Sets the window title dynamically
		* @param name New window title
		* @return Pointer to this window (for chaining)
		*/
		Window* SetName(const std::string& name);

		/**
		* @brief Sets the window position using horizontal and vertical coordinates.
		* @param hor New horizontal position in screen coordinates.
		* @param ver New vertical position in screen coordinates.
		* @return Pointer to this window (for chaining).
		*/
		Window* SetPosition(int hor, int ver);

		/**
		* @brief Sets the window position using a 2D vector.
		* @param pos New position as a Vector2 (x = horizontal, y = vertical).
		* @return Pointer to this window (for chaining).
		*/
		Window* SetPosition(const Vector2& pos);

		/**
		* @brief Sets only the horizontal position of the window.
		* @param hor New horizontal position in screen coordinates.
		* @return Pointer to this window (for chaining).
		*/
		Window* SetHorizontalPos(int hor);

		/**
		* @brief Sets only the vertical position of the window.
		* @param ver New vertical position in screen coordinates.
		* @return Pointer to this window (for chaining).
		*/
		Window* SetVerticalPos(int ver);

		/**
		* @brief Sets both the window width and height.
		* @param width New window width in pixels (minimum 1).
		* @param height New window height in pixels (minimum 1).
		* @return Pointer to this window (for chaining).
		*/
		Window* SetSize(int width, int height);

		/**
		* @brief Sets both the window width and height using a 2D vector.
		* @param size New size as a Vector2 (x = width, y = height), each minimum 1.
		* @return Pointer to this window (for chaining).
		*/
		Window* SetSize(const Vector2& size);

		/**
		* @brief Sets the window width dynamically
		* @param width New width in pixels (minimum 1)
		* @return Pointer to this window (for chaining)
		*/
		Window* SetWidth(int width);

		/**
		* @brief Sets the window height dynamically
		* @param height New height in pixels (minimum 1)
		* @return Pointer to this window (for chaining)
		*/
		Window* SetHeight(int height);

		/**
		* @brief Sets whether the window is resizable
		* @param value true to make resizable, false otherwise
		* @return Pointer to this window (for chaining)
		*/
		Window* SetResizable(bool value);

		/**
		* @brief Sets whether the window should always stay on top
		* @param value true to keep on top, false otherwise
		* @return Pointer to this window (for chaining)
		*/
		Window* SetAlwaysOnTop(bool value);

		/**
		* @brief Sets the opacity of the window
		* @param opacity Value between 0.0 (transparent) and 1.0 (opaque)
		* @return Pointer to this window (for chaining)
		*/
		Window* SetOpacity(float opacity);

		Window* SetAspectRatio(float aspectRatio);
		Window* SetAspectRatio(float minAspectRatio, float maxAspectRatio);

		/**
		* @brief Subscribes a callback to be called when this window object is destryoed.
		*
		* The callback will be stored internally and invoked when the window object is destroyed
		* or explicitly closed. Multiple callbacks can be registered.
		*
		* @param cb The function or lambda to call on window close.
		* @return A unique WindowCallbackID that can be used to remove the callback later.
		*/
		WindowCallbackID AddOnDestroy(Callback&& cb);

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
		WindowCallbackID AddOnSDLWindowClose(Callback&& cb);

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
		WindowCallbackID AddOnSDLRendererDestroy(Callback&& cb);

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

		// ======= Properties that require window recreation =======

		/**
		* @brief Sets whether the window is borderless (Require window recreation to take effect)
		* @param value true for borderless, false otherwise
		* @return Pointer to this window (for chaining)
		*/
		Window* SetBorderless(bool value);

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
		std::string m_name = "Untitled";
		mutable int m_positionX = 0;	// < dosent get update automaticly
		mutable int m_positionY = 30;	// < dosent get update automaticly
		mutable int m_width = 1;		// < dosent get update automaticly
		mutable int m_height = 1;		// < dosent get update automaticly
		mutable Uint64 m_positionFetchedTime = 0;
		mutable Uint64 m_sizeFetchedTime = 0;

		bool m_resizable = true;
		bool m_alwaysOnTop = false;
		bool m_borderless = false;
		float m_opacity = 1;
		float m_minAspectRatio = 0;
		float m_maxAspectRatio = 0;

		IDManager m_callbackIDManager;
		std::vector<WindowCallback<Callback>> m_onDestroyCallbacks;
		std::vector<WindowCallback<Callback>> m_onSDLWindowCloseCallbacks;
		std::vector<WindowCallback<Callback>> m_onSDLRendererDestroyCallbacks;
		std::vector<WindowCallback<WinCallback>> m_onWinResizeCallbacks;

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
		
		/**
		* @brief Gets SDL window flags based on current settings
		* @return SDL_WindowFlags
		*/
		SDL_WindowFlags GetWindowFlags();

		/**
		* @brief Sets additional SDL window properties after creation
		*/
		void SetWindowProperties();

		/**
		* @brief Sets the VSync mode of this window's renderer
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		* @return true on success, false on failure (check SDL_GetError())
		*/
		bool SetVsync(int value);

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
	};

}