#include <string>
#include "Types/Input/InputAction.h"

namespace SDLCore {
	
	InputAction* InputAction::AddKeyAction(KeyCode key) {
		AddUnique<KeyCode>(m_keyActions, key);
		return this;
	}

	InputAction* InputAction::AddMouseAction(MouseButton mouseButton) {
		AddUnique<MouseButton>(m_mouseActions, mouseButton);
		return this;
	}

	InputAction* InputAction::AddGamepadAction(GamepadButton gamepadButton) {
		AddUnique<GamepadButton>(m_gamepadActions, gamepadButton);
		return this;
	}

	InputAction* InputAction::AddKeyAction(const std::vector<KeyCode>& keys) {
		for (auto& k : keys)
			AddKeyAction(k);
		return this;
	}

	InputAction* InputAction::AddMouseAction(const std::vector<MouseButton>& mouseButtons) {
		for (auto& mb : mouseButtons)
			AddMouseAction(mb);
		return this;
	}

	InputAction* InputAction::AddGamepadAction(const std::vector<GamepadButton>& gamepadButtons) {
		for (auto& gb : gamepadButtons)
			AddGamepadAction(gb);
		return this;
	}

	InputAction* InputAction::RemoveKeyAction(KeyCode key) {
		RemoveAction<KeyCode>(m_keyActions, key);
		return this;
	}

	InputAction* InputAction::RemoveMouseAction(MouseButton mouseButton) {
		RemoveAction<MouseButton>(m_mouseActions, mouseButton);
		return this;
	}

	InputAction* InputAction::RemoveGamepadAction(GamepadButton gamepadButton) {
		RemoveAction<GamepadButton>(m_gamepadActions, gamepadButton);
		return this;
	}


	InputAction* InputAction::RemoveKeyAction(const std::vector<KeyCode>& keys) {
		for (const auto& k : keys)
			RemoveAction<KeyCode>(m_keyActions, k);
		return this;
	}

	InputAction* InputAction::RemoveMouseAction(const std::vector<MouseButton>& mouseButtons) {
		for (const auto& mb : mouseButtons)
			RemoveAction<MouseButton>(m_mouseActions, mb);
		return this;
	}

	InputAction* InputAction::RemoveGamepadAction(const std::vector<GamepadButton>& gamepadButtons) {
		for (const auto& gb : gamepadButtons)
			RemoveAction<GamepadButton>(m_gamepadActions, gb);
		return this;
	}

	InputAction* InputAction::ClearKeyAction() {
		m_keyActions.clear();
		return this;
	}

	InputAction* InputAction::ClearMouseAction() {
		m_mouseActions.clear();
		return this;
	}

	InputAction* InputAction::ClearGamepadAction() {
		m_gamepadActions.clear();
		return this;
	}

	bool InputAction::ContainsKeyAction(KeyCode key) const {
		return std::find(m_keyActions.begin(), m_keyActions.end(), key) != m_keyActions.end();
	}

	bool InputAction::ContainsMouseAction(MouseButton mouseButton) const {
		return std::find(m_mouseActions.begin(), m_mouseActions.end(), mouseButton) != m_mouseActions.end();
	}

	bool InputAction::ContainsGamepadAction(GamepadButton gamepadButton) const {
		return std::find(m_gamepadActions.begin(), m_gamepadActions.end(), gamepadButton) != m_gamepadActions.end();
	}

	const std::vector<KeyCode>& InputAction::GetKeyActions() const {
		return m_keyActions;
	}

	const std::vector<MouseButton>& InputAction::GetMouseActions() const {
		return m_mouseActions;
	}

	const std::vector<GamepadButton>& InputAction::GetGamepadActions() const {
		return m_gamepadActions;
	}

	InputAction* InputAction::SetKeyAction(const std::vector<KeyCode>& keys) {
		m_keyActions = keys;
		return this;
	}

	InputAction* InputAction::SetMouseAction(const std::vector<MouseButton>& mouseButtons) {
		m_mouseActions = mouseButtons;
		return this;
	}

	InputAction* InputAction::SetGamepadAction(const std::vector<GamepadButton>& gamepadButtons) {
		m_gamepadActions = gamepadButtons;
		return this;
	}

	std::string InputAction::ToString() const {
		std::string result = "Input Action: KeyActions=\"";

		AppendVectorToString<KeyCode>(result, m_keyActions, "No key actions defined");
		result += " MouseActions=\"";
		AppendVectorToString<MouseButton>(result, m_mouseActions, "No mouse actions defined");
		result += " GamepadActions=\"";
		AppendVectorToString<GamepadButton>(result, m_gamepadActions, "No gamepad actions defined");

		return result;
	}

}