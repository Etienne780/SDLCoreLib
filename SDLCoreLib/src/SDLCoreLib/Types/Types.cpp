#include <CoreLib/Math/Vector4.h>
#include "Types/Types.h"

namespace SDLCore {

	Rect ToRect(const FRect& fRect) {
		return Rect{static_cast<int>(fRect.x), static_cast<int>(fRect.y), 
			static_cast<int>(fRect.w), static_cast<int>(fRect.h) };
	}

	Rect ToRect(const Vector4& fRect) {
		return Rect{ static_cast<int>(fRect.x), static_cast<int>(fRect.y),
			static_cast<int>(fRect.z), static_cast<int>(fRect.w) };
	}


	FRect ToFRect(const Rect& rect) {
		return FRect{ static_cast<float>(rect.x), static_cast<float>(rect.y), 
			static_cast<float>(rect.w), static_cast<float>(rect.h) };
	}

	FRect ToFRect(const Vector4& rect) {
		return FRect{ rect.x, rect.y, rect.z, rect.w };
	}

	TextureParams operator|(TextureParams a, TextureParams b) {
		return static_cast<TextureParams>(static_cast<int>(a) | static_cast<int>(b));
	}

	TextureParams operator&(TextureParams a, TextureParams b) {
		return static_cast<TextureParams>(static_cast<int>(a) & static_cast<int>(b));
	}

	TextureParams& operator|=(TextureParams& a, TextureParams b) {
		a = a | b;
		return a;
	}

	TextureParams& operator&=(TextureParams& a, TextureParams b) {
		a = a & b;
		return a;
	}

	bool operator!(TextureParams a) {
		return static_cast<int>(a) == 0;
	}

}