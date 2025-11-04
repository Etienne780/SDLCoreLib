#pragma once
#include <string>
#include <memory>

#include <SDL3/SDL.h>
#include "SDLCoreTypes.h"

namespace SDLCore {

	class Application;
	class Window {
	friend class Application;
	public:
		~Window();

		/**
		* @brief Creates a window
		*/
		void CreateWindow();

		/**
		* @brief Destroys a window
		*/
		void DestroyWindow();

		/**
		* @brief Destroys a window
		*/
		void CreateRenderer();

		/**
		* @brief Destroys a window
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
		* @brief Gets the id of this window
		*/
		WindowID GetID() const;

		/**
		* @brief Gets the SDL ID of this window if it exits
		*/
		SDL_WindowID GetSDLID() const;

		/**
		* @brief Gets the name of this window
		* @return The name of this window as a string
		*/
		std::string GetName() const;

		/**
		* @brief Gets the width of this window
		* @return The width of this window as a int
		*/
		int GetWidth() const;

		/**
		* @brief Gets the height of this window
		* @return The height of this window as a int
		*/
		int GetHeight() const;

		/**
		* @brief Gets the current VSync setting.
		* @return -1 = adaptive, 0 = off, 1 = on.
		*/
		int GetVsync() const;

	private:
		Window(WindowID id);
		Window(WindowID id, const std::string& name, int width, int height);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		
		static std::unique_ptr<Window> CreateInstance(WindowID id);
		static std::unique_ptr<Window> CreateInstance(WindowID id, const std::string& name, int width, int height);

		WindowID m_id{ SDLCORE_INVALID_ID };
		std::string m_name = "Untitled";
		int m_width = 0;
		int m_height = 0;

		// ========== window ==========
		bool m_resizable = true;
		bool m_alwaysTop = false;
		bool m_borderless = false;

		// ========== renderer ==========
		int m_vsync = true;

		SDL_Window* m_sdlWindow = nullptr;
		SDL_Renderer* m_sdlRenderer = nullptr;

		SDL_WindowFlags GetWindowFlags();

		/**
		* @brief Sets VSync mode for this window’s renderer.
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		* @return true on success, false on failure (check SDL_GetError()).
		*/
		bool SetVsync(int value);
	};

}