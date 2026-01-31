#pragma once
#include <string>
#include <vector>

#include "SDLCoreTypes.h"
#include "SDLCoreError.h"
#include "SDLCoreRenderer.h"
#include "SDLCoreTime.h"
#include "SDLCoreInput.h"
#include "types/Version.h"
#include "Window.h"

namespace SDLCore {

	class Window;

	class Application {
	friend class Window;
	public:
		Application(std::string& name, const Version& version);
		Application(std::string&& name, const Version& version);

		static Application* GetInstance();

		/**
		* @brief Returns whether the entire application is scheduled to quit.
		* @return true if the application shutdown has been requested, false otherwise
		*/
		static bool IsQuit();

		/**
		* @brief Returns the platform on which the application is currently running.
		*
		* This function queries the underlying SDL platform and maps it to the corresponding
		* SDLCore::Platform enum value. It can be used to implement platform-specific logic
		* within the application.
		*
		* @return SDLCore::Platform Enum value representing the current platform.
		*/
		static Platform GetPlatform();

		/**
		* @brief Starts the main loop of the application
		* @return returns an error code or 0
		*/
		SDLResult Start();

		/**
		* @brief Quits the application
		*/
		void Quit();

		/**
		* @brief Adds a new Window instance to the application without creating the underlying SDL window or renderer.
		*
		* This function allocates and registers a new Window object within the Application, but does not yet
		* initialize the SDL window or renderer. It is typically used when the window setup should be controlled
		* manually or deferred.
		*
		* @param idPtr Optional pointer to a WindowID variable that receives the unique ID of the new window.
		*              If provided, the ID will automatically be set to SDLCORE_INVALID_ID when the window closes.
		* @param name The name/title of the window.
		* @param width Desired window width in pixels.
		* @param height Desired window height in pixels.
		*
		* @return Raw pointer to the newly added Window instance, or nullptr if window creation failed.
		*/
		Window* AddWindow(WindowID* idPtr, std::string name, int width, int height);

		/**
		* @brief Creates and fully initializes a new SDL window and renderer.
		*
		* This function creates a new Window object, generates a unique WindowID, and immediately initializes
		* the underlying SDL window and renderer. It also automatically registers an on-close callback that
		* invalidates the provided WindowID pointer when the window is closed.
		*
		* @param idPtr Optional pointer to a WindowID variable that receives the unique ID of the new window.
		*              The ID will automatically be set to SDLCORE_INVALID_ID when the window is closed.
		* @param name The name/title of the window.
		* @param width Desired window width in pixels.
		* @param height Desired window height in pixels.
		*
		* @return Raw pointer to the newly created Window instance, or nullptr if window creation failed.
		*/
		Window* CreateWindow(WindowID* idPtr, const std::string& name, int width, int height);

		/**
		* @brief Deletes a window and releases all associated resources.
		*
		* This function destroys the specified window, including its underlying SDL window
		* and renderer, and removes it from the application's window list.
		*
		* @param id Unique identifier of the window to delete.
		* @return true if the window was found and deleted successfully, false otherwise.
		*/
		bool DeleteWindow(WindowID id);

		/**
		* @brief Deletes all windows managed by the application.
		*
		* This function destroys all existing windows, releases their associated SDL resources,
		* and clears the internal window list.
		*/
		void DeleteAllWindows();

		/**
		* @brief Recreates the renderer for a specific window.
		*
		* This function destroys the existing renderer (if any) associated with the specified
		* window and creates a new renderer using the currently selected render driver.
		*
		* @param id Unique identifier of the window whose renderer should be recreated.
		*/
		void RecreateRendererForWindow(WindowID id);

		/**
		* @brief Recreates the renderers for all existing windows.
		*
		* This function iterates over all windows managed by the application and recreates
		* their renderers using the currently selected render driver.
		*/
		void RecreateRenderersForAllWindows();

		/**
		* @brief Returns whether a window currently has cursor locking enabled.
		* @return True if cursor locking is active for a window, otherwise false.
		*/
		bool IsCursorLocked() const;

		/**
		* @brief Returns whether cursor locking is configured to auto-center on window resize.
		* @return True when lock position is automatically updated to the window center.
		*/
		bool IsCursorLockCenter() const;

		/**
		* @brief Gets a window with a given id
		* @param id to search for
		* @param raw ptr to the window or nullptr if not found
		*/
		Window* GetWindow(WindowID id);

