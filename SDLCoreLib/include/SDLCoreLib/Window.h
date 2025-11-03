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
		WindowID GetID() const;

	private:
		Window(WindowID id);
		Window(WindowID id, const std::string& name, int width, int height);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		
		static std::unique_ptr<Window> CreateInstance(WindowID id);
		static std::unique_ptr<Window> CreateInstance(WindowID id, const std::string& name, int width, int height);

		/**
		* @brief creates a window
		*/
		void Create();

		WindowID m_id{ SDLCORE_INVALID_ID };
		std::string m_name = "Untitled";
		int m_width = 0;
		int m_height = 0;

		bool m_resizable = true;
		bool m_alwaysTop = false;
		bool m_borderless = false;

		SDL_Window* m_sdlWindow = nullptr;

		SDL_WindowFlags GetWindowFlags();
	};

}