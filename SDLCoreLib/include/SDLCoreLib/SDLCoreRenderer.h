#pragma once
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector3.h>
#include <CoreLib/Math/Vector4.h>
#include "Window.h"

namespace SDLCore::Renderer {

	/**
	* @brief Returns the currently active SDL renderer.
	* @return Pointer to the currently active SDL_Renderer, or nullptr if none is set.
	*/
	SDL_Renderer* GetActiveRenderer();

	/**
	* @brief Sets the SDL renderer associated with a given window ID as the current renderer.
	* @param winID The unique ID of the target window. Default is SDLCORE_INVALID_ID
	*
	* Retrieves the window from the Application instance by ID and sets its renderer
	* as the current global renderer context.
	* Logs an error if the renderer is invalid.
	*/
	void SetWindowRenderer(WindowID winID = WindowID{ SDLCORE_INVALID_ID });

	/**
	* @brief Clears the current rendering target using the active draw color.
	*/
	void Clear();

	/**
	* @brief Presents the current rendering buffer to the screen.
	*/
	void Present();

	#pragma region Color

	/**
	* @brief Sets the current drawing color using RGBA values.
	* 
	* @param r Color components in the range 0–255.
	* @param g Color components in the range 0–255.
	* @param b Color components in the range 0–255.
	* @param a Color components in the range 0–255.
	*/
	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/**
	* @brief Sets the current drawing color using RGB values.
	* 
	* @param r Color components in the range 0–255.
	* @param g Color components in the range 0–255.
	* @param b Color components in the range 0–255.
	*/
	void SetColor(Uint8 r, Uint8 g, Uint8 b);

	/**
	* @brief Sets the current drawing color using a Brightness value.
	*
	* @param brightness in the range 0–255.
	* @param alpha components in the range 0–255.
	*/
	void SetColor(Uint8 brightness, Uint8 alpha);

	/**
	* @brief Sets the current drawing color using a Brightness value.
	* @param brightness in the range 0–255.
	*/
	void SetColor(Uint8 brightness);

	/**
	* @brief Sets the current drawing color using RGBA values.
	* @param rgba Color components in the range 0–255.
	*/
	void SetColor(const Vector4& rgba);

	/**
	* @brief Sets the current drawing color using RGB values.
	* @param rgb Color components in the range 0–255.
	*/
	void SetColor(const Vector3& rgb);

	#pragma endregion

	#pragma region Primitives

	#pragma region Rectangle

	/**
	* @brief Draws a filled rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void FillRect(int x, int y, int w, int h);

	/**
	* @brief Draws a filled rectangle.
	* @param pos x,y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void FillRect(const Vector2& pos, int w, int h);

	/**
	* @brief Draws a filled rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void FillRect(int x, int y, const Vector2& size);

	/**
	* @brief Draws a filled rectangle.
	* @param pos x,y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void FillRect(const Vector2& pos, const Vector2& size);

	#pragma endregion

	#pragma endregion
}