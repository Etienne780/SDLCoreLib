#pragma once
#include "Types/Font/Font.h"
#include "Types/Texture.h"
#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	// needs the other overloads
	// FontID RegisterFont(const std::shared_ptr<Font>& font);
	// TextureID RegisterTexture(const Texture& texture);

	UIColorID RegisterColor(const Vector4& color);
	UIFontID RegisterFont(const SystemFilePath& path);
	UITextureID RegisterTexture(const SystemFilePath& path);
	UINumberID RegisterNumber(int number);
	UINumberID RegisterNumber(float number);
	UINumberID RegisterNumber(double number);
   
	void SetRegisteredColor(UIColorID id, const Vector4& color);
	void SetRegisteredFont(UIFontID id, const SystemFilePath& path);
	void SetRegisteredTexture(UITextureID id, const SystemFilePath& path);
	void SetRegisteredNumber(UINumberID id, int number);
	void SetRegisteredNumber(UINumberID id, float number);
	void SetRegisteredNumber(UINumberID id, double number);

	bool TryGetRegisteredColor(UIColorID id, Vector4& outValue);
	bool TryGetRegisteredFont(UIFontID id, std::shared_ptr<Font>& outValue);
	bool TryGetRegisteredTexture(UITextureID id, Texture& outValue);
	bool TryGetRegisteredNumber(UINumberID id, int& outValue);
	bool TryGetRegisteredNumber(UINumberID id, float& outValue);
	bool TryGetRegisteredNumber(UINumberID id, double& outValue);

	bool TryGetRegisteredColor(UIColorID id, const Vector4*& outValue);
	bool TryGetRegisteredFont(UIFontID id, const std::shared_ptr<Font>*& outValue);
	bool TryGetRegisteredTexture(UITextureID id, const Texture*& outValue);
}
