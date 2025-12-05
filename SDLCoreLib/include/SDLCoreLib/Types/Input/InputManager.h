#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <CoreLib\Math\Vector2.h>

#include "SDLCoreTypes.h"
#include "InputTypes.h"
#include "InputAction.h"

class Vector2;

namespace SDLCore {

	class Application;

	/**
	* @brief Central input management system handling keyboard, mouse, and gamepad input across multiple SDL windows.
	*
	* Provides a static interface for querying input states, processing events,
	* and mapping user-defined InputActions to actual device inputs.
	*/
	class Input {
		friend class Application;
	public:
		/**
		* @brief Sets the currently active window for input queries.
		* @param windowID The WindowID to activate (default: invalid window).
		*/
		static void SetWindow(WindowID windowID = WindowID(SDLCORE_INVALID_ID));

		/**
		* @brief Resets the currently active window state to no window.
		*/
		static void DropWindow();

		// =========================================================
		// Input Action Interface
		// =========================================================

		/**
		* @brief Checks if any key, mouse button, or gamepad button assigned to the action is currently pressed.
		* @param action InputAction to check.
		* @return True if the action is currently pressed.
		*/
		static bool ActionPressed(const InputAction& action);

		/**
		* @brief Checks if the action was just pressed this frame.
		* @param action InputAction to check.
		* @return True if any assigned input was newly pressed this frame.
		*/
		static bool ActionJustPressed(const InputAction& action);

		/**
		* @brief Checks if the action was just released this frame.
		* @param action InputAction to check.
		* @return True if any assigned input was released this frame.
		*/
		static bool ActionJustReleased(const InputAction& action);


		// =========================================================
		// Keyboard
		// =========================================================

		/**
		* @brief Checks if a key is currently pressed.
		* @param key KeyCode to check.
		* @return True if the key is pressed.
		*/
		static bool KeyPressed(KeyCode key);

		/**
		* @brief Checks if a key is repeating (held down and generating repeat events).
		* @param key KeyCode to check.
		* @return True if the key is repeating.
		*/
		static bool KeyRepeating(KeyCode key);

		/**
		* @brief Checks if a key was just pressed this frame.
		* @param key KeyCode to check.
		* @return True if the key transitioned from released to pressed this frame.
		*/
		static bool KeyJustPressed(KeyCode key);

		/**
		* @brief Checks if a key was just released this frame.
		* @param key KeyCode to check.
		* @return True if the key transitioned from pressed to released this frame.
		*/
		static bool KeyJustReleased(KeyCode key);

		/**
		* @brief Checks if any key is currently pressed.
		* @return True if any key is pressed.
		*/
		static bool AnyKeyPressed();

		/**
		* @brief Checks if any key is currently pressed and returns its KeyCode.
		* @param keyOut Reference to store the last detected KeyCode.
		* @return True if any key is pressed.
		*/
		static bool AnyKeyPressed(KeyCode& keyOut);

		/**
		* @brief Checks if any key was just pressed this frame.
		* @return True if any key was newly pressed.
		*/
		static bool AnyKeyJustPressed();

		/**
		* @brief Checks if any key was just pressed this frame and returns its KeyCode.
		* @param keyOut Reference to store the detected KeyCode.
		* @return True if any key was newly pressed.
		*/
		static bool AnyKeyJustPressed(KeyCode& keyOut);

		/**
		* @brief Checks if any key was just released this frame.
		* @return True if any key was just released.
		*/
		static bool AnyKeyJustReleased();

		/**
		* @brief Checks if any key was just released this frame and returns its KeyCode.
		* @param keyOut Reference to store the detected KeyCode.
		* @return True if any key was just released.
		*/
		static bool AnyKeyJustReleased(KeyCode& keyOut);

		// =========================================================
		// Mouse
		// =========================================================

		/**
		* @brief Checks if a specific mouse button is pressed.
		* @param button MouseButton to check.
		* @return True if the button is pressed.
		*/
		static bool MousePressed(MouseButton button);

		/**
		* @brief Checks if a specific mouse button was just pressed.
		* @param button MouseButton to check.
		* @return True if the button transitioned from released to pressed this frame.
		*/
		static bool MouseJustPressed(MouseButton button);