		/*
		* @brief Gets the current amount of windows owned by this application
		* @return Amount of windows
		*/
		size_t GetWindowCount() const;

		/*
		* @brief Gets the name of the current application
		* @return Application name
		*/
		std::string GetName() const;

		/*
		* @brief Gets the Version of the current application
		* @return Application version
		*/
		Version GetVersion() const;

		/**
		* @brief Returns the window ID for which cursor locking is currently active.
		* @return The WindowID of the locked window, or an invalid ID if no lock is active.
		*/
		WindowID GetCursorLockWinID() const;

		/**
		* @brief Returns a user- and app-specific directory path for storing files.
		*
		* This function returns the "preference directory" where the application can
		* safely write user-specific files such as preferences or save games.
		* The directory is unique per user and per application. The wrapper converts
		* the SDL-allocated string to a SystemFilePath object and handles memory cleanup.
		*
		* Example paths:
		* 
		* - Windows: `C:\Users\Bob\AppData\Roaming\MyCompany\MyApp\`
		* 
		* - Linux:   `/home/bob/.local/share/MyApp/`
		* 
		* - macOS:   `/Users/bob/Library/Application Support/MyApp/`
		*
		* Rules:
		* 
		* - Provide a consistent organization name for all your apps.
		* 
		* - Use a unique application name.
		* 
		* - Only letters, numbers, and spaces; avoid punctuation.
		*
		* @param orgName Name of the organization (default: "DefaultCompany").
		* @return SystemFilePath representing the absolute, writable user directory.
		*         Returns an empty path if the directory could not be created.
		*/
		SystemFilePath GetPrefPath(const std::string& orgName = "DefaultCompany") const;

		/**
		* @brief Returns the directory where the application was executed from.
		*
		* This function wraps SDL_GetBasePath() and returns a SystemFilePath representing
		* the absolute path of the directory containing the application binary.
		* The path is guaranteed to end with a platform-specific separator ('\\' on Windows, '/' elsewhere).
		*
		* Notes:
		* 
		* - On macOS and iOS, if the application is inside a ".app" bundle, this function
		*   returns the Resource directory (e.g., MyApp.app/Contents/Resources/).
		*   This can be overridden via the SDL_FILESYSTEM_BASE_DIR_TYPE property in Info.plist.
		* 
		* - On platforms like Nintendo 3DS, this may return a read-only directory (e.g., "romfs").
		* 
		* - SDL caches the result internally; the first call may be slower.
		*
		* @return SystemFilePath representing the base directory of the application.
		*         Returns an empty path on error; GetError() contains the platform-specific reason.
		*
		* @threadsafety Safe to call from any thread.
		*/
		SystemFilePath GetBasePath() const;

		/**
		* @brief Returns the names of all available 2D rendering drivers.
		*
		* Queries SDL for all built-in rendering drivers and returns their names
		* in the order SDL would normally initialize them.
		*
		* @return std::vector<std::string> containing the names of the render drivers.
		*         Drivers that are unavailable (NULL) are omitted.
		*
		* @threadsafety Safe to call from any thread.
		*/
		std::vector<std::string> GetRenderDrivers() const;

		/**
		* @brief Returns the currently selected rendering driver.
		*
		* The returned string corresponds to the driver that will be used when
		* creating the SDL_Renderer for application windows.
		*
		* @return std::string containing the name of the current render driver.
		*/
		const std::string& GetCurrentRenderDriver() const;

		/**
		* @brief Returns the current clipboard text.
		*
		* @return Clipboard text as std::string.
		*         Returns an empty string if no text is available or on error.
		*
		* @threadsafety Safe to call from any thread.
		*/
		std::string GetClipboardText() const;

		/**
		* @brief Returns all available clipboard MIME types.
		*
		* @return Vector of MIME type strings.
		*
		* @threadsafety Safe to call from any thread.
		*/
		std::vector<std::string> GetClipboardMimeTypes() const;

		/**
		* @brief Retrieves raw clipboard data for a specific MIME type.
		*
		* Queries SDL for the clipboard content of the given MIME type.
		*
		* @param mimeType MIME type to query.
		* @param outSize Will be set to the size of the returned data in bytes.
		* @return Pointer to SDL-owned clipboard data, or nullptr if unavailable.
		*
		* @note The returned pointer is owned by SDL and valid only until the next
		*       clipboard operation. Do not attempt to free it.
		* @threadsafety Safe to call from any thread.
		*/
		const void* GetClipboardData(const std::string& mimeType, std::size_t& outSize) const;

