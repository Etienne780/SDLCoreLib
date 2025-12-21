#include <vector>
#include <shared_mutex>

#include "UI/Types/UITypes.h"
#include "UI/UIRegistry.h"

namespace SDLCore::UI {

    static std::vector<Vector4> g_colors;
    static std::vector<std::shared_ptr<Font>> g_fonts;
    static std::vector<Texture> g_textures;
    static std::vector<double> g_numbers; // alle Zahlen als double speichern

	static std::shared_mutex g_colorsMutex;
	static std::shared_mutex g_fontsMutex;
	static std::shared_mutex g_texturesMutex;
	static std::shared_mutex g_numbersMutex;
	
	template<typename T>
	static bool IsInBounds(uint32_t id, const std::vector<T>& vec) {
		return id < vec.size();
	}

	template<typename ID, typename T>
	static ID RegisterValue(const T& value, std::vector<T>& vec, std::shared_mutex& mutex) {
		std::unique_lock lock(mutex);
		vec.emplace_back(value);
		return ID(static_cast<uint32_t>(vec.size() - 1));
	}

	template<typename ID, typename T>
	static ID RegisterValue(T&& value, std::vector<T>& vec, std::shared_mutex& mutex) {
		std::unique_lock lock(mutex);
		vec.emplace_back(std::forward<T>(value));
		return ID(static_cast<uint32_t>(vec.size() - 1));
	}

	template<typename T>
	static void SetRegisteredValue(uint32_t id, const T& value, std::vector<T>& vec, std::shared_mutex& mutex) {
		std::unique_lock lock(mutex);
		if (IsInBounds(id, vec))
			vec[id] = value;
	}

	template<typename T>
	static bool TryGetRegisteredValue(uint32_t id, std::vector<T>& vec, T& outValue, std::shared_mutex& mutex) {
		std::shared_lock lock(mutex);
		if (IsInBounds(id, vec)) {
			outValue = vec[id];
			return true;
		}
		return false;
	}

	template<typename T>
	static bool TryGetRegisteredValuePtr(uint32_t id, const std::vector<T>& vec, const T*& outValue, std::shared_mutex& mutex) {
		std::shared_lock lock(mutex);
		if (IsInBounds(id, vec)) {
			outValue = &vec[id];
			return true;
		}
		return false;
	}

	UIColorID UIRegistry::RegisterColor(const Vector4& color) {
		return RegisterValue<UIColorID>(color, g_colors, g_colorsMutex);
	}

	UIFontID UIRegistry::RegisterFont(const SystemFilePath& path) {
		return RegisterValue<UIFontID>(std::make_shared<Font>(path), g_fonts, g_fontsMutex);
	}
	
	UITextureID UIRegistry::RegisterTexture(const SystemFilePath& path) {
		Texture tex(path);
		return RegisterValue<UITextureID>(std::move(tex), g_textures, g_texturesMutex);
	}
	
	UINumberID UIRegistry::RegisterNumber(int number) {
		return RegisterValue<UINumberID>(static_cast<double>(number), g_numbers, g_numbersMutex);
	}
	
	UINumberID UIRegistry::RegisterNumber(float number) {
		return RegisterValue<UINumberID>(static_cast<double>(number), g_numbers, g_numbersMutex);
	}
	
	UINumberID UIRegistry::RegisterNumber(double number) {
		return RegisterValue<UINumberID>(number, g_numbers, g_numbersMutex);
	}


	void UIRegistry::SetRegisteredColor(UIColorID id, const Vector4& color) {
		SetRegisteredValue(id.value, color, g_colors, g_colorsMutex);
	}

	void UIRegistry::SetRegisteredFont(UIFontID id, const SystemFilePath& path) {
		SetRegisteredValue(id.value, std::make_shared<Font>(path), g_fonts, g_fontsMutex);
	}

	void UIRegistry::SetRegisteredTexture(UITextureID id, const SystemFilePath& path) {
		Texture tex(path);
		SetRegisteredValue(id.value, std::move(tex), g_textures, g_texturesMutex);
	}

	void UIRegistry::SetRegisteredNumber(UINumberID id, int number) {
		SetRegisteredValue(id.value, static_cast<double>(number), g_numbers, g_numbersMutex);
	}

	void UIRegistry::SetRegisteredNumber(UINumberID id, float number) {
		SetRegisteredValue(id.value, static_cast<double>(number), g_numbers, g_numbersMutex);
	}

	void UIRegistry::SetRegisteredNumber(UINumberID id, double number) {
		SetRegisteredValue(id.value, number, g_numbers, g_numbersMutex);
	}


	bool UIRegistry::TryGetRegisteredColor(UIColorID id, Vector4& outValue) {
		return TryGetRegisteredValue(id.value, g_colors, outValue, g_colorsMutex);
	}

	bool UIRegistry::TryGetRegisteredFont(UIFontID id, std::shared_ptr<Font>& outValue) {
		return TryGetRegisteredValue(id.value, g_fonts, outValue, g_fontsMutex);
	}

	bool UIRegistry::TryGetRegisteredTexture(UITextureID id, Texture& outValue) {
		return TryGetRegisteredValue(id.value, g_textures, outValue, g_texturesMutex);
	}

	bool UIRegistry::TryGetRegisteredNumber(UINumberID id, int& outValue) {
		double tmp;
		if (TryGetRegisteredValue(id.value, g_numbers, tmp, g_numbersMutex)) {
			outValue = static_cast<int>(tmp);
			return true;
		}
		return false;
	}

	bool UIRegistry::TryGetRegisteredNumber(UINumberID id, float& outValue) {
		double tmp;
		if (TryGetRegisteredValue(id.value, g_numbers, tmp, g_numbersMutex)) {
			outValue = static_cast<float>(tmp);
			return true;
		}
		return false;
	}

	bool UIRegistry::TryGetRegisteredNumber(UINumberID id, double& outValue) {
		return TryGetRegisteredValue(id.value, g_numbers, outValue, g_numbersMutex);
	}


	bool UIRegistry::TryGetRegisteredColor(UIColorID id, const Vector4*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_colors, outValue, g_colorsMutex);
	}

	bool UIRegistry::TryGetRegisteredFont(UIFontID id, const std::shared_ptr<Font>*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_fonts, outValue, g_fontsMutex);
	}

	bool UIRegistry::TryGetRegisteredTexture(UITextureID id, const Texture*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_textures, outValue, g_texturesMutex);
	}

	bool UIRegistry::TryResolve(UIColorID id, Vector4& out) {
		if (Vector4 color; TryGetRegisteredColor(id, color)) {
			out = color;
			return true;
		}
		return false;
	}

	bool UIRegistry::TryResolve(UINumberID id, int& out) {
		if (int value; TryGetRegisteredNumber(id, value)) {
			out = value;
			return true;
		}
		return false;
	}

	bool UIRegistry::TryResolve(UINumberID id, float& out) {
		if (float value; TryGetRegisteredNumber(id, value)) {
			out = value;
			return true;
		}
		return false;
	}

	bool UIRegistry::TryResolve(UINumberID id, double& out) {
		if (double value; TryGetRegisteredNumber(id, value)) {
			out = value;
			return true;
		}
		return false;
	}

	bool UIRegistry::TryResolve(UINumberID id, Vector2& out) {
		if (float value; TryGetRegisteredNumber(id, value)) {
			out = Vector2(value);
			return true;
		}
		return false;
	}

	bool UIRegistry::TryResolve(UINumberID id, Vector4& out) {
		if (float value; TryGetRegisteredNumber(id, value)) {
			out = Vector4(value);
			return true;
		}
		return false;
	}

}