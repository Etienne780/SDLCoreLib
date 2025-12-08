#include "Types/Input/InputTypes.h"

namespace SDLCore {

	void KeyState::Update() {
		wasPressed = false;
		wasReleased = false;
		isRepeating = false;
	}

    void KeyState::SetState(bool pressed, bool repeat) {
        if (pressed) {
            if (!isPressed) {
                wasPressed = true;
                isRepeating = true;
            }
            else {
                isRepeating = repeat;
            }
            isPressed = true;
        }
        else {
            if (isPressed) {
                wasReleased = true;
            }
            isPressed = false;
        }
    }

	bool KeyState::JustPressed() const {
		return wasPressed;
	}

	bool KeyState::JustReleased() const {
		return wasReleased;
	}

}