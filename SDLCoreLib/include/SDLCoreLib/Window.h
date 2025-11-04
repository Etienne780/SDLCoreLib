#pragma once
#include <string>
#include <memory>
#include <functional>

#include <SDL3/SDL.h>
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

		~Window();

		/**
		* @brief Creates the SDL window with current settings
		*/
		void CreateWindow();

		/**
		* @brief Destroys the SDL window and releases its resources
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
		* @return SDL_Window shared pointer
		*/
		std::shared_ptr<SDL_Window> GetSDLWindow();

		/**
		* @brief Gets the SDL Renderer of this window
		* @return SDL_Renderer shared pointer
		*/
		std::shared_ptr<SDL_Renderer> GetSDLRenderer();

		/**
		* @brief Gets the name/title of this window
		* @return Window name as std::string
		*/
		std::string GetName() const;

		/**
		* @brief Gets the current width of the window
		* @return Width in pixels
		*/
		int GetWidth() const;

		/**
		* @brief Gets the current height of the window
		* @return Height in pixels
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
		* @brief Sets whether the window is borderless
		* @param value true for borderless, false otherwise
		* @return Pointer to this window (for chaining)
		*/
		Window* SetBorderless(bool value);

		/**
		* @brief Sets the opacity of the window
		* @param opacity Value between 0.0 (transparent) and 1.0 (opaque)
		* @return Pointer to this window (for chaining)
		*/
		Window* SetOpacity(float opacity);

		/**
		* @brief Subcribes to the close event of this window
		*/
		Window* SetOnClose(Callback cb);

		// ======= Properties that require window recreation =======

	private:
		Window(WindowID id);
		Window(WindowID id, const std::string& name, int width, int height);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		
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
		int m_width = 1;
		int m_height = 1;

		bool m_resizable = true;
		bool m_alwaysOnTop = false;
		bool m_borderless = false;
		float m_opacity = 1;

		Callback m_onClose = nullptr;

		// ======= Renderer properties =======
		int m_vsync = true;

		std::shared_ptr<SDL_Window> m_sdlWindow = nullptr;
		std::shared_ptr<SDL_Renderer> m_sdlRenderer = nullptr;

		void CallOnClose();
		
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
	};

}