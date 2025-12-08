#include <CoreLib/Log.h>
#include "Application.h"
#include "Types/Input/InputManager.h"

namespace SDLCore {

	void Input::SetWindow(WindowID winID) {
		if (winID.IsInvalid()) {
			s_activeSDLWindowID = 0;
			s_activeWinID.SetInvalid();
			s_activeWindowState = nullptr;
			return;
		}

		auto* app = Application::GetInstance();
		if (!app) {
			Log::Error("SDLCore::Input::SetWindow: Cant set window, cant get instance if application! Set active window to default");
			s_activeSDLWindowID = 0;
			s_activeWinID.SetInvalid();
			s_activeWindowState = nullptr;
			return;
		}

		s_activeWinID = winID;
		auto* win = app->GetWindow(winID);
		if (!win) {
			Log::Error("SDLCore::Input::SetWindow: Cant set window, window with id '{}', dosent exist! Set active window to default", winID);
			s_activeSDLWindowID = 0;
			s_activeWinID.SetInvalid();
			s_activeWindowState = nullptr;
			return;
		}

		auto id = win->GetSDLID();
		if (s_activeSDLWindowID == id)
			return;

		s_activeSDLWindowID = id;
		for (auto& state : s_windowStates) {
			if (state.sdlWinID == s_activeSDLWindowID) {
				s_activeWindowState = &state;
				return;
			}
		}
		// if not found, added it
		s_windowStates.emplace_back(s_activeSDLWindowID);
		s_activeWindowState = &s_windowStates.back();
	}

	WindowID Input::GetWindowID() {
		return s_activeWinID;
	}

	void Input::DropWindow() {
		s_activeWindowState = nullptr;
		s_activeSDLWindowID = 0;
	}
	
	void Input::LateUpdate() {
		for (auto& state : s_windowStates) {
			state.lastMousePos.Set(state.mousePos);
			state.scrollDir = 0;
			// resets the was pressed bools for the just methods
			for (auto& [_, key] : state.keyStates) {
				key.Update();
			}

			for (auto& [_, mb] : state.mouseButtonStates) {
				mb.Update();
			}
		}
	}