		/**
		* @brief Checks if a specific mouse button was just released.
		* @param button MouseButton to check.
		* @return True if the button transitioned from pressed to released this frame.
		*/
		static bool MouseJustReleased(MouseButton button);

		/**
		* @brief Checks if any mouse button is currently pressed.
		* @return True if any mouse button is pressed.
		*/
		static bool AnyMousePressed();

		/**
		* @brief Checks if any mouse button is pressed and returns which one.
		* @param mouseButtonOut Reference to store the detected MouseButton.
		* @return True if any mouse button is pressed.
		*/
		static bool AnyMousePressed(MouseButton& mouseButtonOut);

		/**
		* @brief Checks if any mouse button was just pressed this frame.
		* @return True if any button was newly pressed.
		*/
		static bool AnyMouseJustPressed();

		/**
		* @brief Checks if any mouse button was just pressed and returns which one.
		* @param mouseButtonOut Reference to store the detected MouseButton.
		* @return True if any button was newly pressed.
		*/
		static bool AnyMouseJustPressed(MouseButton& mouseButtonOut);

		/**
		* @brief Checks if any mouse button was just released.
		* @return True if any button was just released.
		*/
		static bool AnyMouseJustReleased();

		/**
		* @brief Checks if any mouse button was just released and returns which one.
		* @param mouseButtonOut Reference to store the detected MouseButton.
		* @return True if any button was just released.
		*/
		static bool AnyMouseJustReleased(MouseButton& mouseButtonOut);

		/**
		* @brief Checks if a specific mouse button is being held and the mouse moved (dragging).
		* @param button MouseButton to check.
		* @return True if the mouse is currently dragging.
		*/
		static bool IsMouseDragging(MouseButton button);

		/**
		* @brief Gets the current mouse position relative to the active window.
		* @return Current mouse position as a Vector2.
		*/
		static Vector2 GetMousePosition();

		/**
		* @brief Gets the change in mouse position since the last frame.
		* @param invertYAchses Inverts the Y delta if true.
		* @return Mouse movement delta as a Vector2.
		*/
		static Vector2 GetMouseDelta(bool invertYAchses = false);

		static Vector2 GetRelativePosition();

		/**
		* @brief Gets the mouse scroll direction (1 = up, -1 = down, 0 = none).
		* @return Scroll direction integer.
		*/
		static int GetScrollDir();

		/**
		* @brief Gets the mouse scroll direction and writes it to the output parameter.
		* @param dir Reference to store the scroll direction.
		* @return True if a scroll occurred this frame.
		*/
		static bool GetScrollDir(int& dir);

		// =========================================================
		// Gamepad
		// =========================================================

		/**
		* @brief Checks if a specific gamepad button is pressed.
		* @param gamepadIndex Index of the connected gamepad.
		* @param button GamepadButton to check.
		* @return True if the button is pressed.
		*/
		static bool GamepadButtonPressed(int gamepadIndex, GamepadButton button);


		/**
		* @brief Checks if a specific gamepad button was just pressed.
		* @param gamepadIndex Index of the connected gamepad.
		* @param button GamepadButton to check.
		* @return True if the button was newly pressed.
		*/
		static bool GamepadButtonJustPressed(int gamepadIndex, GamepadButton button);

		/**
		* @brief Returns the analog value of a gamepad axis.
		* @param gamepadIndex Index of the connected gamepad.
		* @param axis Axis type to query.
		* @return Normalized axis value in range [-1, 1].
		*/
		static float GamepadAxis(int gamepadIndex, GamepadAxisType  axis);

	private:
		Input() = default;

		// =================== Internal State Masks ===================

		static constexpr uint8_t INPUT_STATE_PRESSED = 0b00;
		static constexpr uint8_t INPUT_STATE_RELEASED = 0b01;
		static constexpr uint8_t INPUT_STATE_WAS_PRESSED = 0b10;
		static constexpr uint8_t INPUT_STATE_WAS_RELEASED = 0b11;
		static constexpr uint8_t INPUT_TYPE_KEYBOARD = 0b0;
		static constexpr uint8_t INPUT_TYPE_MOUSE = 0b1;

		/**
		* @brief Stores input state for a single SDL window.
		*/
		struct WindowInputState {
			SDL_WindowID sdlWinID = 0;
			bool focused = false;
			std::unordered_map<int, KeyState> keyStates;
			std::unordered_map<int, KeyState> mouseButtonStates;

