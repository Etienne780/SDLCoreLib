#pragma once
#include <vector>
#include <CoreLib/FormatUtils.h>
#include "InputTypes.h"

namespace SDLCore {

	/*
	* InputAction represents a logical action that can be triggered by one or more keys
	* or mouse buttons.
	*
	* Example use case:
	* - Define an "Jump" action that can be triggered by the space bar OR the right mouse button.
	* - Check InputAction in your game loop instead of querying multiple individual inputs.
	*/
	class InputAction {
	public:
		InputAction() = default;
		~InputAction() = default;

		/**
		* @brief Adds a single keyboard key as a trigger for this action.
		*        If the key is already assigned, it will not be added again.
		* @param key The KeyCode to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddKeyAction(KeyCode key);

		/**
		* @brief Adds a single mouse button as a trigger for this action.
		*        If the button is already assigned, it will not be added again.
		* @param mouseButton The MouseButton to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddMouseAction(MouseButton mouseButton);

		/**
		* @brief Adds a single gamepad button as a trigger for this action.
		*        If the button is already assigned, it will not be added again.
		* @param gamepadButton The GamepadButton to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddGamepadAction(GamepadButton gamepadButton);

		/**
		* @brief Adds multiple keyboard keys as triggers for this action.
		*        Duplicate entries are ignored.
		* @param keys A vector of KeyCodes to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddKeyAction(const std::vector<KeyCode>& keys);

		/**
		* @brief Adds multiple mouse buttons as triggers for this action.
		*        Duplicate entries are ignored.
		* @param mouseButtons A vector of MouseButtons to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddMouseAction(const std::vector<MouseButton>& mouseButtons);

		/**
		* @brief Adds multiple gamepad buttons as triggers for this action.
		*        Duplicate entries are ignored.
		* @param gamepadButtons A vector of GamepadButtons to add.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* AddGamepadAction(const std::vector<GamepadButton>& gamepadButtons);

		/**
		* @brief Removes a specific key from this action.
		*        If the key is not assigned, no changes are made.
		* @param key The KeyCode to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveKeyAction(KeyCode key);

		/**
		* @brief Removes a specific mouse button from this action.
		*        If the button is not assigned, no changes are made.
		* @param mouseButton The MouseButton to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveMouseAction(MouseButton mouseButton);

		/**
		* @brief Removes a specific gamepad button from this action.
		*        If the button is not assigned, no changes are made.
		* @param gamepadButton The GamepadButton to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveGamepadAction(GamepadButton gamepadButton);

		/**
		* @brief Removes multiple keys from this action.
		*        Keys that are not assigned are ignored.
		* @param keys A vector of KeyCodes to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveKeyAction(const std::vector<KeyCode>& keys);

		/**
		* @brief Removes multiple mouse buttons from this action.
		*        Buttons that are not assigned are ignored.
		* @param mouseButtons A vector of MouseButtons to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveMouseAction(const std::vector<MouseButton>& mouseButtons);

		/**
		* @brief Removes multiple gamepad buttons from this action.
		*        Buttons that are not assigned are ignored.
		* @param gamepadButtons A vector of GamepadButtons to remove.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* RemoveGamepadAction(const std::vector<GamepadButton>& gamepadButtons);

		/**
		* @brief Removes all assigned keys from this action.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* ClearKeyAction();

		/**
		* @brief Removes all assigned mouse buttons from this action.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* ClearMouseAction();

		/**
		* @brief Removes all assigned gamepad buttons from this action.
		* @return Pointer to this InputAction for method chaining.
		*/
		InputAction* ClearGamepadAction();

		/**
		* @brief Checks whether the specified key is assigned to this action.
		* @param key The KeyCode to check.
		* @return True if the key is part of this action; otherwise, false.
		*/
		bool ContainsKeyAction(KeyCode key) const;

		/**
		* @brief Checks whether the specified mouse button is assigned to this action.
		* @param mouseButton The MouseButton to check.
		* @return True if the mouse button is part of this action; otherwise, false.
		*/
		bool ContainsMouseAction(MouseButton mouseButton) const;