		/**
		* @brief Retrieves clipboard data cast to a specific type.
		*
		* Returns a pointer to type T if the clipboard data size matches sizeof(T).
		*
		* @tparam T Type to cast the clipboard data to.
		* @param mimeType MIME type to query.
		* @param outSize Will be set to the size of the returned data in bytes if successful.
		* @return Pointer to T, or nullptr if size/type mismatch.
		* @threadsafety Safe to call from any thread.
		*/
		template<typename T>
		const T* GetClipboardData(const std::string& mimeType, size_t& outSize) const {
			std::size_t size = 0;
			const void* rawData = GetClipboardData(mimeType, size);
			if (rawData && size == sizeof(T)) {
				outSize = size;
				return static_cast<const T*>(rawData);
			}
			return nullptr;
		}

		/**
		* @brief Retrieves clipboard data cast to a specific type, ignoring size.
		*
		* Returns a pointer to type T if the clipboard data size matches sizeof(T).
		*
		* @tparam T Type to cast the clipboard data to.
		* @param mimeType MIME type to query.
		* @return Pointer to T, or nullptr if size/type mismatch.
		* @threadsafety Safe to call from any thread.
		*/
		template<typename T>
		const T* GetClipboardData(const std::string& mimeType) const {
			std::size_t size = 0;
			const void* rawData = GetClipboardData(mimeType, size);
			if (rawData && size == sizeof(T)) {
				return static_cast<const T*>(rawData);
			}
			return nullptr;
		}

		/**
		* @brief Container traits to extract element type for generic clipboard array access.
		*
		* Provides a generic way to deduce value_type from containers.
		*/
		template<typename Container>
		struct ContainerTraits {
			using value_type = typename Container::value_type;
			static constexpr bool is_fixed_size = false; ///< Indicates whether container size is fixed (e.g., std::array)
		};

		/**
		* @brief Specialization of ContainerTraits for std::array.
		*/
		template<typename T, std::size_t N>
		struct ContainerTraits<std::array<T, N>> {
			using value_type = T;
			static constexpr bool is_fixed_size = true;
		};

		/**
		* @brief Retrieves clipboard data for containers (std::vector, std::array, etc.).
		*
		* Returns a pointer to the underlying element type if the data size
		* is compatible (multiple of element size).
		*
		* @tparam Container The container type (must have value_type defined).
		* @param mimeType MIME type to query.
		* @param outSize Will be set to the total size in bytes of the returned data.
		* @return Pointer to the first element, or nullptr if size/type mismatch.
		* @threadsafety Safe to call from any thread.
		*/
		template<typename Container>
		const typename ContainerTraits<Container>::value_type* GetClipboardDataArray(
			const std::string& mimeType,
			std::size_t& outSize
		) const {
			static_assert(std::is_standard_layout_v<typename ContainerTraits<Container>::value_type>,
				"Clipboard template only works with standard-layout types");

			outSize = 0;
			const void* rawData = GetClipboardData(mimeType, outSize);
			if (!rawData)
				return nullptr;

			using Element = typename ContainerTraits<Container>::value_type;
			if (outSize % sizeof(Element) != 0)
				return nullptr;

			return static_cast<const Element*>(rawData);
		}

		/**
		* @brief Sets the application's frame rate cap or VSync mode.
		*
		* This function configures how the application's rendering loop limits its frame rate.
		* Depending on the value, it either enables vertical synchronization (VSync),
		* adaptive VSync, or sets a custom FPS cap. Negative special values are used
		* to represent different synchronization modes.
		*
		* @param value Frame cap mode or FPS limit.
		* 
		* - APPLICATION_FPS_UNCAPPED: No frame rate limit.
		* 
		* - APPLICATION_FPS_VSYNC_ON: Enables standard VSync.
		* 
		* - APPLICATION_FPS_VSYNC_ADAPTIVE_ON: Enables adaptive VSync.
		* 
		* - Any positive integer: Caps the frame rate to the specified FPS value.
		*/
		void SetFPSCap(int value);

		/**
		* @brief Enables or disables cursor locking for a specific window.
		* 
		* When cursor locking is active, the cursor is repositioned according to the lock configuration.
		* For input processing while the cursor is locked, relative mouse movement should be used
		* (e.g. SDL relative motion or Input::GetRelativePosition()), as absolute coordinates are not meaningful
		* during cursor lock.
		* 
		* @param winID Identifier of the target window.
		* @param active Enables cursor locking when true, disables when false.
		* @param center When true, the lock position is automatically updated to the window center.
		* @return True on successful configuration, false if the window does not exist.
		*/
		bool SetCursorLock(WindowID winID, bool active, bool center = true);

