#pragma once
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>

#include "Types/Font/Font.h"
#include "Types/Texture.h"
#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	// TODO: Consider adding additional overloads for RegisterFont/RegisterTexture
	//       to support direct Font or Texture objects if needed.
	//       This allows more flexible usage without relying solely on file paths.

	/*
	* @brief registry for ui values that get often used
	*/
	class UIRegistry {
	public:
		/**
		* @brief Registers a color in the UI system and returns a unique ID.
		* @param color Color to register as Vector4 (RGBA).
		* @return UIColorID Identifier for the registered color.
		*/
		static UIColorID RegisterColor(const Vector4& color);

		/**
		* @brief Registers a font from a file path and returns a unique ID.
		* @param path File path to the font.
		* @return UIFontID Identifier for the registered font.
		*/
		static UIFontID RegisterFont(const SystemFilePath& path);

		/**
		* @brief Registers a texture from a file path and returns a unique ID.
		*        The texture is loaded and stored in the registry.
		* @param path File path to the texture.
		* @return UITextureID Identifier for the registered texture.
		*/
		static UITextureID RegisterTexture(const SystemFilePath& path);

		/**
		* @brief Registers a numeric value as a double in the registry.
		* @param number Numeric value (int/float/double).
		* @return UINumberID Identifier for the registered number.
		*/
		static UINumberID RegisterNumber(int number);
		static UINumberID RegisterNumber(float number);
		static UINumberID RegisterNumber(double number);

		/**
		* @brief Updates a registered color by its ID.
		* @param id Identifier of the color to update.
		* @param color New color value.
		*/
		static void SetRegisteredColor(UIColorID id, const Vector4& color);

		/**
		* @brief Updates a registered font by its ID.
		* @param id Identifier of the font to update.
		* @param path New file path for the font.
		*/
		static void SetRegisteredFont(UIFontID id, const SystemFilePath& path);

		/**
		* @brief Updates a registered texture by its ID.
		* @param id Identifier of the texture to update.
		* @param path New file path for the texture.
		*/
		static void SetRegisteredTexture(UITextureID id, const SystemFilePath& path);

		/**
		* @brief Updates a registered number by its ID.
		* @param id Identifier of the number to update.
		* @param number New numeric value.
		*/
		static void SetRegisteredNumber(UINumberID id, int number);
		static void SetRegisteredNumber(UINumberID id, float number);
		static void SetRegisteredNumber(UINumberID id, double number);

		/**
		* @brief Attempts to get a copy of a registered color.
		* @param id Color identifier.
		* @param outValue Output variable to receive the value.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredColor(UIColorID id, Vector4& outValue);

		/**
		* @brief Attempts to get a copy of a registered font.
		* @param id Font identifier.
		* @param outValue Output shared_ptr to receive the font.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredFont(UIFontID id, std::shared_ptr<Font>& outValue);

		/**
		* @brief Attempts to get a copy of a registered texture.
		* @param id Texture identifier.
		* @param outValue Output variable to receive the texture.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredTexture(UITextureID id, Texture& outValue);

		/**
		* @brief Attempts to get a copy of a registered numeric value.
		* @param id Number identifier.
		* @param outValue Output variable to receive the number.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredNumber(UINumberID id, int& outValue);
		static bool TryGetRegisteredNumber(UINumberID id, float& outValue);
		static bool TryGetRegisteredNumber(UINumberID id, double& outValue);

		/**
		* @brief Attempts to get a pointer to a registered color (no copy).
		* @param id Color identifier.
		* @param outValue Pointer to the stored color value.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredColor(UIColorID id, const Vector4*& outValue);

		/**
		* @brief Attempts to get a pointer to a registered font (no copy).
		* @param id Font identifier.
		* @param outValue Pointer to the stored shared_ptr<Font>.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredFont(UIFontID id, const std::shared_ptr<Font>*& outValue);

		/**
		* @brief Attempts to get a pointer to a registered texture (no copy).
		* @param id Texture identifier.
		* @param outValue Pointer to the stored texture.
		* @return True if found, false otherwise.
		*/
		static bool TryGetRegisteredTexture(UITextureID id, const Texture*& outValue);

		static bool TryResolve(UIColorID id, Vector4& out);
		static bool TryResolve(UITextureID id, Texture& out);

		static bool TryResolve(UINumberID id, int& out);
		static bool TryResolve(UINumberID id, float& out);
		static bool TryResolve(UINumberID id, double& out);
		static bool TryResolve(UINumberID id, Vector2& out);
		static bool TryResolve(UINumberID id, Vector4& out);
	};

}