			KeyCode keyCodeLastPressed = KeyCode::UNKNOWN;
			MouseButton mouseBtnLastPressed = MouseButton::UNKNOWN;

			Vector2 mousePos{ 0, 0 };
			Vector2 lastMousePos{ 0, 0 };
			Vector2 relativeMousePos{ 0, 0 };
			int scrollDir = 0;

			WindowInputState(SDL_WindowID _sdlWinID)
				: sdlWinID(_sdlWinID) {
			}
		};

		// Active window and input states
		static inline std::vector<WindowInputState> s_windowStates;
		static inline WindowInputState* s_activeWindowState = nullptr;
		static inline SDL_WindowID s_activeSDLWindowID = 0;


		/**
		* @brief Stores button and axis states for a single gamepad.
		*/
		struct GamepadState {
			SDL_Gamepad* handle = nullptr;
			std::unordered_map<int, KeyState> buttonStates;
			std::unordered_map<int, float> axisValues;
		};
		static inline std::vector<GamepadState> s_gamepads;

		/**
		* @brief Updates all input states after event processing.
		*
		* Should be called once per frame after event polling to update previous state tracking.
		*/
		static void LateUpdate();

		/**
		* @brief Processes a single SDL event and updates the corresponding input state.
		* @param e SDL_Event to process.
		*/
		static void ProcessEvent(const SDL_Event& e);

		/**
		* @brief Returns true if an active window is currently set.
		*/
		static bool IsWindowSet();

		/**
		* @brief Retrieves the input state associated with the given SDL window.
		* @param sdlWinID SDL window ID.
		* @return Pointer to the WindowInputState, or nullptr if not found.
		*/
		static WindowInputState* GetWindowState(SDL_WindowID sdlWinID);

		/**
		* @brief Checks a specific key state across all registered windows.
		* @param key KeyCode to check.
		* @param stateMask Bitmask representing the desired state.
		* @return True if the key matches the given state.
		*/
		static bool CheckKeyAcrossWindows(KeyCode key, uint8_t stateMask);

		/**
		* @brief Checks a specific mouse button state across all registered windows.
		* @param mouseButton MouseButton to check.
		* @param stateMask Bitmask representing the desired state.
		* @return True if the mouse button matches the given state.
		*/
		static bool CheckMouseButtonAcrossWindows(MouseButton mouseButton, uint8_t stateMask);

		/**
		* @brief Checks for any input (keyboard or mouse) matching a given state across all windows.
		* @param typeMask Specifies input type (keyboard/mouse).
		* @param stateMask State to check for.
		* @param inputOut Optional pointer to receive the matching input ID.
		* @return True if a matching input is found.
		*/
		static bool CheckAnyInputAcrossWindows(uint8_t typeMask, uint8_t stateMask, int* inputOut = nullptr);
	
		/**
		* @brief Checks the state of a specific input (key or button) for a single window.
		* @param state Reference to the window input state.
		* @param inputID Internal input identifier.
		* @param typeMask Input type (keyboard or mouse).
		* @param stateMask Desired state bitmask.
		* @return True if the input matches the given state.
		*/
		static bool CheckInputState(WindowInputState& state, int inputID, uint8_t typeMask, uint8_t  stateMask);

		/**
		* @brief Checks whether any input of a given type in a window matches a specific state.
		* @param state Reference to the window input state.
		* @param typeMask Input type (keyboard or mouse).
		* @param stateMask Desired state bitmask.
		* @param inputOut Optional pointer to receive the matching input ID.
		* @return True if a matching input is found.
		*/
		static bool AnyInputState(WindowInputState& state, uint8_t typeMask, uint8_t stateMask, int* inputOut = nullptr);

		// =================== Conversion Helpers ===================

		static inline constexpr int ToScancode(KeyCode key) noexcept { return static_cast<int>(key); }
		static inline constexpr int ToMouseButtonID(MouseButton button) noexcept { return static_cast<int>(button); }
		static inline constexpr KeyCode ToKeyCode(int scanCode) noexcept { return static_cast<KeyCode>(scanCode); }
		static inline constexpr MouseButton ToMouseButton(int mouseButtonID) noexcept { return static_cast<MouseButton>(mouseButtonID); }
	};

}