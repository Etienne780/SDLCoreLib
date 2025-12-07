#include <vector>

#include "UI/UIRegistry.h"

namespace SDLCore::UI {

    static std::vector<Vector4> g_colors;
    static std::vector<std::shared_ptr<Font>> g_fonts;
    static std::vector<Texture> g_textures;
    static std::vector<double> g_numbers; // alle Zahlen als double speichern

	template<typename T>
	static bool IsInBounds(uint32_t id, const std::vector<T>& vec) {
		return id < vec.size();
	}

	template<typename ID, typename T>
	static ID RegisterValue(const T& value, std::vector<T>& vec) {
		vec.emplace_back(value);
		return ID(static_cast<uint32_t>(vec.size() - 1));
	}

	template<typename T>
	static void SetRegisteredValue(uint32_t id, const T& value, std::vector<T>& vec) {
		if (IsInBounds(id, vec))
			vec[id] = value;
	}

	template<typename T>
	static bool TryGetRegisteredValue(uint32_t id, std::vector<T>& vec, T& outValue) {
		if (IsInBounds(id, vec)) {
			outValue = vec[id];
			return true;
		}
		return false;
	}

	template<typename T>
	static bool TryGetRegisteredValuePtr(uint32_t id, const std::vector<T>& vec, const T*& outValue) {
		if (IsInBounds(id, vec)) {
			outValue = &vec[id];
			return true;
		}
		return false;
	}

	UIColorID RegisterColor(const Vector4& color) {
		return RegisterValue<UIColorID>(color, g_colors);
	}

	UIFontID RegisterFont(const SystemFilePath& path) {
		return RegisterValue<UIFontID>(std::make_shared<Font>(path), g_fonts);
	}
	
	UITextureID RegisterTexture(const SystemFilePath& path) {
		return RegisterValue<UITextureID>(Texture(path), g_textures);
	}
	
	UINumberID RegisterNumber(int number) {
		return RegisterValue<UINumberID>(static_cast<double>(number), g_numbers);
	}
	
	UINumberID RegisterNumber(float number) {
		return RegisterValue<UINumberID>(static_cast<double>(number), g_numbers);
	}
	
	UINumberID RegisterNumber(double number) {
		return RegisterValue<UINumberID>(number, g_numbers);
	}


	void SetRegisteredColor(UIColorID id, const Vector4& color) {
		SetRegisteredValue(id.value, color, g_colors);
	}

	void SetRegisteredFont(UIFontID id, const SystemFilePath& path) {
		SetRegisteredValue(id.value, std::make_shared<Font>(path), g_fonts);
	}

	void SetRegisteredTexture(UITextureID id, const SystemFilePath& path) {
		SetRegisteredValue(id.value, Texture(path), g_textures);
	}

	void SetRegisteredNumber(UINumberID id, int number) {
		SetRegisteredValue(id.value, static_cast<double>(number), g_numbers);
	}

	void SetRegisteredNumber(UINumberID id, float number) {
		SetRegisteredValue(id.value, static_cast<double>(number), g_numbers);
	}

	void SetRegisteredNumber(UINumberID id, double number) {
		SetRegisteredValue(id.value, number, g_numbers);
	}


	bool TryGetRegisteredColor(UIColorID id, Vector4& outValue) {
		return TryGetRegisteredValue(id.value, g_colors, outValue);
	}

	bool TryGetRegisteredFont(UIFontID id, std::shared_ptr<Font>& outValue) {
		return TryGetRegisteredValue(id.value, g_fonts, outValue);
	}

	bool TryGetRegisteredTexture(UITextureID id, Texture& outValue) {
		return TryGetRegisteredValue(id.value, g_textures, outValue);
	}

	bool TryGetRegisteredNumber(UINumberID id, int& outValue) {
		double tmp;
		if (TryGetRegisteredValue(id.value, g_numbers, tmp)) {
			outValue = static_cast<int>(tmp);
			return true;
		}
		return false;
	}

	bool TryGetRegisteredNumber(UINumberID id, float& outValue) {
		double tmp;
		if (TryGetRegisteredValue(id.value, g_numbers, tmp)) {
			outValue = static_cast<float>(tmp);
			return true;
		}
		return false;
	}

	bool TryGetRegisteredNumber(UINumberID id, double& outValue) {
		return TryGetRegisteredValue(id.value, g_numbers, outValue);
	}


	bool TryGetRegisteredColor(UIColorID id, const Vector4*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_colors, outValue);
	}

	bool TryGetRegisteredFont(UIFontID id, const std::shared_ptr<Font>*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_fonts, outValue);
	}

	bool TryGetRegisteredTexture(UITextureID id, const Texture*& outValue) {
		return TryGetRegisteredValuePtr(id.value, g_textures, outValue);
	}

}