		/**
		* @brief Activates or deactivates the internal cursor-lock behavior for the currently locked window.
		* 
		* For input processing while cursor locking is active, relative mouse movement should be used
		* (e.g. SDL relative motion or engine-provided relative delta). Absolute mouse positions do not reflect
		* real cursor motion when locking is enabled.
		* 
		* @param active When true, cursor grab is enforced; when false, the previous grab state is restored.
		*/
		void SetCursorLockActive(bool active);

		/**
		* @brief Sets the horizontal lock position for the cursor.
		* @param x Desired lock position on the x-axis (clamped to >= 0).
		*/
		void SetCursorLockPosX(float x);

		/**
		* @brief Sets the vertical lock position for the cursor.
		* @param y Desired lock position on the y-axis (clamped to >= 0).
		*/
		void SetCursorLockPosY(float y);

		/**
		* @brief Sets the rendering driver to use for newly created window renderer.
		*
		* This function updates the internal driver selection. It dose not create
		* new renderers for the current windows
		*
		* @param driver Name of the rendering driver to use. If empty, SDL will choose
		*               the default driver automatically.
		*/
		void SetRenderDriver(const std::string& driver);

		/**
		* @brief Sets the clipboard text.
		*
		* @param text UTF-8 encoded text to place into the clipboard.
		* @return true on success, false on failure.
		*
		* @threadsafety Safe to call from any thread.
		*/
		bool SetClipboardText(const std::string& text) const;

		/**
		* @brief Sets custom clipboard data using SDL clipboard callbacks.
		*
		* Registers clipboard data that is provided on demand via SDL callbacks.
		* This is the low-level entry point used internally by higher-level
		* typed clipboard helper functions.
		*
		* The clipboard data may be exposed under multiple MIME types at once.
		*
		* note: The lifetime of the data referenced by userdata must exceed the
		*       lifetime of the clipboard ownership.
		* 
		* @param callback Callback invoked by SDL when clipboard data is requested.
		* @param cleanup Optional cleanup callback invoked by SDL when clipboard data is released.
		* @param userdata User-defined pointer passed to the callbacks.
		* @param mimeTypes List of MIME types under which the clipboard data is advertised.
		*
		* @return true if the clipboard data was successfully registered, false otherwise.
		*
		* @threadsafety Safe to call from any thread.
		*/
		bool SetClipboardData(
			SDL_ClipboardDataCallback callback,
			SDL_ClipboardCleanupCallback cleanup,
			void* userdata,
			const std::vector<std::string>& mimeTypes
		) const;
		
		/**
		* @brief Sets clipboard data for a single fixed-size value.
		*
		* Stores a trivially copyable value in the clipboard under one or more
		* MIME types. The data is exposed to SDL as a raw memory block of size
		* sizeof(T).
		*
		*  note: The referenced value must remain valid while the clipboard owns the data.
		* 
		* @tparam T Type of the value to store. Must be standard-layout.
		* @param value Value to expose via the clipboard.
		* @param mimeTypes List of MIME types identifying the clipboard data.
		*
		* @return true on success, false on failure.
		*
		* @threadsafety Safe to call from any thread.
		*/
		template<typename T>
		bool SetClipboardData(const T& value, const std::vector<std::string>& mimeTypes) const {
			static_assert(std::is_standard_layout_v<T>,
				"Clipboard data must be standard-layout");

			auto callback = [](void* userdata, const char* mime, size_t* outSize) -> const void* {
				if (!userdata || !outSize) return nullptr;
				*outSize = sizeof(T);
				return userdata;
			};

			return SetClipboardData(
				callback,
				nullptr,
				const_cast<T*>(&value),
				mimeTypes
			);
		}