		/**
		* @brief Checks whether the specified gamepad button is assigned to this action.
		* @param gamepadButton The GamepadButton to check.
		* @return True if the gamepad button is part of this action; otherwise, false.
		*/
		bool ContainsGamepadAction(GamepadButton gamepadButton) const;

		/**
		* @brief Get all keyboard keys, that can trigger this action.
		* @return A const reference to the vector of KeyCodes
		*/
		const std::vector<KeyCode>& GetKeyActions() const;

		/**
		* @brief Get all mouse buttons, that can trigger this action.
		* @return A const reference to the vector of MouseButtons
		*/
		const std::vector<MouseButton>& GetMouseActions() const;

		/**
		* @brief Get all gamepad buttons, that can trigger this action.
		* @return A const reference to the vector of GamepadButtons
		*/
		const std::vector<GamepadButton>& GetGamepadActions() const;

		/**
		* @brief Replace all existing key triggers with a new set.
		* @param keys A vector of KeyCodes
		* @return Pointer to this InputAction for method chaining
		*/
		InputAction* SetKeyAction(const std::vector<KeyCode>& keys);

		/**
		* @brief Replace all existing mouse button triggers with a new set.
		* @param mouseButtons A vector of MouseButtons
		* @return Pointer to this InputAction for method chaining
		*/
		InputAction* SetMouseAction(const std::vector<MouseButton>& mouseButtons);

		/**
		* @brief Replace all existing gamepad button triggers with a new set.
		* @param gamepadButtons A vector of GamepadButtons
		* @return Pointer to this InputAction for method chaining
		*/
		InputAction* SetGamepadAction(const std::vector<GamepadButton>& gamepadButtons);

		/**
		* @brief Converts this InputAction to a human-readable string representation.
		*        Lists all assigned keys, mouse buttons, and gamepad buttons.
		* @return A string describing the InputAction and its assigned inputs.
		*/
		std::string ToString() const;

	private:
		std::vector<KeyCode> m_keyActions;
		std::vector<MouseButton> m_mouseActions;
		std::vector<GamepadButton> m_gamepadActions;

		/**
		* @brief Helper function that adds an element to a vector only if it is not already present.
		* @tparam T Type of element (KeyCode or MouseButton)
		* @param vec Vector to insert into
		* @param value Value to add
		*/
		template <typename T>
		static void AddUnique(std::vector<T>& vec, T value) {
			if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
				vec.push_back(value);
			}
		}

		/**
		* @brief Helper function that removes an element from a vector if it exists.
		* @tparam T Type of element (KeyCode, MouseButton, or GamepadButton)
		* @param vec Vector to remove the element from.
		* @param value Value to remove.
		*/
		template<typename T>
		void RemoveAction(std::vector<T>& vec, T value) {
			auto it = std::find(vec.begin(), vec.end(), value);
			if (it != vec.end())
				vec.erase(it);
		}

		/**
		* @brief Appends the elements of a vector to a string, separated by commas.
		*        If the vector is empty, appends a placeholder text instead.
		* @tparam T Type of elements in the vector (KeyCode, MouseButton, GamepadButton, etc.)
		* @param text Reference to the string to append to
		* @param vec Vector of elements to convert and append
		* @param emptyText Text to append if the vector is empty
		*/
		template<typename T>
		void AppendVectorToString(std::string& text, const std::vector<T>& vec, const std::string& emptyText) const {
			if (vec.empty()) {
				text += "-" + emptyText + "-\"";
			}
			else {
				for (size_t i = 0; i < vec.size() - 1; i++) {
					T element = vec[i];
					text += FormatUtils::toString(element) + ", ";
				}
				T element = vec.back();
				text += FormatUtils::toString(element) + "\"";
			}
		}
	};

}

template<>
static inline std::string FormatUtils::toString<SDLCore::InputAction>(SDLCore::InputAction inputAction) {
	return inputAction.ToString();
}