	void Input::ProcessEvent(const SDL_Event& e) {
		SDL_WindowID sdlWinID = e.window.windowID;
		WindowInputState* state = GetWindowState(sdlWinID);
		if (!state)
			return;

		SDL_Window* sdlFocusWin = SDL_GetKeyboardFocus();
		SDL_WindowID sdlFocusWinID = sdlFocusWin ? SDL_GetWindowID(sdlFocusWin) : 0;

		state->focused = (sdlWinID == sdlFocusWinID);

		if (state->focused) {
			switch (e.type) {
			case SDL_EVENT_MOUSE_MOTION:
				state->mousePos.Set(e.motion.x, e.motion.y);
				state->relativeMousePos.Set(e.motion.xrel, e.motion.yrel);
				break;

			case SDL_EVENT_MOUSE_WHEEL:
				state->scrollDir = static_cast<int>(e.wheel.y);
				break;

				// Key- und Mouse-Button-Events:
			case SDL_EVENT_KEY_DOWN:
				state->keyStates[e.key.scancode].SetState(true, e.key.repeat != 0);
				state->keyCodeLastPressed = ToKeyCode(e.key.scancode);
				break;
			case SDL_EVENT_KEY_UP: {
				state->keyStates[e.key.scancode].SetState(false);
				if (state->keyCodeLastPressed == ToKeyCode(e.key.scancode)) {
					state->keyCodeLastPressed = KeyCode::UNKNOWN;
				}
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				state->mouseButtonStates[e.button.button].SetState(true);
				state->mouseBtnLastPressed = ToMouseButton(e.button.button);
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP: {
				state->mouseButtonStates[e.button.button].SetState(false);
				if (state->mouseBtnLastPressed == ToMouseButton(e.button.button)) {
					state->mouseBtnLastPressed = MouseButton::UNKNOWN;
				}
				break;
			}
			}
		}
		else {
			state->keyStates.clear();
			state->mouseButtonStates.clear();
			state->mousePos.Set(0, 0);
			state->scrollDir = 0;
		}
	}

	#pragma region INPUT_ACTION_[ACTION]

	bool Input::ActionPressed(const InputAction& action) {
		auto& keyActions = action.GetKeyActions();
		for (auto& kc : keyActions) {
			if (KeyPressed(kc))
				return true;
		}

		auto& mouseActions = action.GetMouseActions();
		for (auto& mb : mouseActions) {
			if (MousePressed(mb))
				return true;
		}

		static bool warned = false;
		if (!warned) {
			Log::Warn("SDLCore::Input::ActionPressed: Gamepad support not implemented yet!");
			warned = true;
		}

		// Placeholder for future gamepad input check
		// auto& gamepadActions = action.GetGamepadActions();
		// for (auto& gp : gamepadActions)
		//     if (GamepadPressed(gp))
		//         return true;

		return false;
	}

	bool Input::ActionJustPressed(const InputAction& action) {
		auto& keyActions = action.GetKeyActions();
		for (auto& kc : keyActions) {
			if (KeyJustPressed(kc))
				return true;
		}

		auto& mouseActions = action.GetMouseActions();
		for (auto& mb : mouseActions) {
			if (MouseJustPressed(mb))
				return true;
		}

		static bool warned = false;
		if (!warned) {
			Log::Warn("SDLCore::Input::ActionJustPressed: Gamepad support not implemented yet!");
			warned = true;
		}

		// Placeholder for future gamepad input check
		// auto& gamepadActions = action.GetGamepadActions();
		// for (auto& gp : gamepadActions)
		//     if (GamepadJustPressed(gp))
		//         return true;

		return false;
	}

	bool Input::ActionJustReleased(const InputAction& action) {
		auto& keyActions = action.GetKeyActions();
		for (auto& kc : keyActions) {
			if (KeyJustReleased(kc))
				return true;
		}

		auto& mouseActions = action.GetMouseActions();
		for (auto& mb : mouseActions) {
			if (MouseJustReleased(mb))
				return true;
		}

		static bool warned = false;
		if (!warned) {
			Log::Warn("SDLCore::Input::ActionJustReleased: Gamepad support not implemented yet!");
			warned = true;
		}

		// Placeholder for future gamepad input check
		// auto& gamepadActions = action.GetGamepadActions();
		// for (auto& gp : gamepadActions)
		//     if (GamepadJustReleased(gp))
		//         return true;

		return false;
	}

	#pragma endregion

	#pragma region KEY_[ACTION]

	bool Input::KeyPressed(KeyCode key) {
		return CheckKeyAcrossWindows(key, INPUT_STATE_PRESSED);
	}

	bool Input::KeyRepeating(KeyCode key) {
		if (IsWindowSet()) {
			auto& map = s_activeWindowState->keyStates;
			auto it = map.find(ToScancode(key));
			if (it == map.end())
				return false;
			return it->second.isRepeating;
		}
			

		for (auto& state : s_windowStates) {
			auto& map = state.keyStates;
			auto it = map.find(ToScancode(key));
			if (it != map.end() && it->second.isRepeating)
				return true;
		}

		return false;
	}

	bool Input::KeyJustPressed(KeyCode key) {
		return CheckKeyAcrossWindows(key, INPUT_STATE_WAS_PRESSED);
	}

	bool Input::KeyJustReleased(KeyCode key) {
		return CheckKeyAcrossWindows(key, INPUT_STATE_WAS_RELEASED);
	}

	bool Input::AnyKeyPressed() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_PRESSED, nullptr);
	}

	bool Input::AnyKeyPressed(KeyCode& keyOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_PRESSED, &tmp);
		if (result)
			keyOut = ToKeyCode(tmp);
		return result;
	}

	bool Input::AnyKeyJustPressed() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_WAS_PRESSED, nullptr);
	}

	bool Input::AnyKeyJustPressed(KeyCode& keyOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_WAS_PRESSED, &tmp);
		if (result)
			keyOut = ToKeyCode(tmp);
		return result;
	}

	bool Input::AnyKeyJustReleased() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_WAS_RELEASED, nullptr);
	}

	bool Input::AnyKeyJustReleased(KeyCode& keyOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_KEYBOARD, INPUT_STATE_WAS_RELEASED, &tmp);
		if (result)
			keyOut = ToKeyCode(tmp);
		return result;
	}

	#pragma endregion

	#pragma region MOUSE_[ACTION]

	bool Input::MousePressed(MouseButton button) {
		return CheckMouseButtonAcrossWindows(button, INPUT_STATE_PRESSED);
	}

	bool Input::MouseJustPressed(MouseButton button) {
		return CheckMouseButtonAcrossWindows(button, INPUT_STATE_WAS_PRESSED);
	}

	bool Input::MouseJustReleased(MouseButton button) {
		return CheckMouseButtonAcrossWindows(button, INPUT_STATE_WAS_RELEASED);
	}

	bool Input::AnyMousePressed() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_PRESSED, nullptr);
	}

	bool Input::AnyMousePressed(MouseButton& mouseButtonOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_PRESSED, &tmp);
		if (result)
			mouseButtonOut = ToMouseButton(tmp);
		return result;
	}

	bool Input::AnyMouseJustPressed() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_WAS_PRESSED, nullptr);
	}

	bool Input::AnyMouseJustPressed(MouseButton& mouseButtonOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_WAS_PRESSED, &tmp);
		if (result)
			mouseButtonOut = ToMouseButton(tmp);
		return result;
	}

	bool Input::AnyMouseJustReleased() {
		return CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_WAS_RELEASED, nullptr);
	}

	bool Input::AnyMouseJustReleased(MouseButton& mouseButtonOut) {
		int tmp;
		bool result = CheckAnyInputAcrossWindows(INPUT_TYPE_MOUSE, INPUT_STATE_WAS_RELEASED, &tmp);
		if (result)
			mouseButtonOut = ToMouseButton(tmp);
		return result;
	}

	bool Input::IsMouseDragging(MouseButton button) {
		return (MousePressed(button) && GetMouseDelta().SquaredMagnitude() > 0);
	}

	Vector2 Input::GetMousePosition() {
		if (IsWindowSet())
			return s_activeWindowState->mousePos;

		SDL_Window* sdlFocusWin = SDL_GetKeyboardFocus();
		SDL_WindowID sdlFocusWinID = sdlFocusWin ? SDL_GetWindowID(sdlFocusWin) : 0;

		if (sdlFocusWinID != 0) {
			for (auto& state : s_windowStates) {
				if (state.sdlWinID == sdlFocusWinID)
					return state.mousePos;
			}
		}

		return Vector2{-1};
	}

	Vector2 Input::GetMouseDelta(bool invertYAchses) {
		if (IsWindowSet()) {
			Vector2& mousePos = s_activeWindowState->mousePos;
			Vector2& lastPos = s_activeWindowState->lastMousePos;
			return Vector2 {
				mousePos.x - lastPos.x,
				((invertYAchses) ? lastPos.y - mousePos.y : mousePos.y - lastPos.y)
			};
		}
			
		SDL_Window* sdlFocusWin = SDL_GetKeyboardFocus();
		SDL_WindowID sdlFocusWinID = sdlFocusWin ? SDL_GetWindowID(sdlFocusWin) : 0;

		if (sdlFocusWinID != 0) {
			for (auto& state : s_windowStates) {
				if (state.sdlWinID == sdlFocusWinID) {
					Vector2& mousePos = state.mousePos;
					Vector2& lastPos = state.lastMousePos;
					return Vector2{
						mousePos.x - lastPos.x,
						((invertYAchses) ? lastPos.y - mousePos.y : mousePos.y - lastPos.y)
					};
				}
			}
		}

		return Vector2{-1};
	}


	Vector2 Input::GetRelativePosition() {
		if (IsWindowSet())
			return s_activeWindowState->relativeMousePos;

		SDL_Window* sdlFocusWin = SDL_GetKeyboardFocus();
		SDL_WindowID sdlFocusWinID = sdlFocusWin ? SDL_GetWindowID(sdlFocusWin) : 0;

		if (sdlFocusWinID != 0) {
			for (auto& state : s_windowStates) {
				if (state.sdlWinID == sdlFocusWinID) {
					return state.relativeMousePos;
				}
			}
		}

		return Vector2{-1};
	}

	int Input::GetScrollDir() {
		if (IsWindowSet())
			return s_activeWindowState->scrollDir;

		for (auto& state : s_windowStates)
			if (state.scrollDir != 0)
				return state.scrollDir;

		return 0;
	}

	bool Input::GetScrollDir(int& dir) {
		if (IsWindowSet()) {
			dir = s_activeWindowState->scrollDir;
			return dir != 0;
		}

		for (auto& state : s_windowStates) {
			if (state.scrollDir != 0) {
				dir = state.scrollDir;
				return dir != 0;
			}
		}

		dir = 0;
		return false;
	}

	#pragma endregion

	#pragma region GAMEPAD_[ACTION]

	bool Input::GamepadButtonPressed(int gamepadIndex, GamepadButton button) {
		Log::Warn("CoreLib::Input::GamepadButtonPressed: Function is not implemented");
		return false;
	}

	bool Input::GamepadButtonJustPressed(int gamepadIndex, GamepadButton button) {
		Log::Warn("CoreLib::Input::GamepadButtonJustPressed: Function is not implemented");
		return false;
	}

	float Input::GamepadAxis(int gamepadIndex, GamepadAxisType  axis) {
		Log::Warn("CoreLib::Input::GamepadAxis: Function is not implemented");
		return 0.0f;
	}

	#pragma endregion

	bool Input::IsWindowSet() {
		return (s_activeWindowState != nullptr && s_activeSDLWindowID != 0);
	}

	Input::WindowInputState* Input::GetWindowState(SDL_WindowID sdlWinID) {
		auto it = std::find_if(s_windowStates.begin(), s_windowStates.end(),
			[sdlWinID](const WindowInputState& state) { return state.sdlWinID == sdlWinID; } );
		if (it != s_windowStates.end())
			return &(*it);
		return nullptr;
	}

	bool Input::CheckKeyAcrossWindows(KeyCode key, uint8_t stateMask) {
		if (IsWindowSet())
			return CheckInputState(*s_activeWindowState, ToScancode(key), INPUT_TYPE_KEYBOARD, stateMask);

		for (auto& state : s_windowStates)
			if (CheckInputState(state, ToScancode(key), INPUT_TYPE_KEYBOARD, stateMask))
				return true;

		return false;
	}

	bool Input::CheckMouseButtonAcrossWindows(MouseButton mouseButton, uint8_t stateMask) {
		if (IsWindowSet())
			return CheckInputState(*s_activeWindowState, ToMouseButtonID(mouseButton), INPUT_TYPE_MOUSE, stateMask);

		for (auto& state : s_windowStates)
			if (CheckInputState(state, ToMouseButtonID(mouseButton), INPUT_TYPE_MOUSE, stateMask))
				return true;

		return false;
	}

	bool Input::CheckAnyInputAcrossWindows(uint8_t typeMask, uint8_t stateMask, int* inputOut) {
		if (IsWindowSet())
			return AnyInputState(*s_activeWindowState, typeMask, stateMask, inputOut);

		for (auto& state : s_windowStates) {
			if (AnyInputState(state, typeMask, stateMask, inputOut))
				return true;
		}
		return false;
	}

	bool Input::CheckInputState(WindowInputState& state, int inputID, uint8_t typeMask, uint8_t  stateMask) {
		auto& map = (typeMask == INPUT_TYPE_KEYBOARD) ? state.keyStates : state.mouseButtonStates;
		auto it = map.find(inputID);
		if (it == map.end())
			return false;

		const KeyState& ks = it->second;

		bool isPrevious = (stateMask & 0b10) != 0;
		bool isPressed = (stateMask & 0b01) == 0;

		if (isPrevious) {
			return isPressed ? ks.wasPressed : ks.wasReleased;
		}
		else {
			return isPressed ? ks.isPressed : !ks.isPressed;
		}
	}

	bool Input::AnyInputState(WindowInputState& state, uint8_t typeMask, uint8_t stateMask, int* inputOut) {
		if (stateMask == INPUT_STATE_PRESSED) {
			bool result = (typeMask == INPUT_TYPE_KEYBOARD) ? state.keyCodeLastPressed != KeyCode::UNKNOWN : state.mouseBtnLastPressed != MouseButton::UNKNOWN;
			if (result) {
				if(inputOut)
					*inputOut = (typeMask == INPUT_TYPE_KEYBOARD) ? ToScancode(state.keyCodeLastPressed) : ToMouseButtonID(state.mouseBtnLastPressed);
				return result;
			}
		}
			
		auto& map = (typeMask == INPUT_TYPE_KEYBOARD) ? state.keyStates : state.mouseButtonStates;

		for (auto& [inputID, ks] : map) {
			bool isPrevious = (stateMask & 0b10) != 0;
			bool isPressed = (stateMask & 0b01) == 0;

			bool matched = false;
			if (isPrevious) {
				matched = isPressed ? ks.wasPressed : ks.wasReleased;
			}
			else {
				matched = isPressed ? ks.isPressed : !ks.isPressed;
			}

			if (matched) {
				if (inputOut)
					*inputOut = inputID;
				return true;
			}
		}

		return false;
	}


}