		/**
		* @brief Sets clipboard data for a dynamic array of values.
		*
		* Stores the contents of a std::vector as a contiguous block of memory
		* in the clipboard. The clipboard data size is calculated as
		* values.size() * sizeof(T).
		*
		* The same data block can be exposed under multiple MIME types.
		*
		* note: Only the raw element data is exposed, not the std::vector object itself.
		*       The vector must remain valid while the clipboard owns the data.
		* 
		* @tparam T Element type of the vector. Must be standard-layout.
		* @param values Vector containing the data to expose.
		* @param mimeTypes List of MIME types identifying the clipboard data.
		*
		* @return true on success, false on failure.
		*
		* @threadsafety Safe to call from any thread.
		*/
		template<typename T>
		bool SetClipboardData(
			const std::vector<T>& values,
			const std::vector<std::string>& mimeTypes
		) const {
			static_assert(std::is_standard_layout_v<T>,
				"Clipboard vector elements must be standard-layout");

			auto callback = [](void* userdata, const char*, size_t* outSize) -> const void* {
				if (!userdata || !outSize) 
					return nullptr;

				const auto* vec = static_cast<const std::vector<T>*>(userdata);
				*outSize = vec->size() * sizeof(T);
				return vec->data();
			};

			return SetClipboardData(
				callback,
				nullptr,
				const_cast<std::vector<T>*>(&values),
				mimeTypes
			);
		}

		/**
		* @brief Sets clipboard data for a fixed-size array of values.
		*
		* Stores the contents of a std::array as a contiguous block of memory
		* in the clipboard. The clipboard data size is calculated as
		* N * sizeof(T).
		*
		* The same data block can be exposed under multiple MIME types.
		*
		* note: Only the raw element data is exposed, not the std::array object itself.
		*       The array must remain valid while the clipboard owns the data.
		* 
		* @tparam T Element type of the array. Must be standard-layout.
		* @tparam N Number of elements in the array.
		* @param values Array containing the data to expose.
		* @param mimeTypes List of MIME types identifying the clipboard data.
		*
		* @return true on success, false on failure.
		*
		* @threadsafety Safe to call from any thread.
		*/
		template<typename T, size_t N>
		bool SetClipboardData(
			const std::array<T, N>& values,
			const std::vector<std::string>& mimeTypes
		) const {
			static_assert(std::is_standard_layout_v<T>,
				"Clipboard vector elements must be standard-layout");

			auto callback = [](void* userdata, const char*, size_t* outSize) -> const void* {
				if (!userdata || !outSize) 
					return nullptr;

				const auto* vec = static_cast<const std::array<T, N>*>(userdata);
				*outSize = N * sizeof(T);
				return vec->data();
			};

			return SetClipboardData(
				callback,
				nullptr,
				const_cast<std::array<T, N>*>(&values),
				mimeTypes
			);
		}

		/*
		* @brief Called on programm start
		*/
		virtual void OnStart() = 0;

		/*
		* @brief called every frame of the programm
		*/
		virtual void OnUpdate() = 0;

		/*
		* @brief called called on programm end
		*/
		virtual void OnQuit() = 0;
	private:
		std::string m_name = "UNKNOWN";
		Version m_version{ 0, 0, 0 };

		// 0 = ok; < 0 error
		int cancelStart = 0;
		
		SDL_Event m_sdlEvent;
		std::vector<std::unique_ptr<Window>> m_windows;
		std::vector<WindowID> m_windowsToClose;
		SDLCoreIDManager m_windowIDManager;
		std::string m_renderDriver;

		bool m_closeApplication = false;
		int m_vsync = 0;
		int m_fpsCap = 0;

		WindowID m_cursorLockWinID;
		WindowCallbackID m_cursorLockResizeCallbackID;
		WindowCallbackID m_cursorLockFocusGainCallbackID;
		WindowCallbackID m_cursorLockFocusLostCallbackID;
		SDL_Window* m_cursorLockSDLWin;
		bool m_cursorLockWinActive = false;
		bool m_cursorLockLastWinCursorGrab = false;// stores the last state of cursor grab before window was locked
		float m_cursorLockPosX = 0;
		float m_cursorLockPosY = 0;
		float m_cursorLastTime = 0;
		const float m_cursorTick = 0.1f;// intervall speed of the cursorLock (in sec)

		void Init();
		void ProcessSDLPollEvents();
		void ProcessSDLPollEventWindow(const std::unique_ptr<Window>& window);
		void FPSCapDelay(uint64_t frameStartTime) const;
		void LockCursor();
		/*
		* @brief Sets every var that is used for cursorlock to its default values.
		* and unsubs to the old win resize if the cb exists
		*/
		void ResetCursorLockParams();

		/**
		* @brief Sets the given value to all windows 
		* @param value -1 = adaptive, 0 = disabled, 1 = enabled
		*/
		void SetVsyncOnWindows(int value);
	};

}