#pragma once
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector3.h>
#include <CoreLib/Math/Vector4.h>
#include "SDLCoreTypes.h"
#include "Window.h"

namespace SDLCore {
	class Vertex;
}

namespace SDLCore::Renderer {

	enum class BlendMode {
		NONE				= SDL_BLENDMODE_NONE,				/**< no blending: dstRGBA = srcRGBA */
		BLEND				= SDL_BLENDMODE_BLEND,				/**< alpha blending: dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA)), dstA = srcA + (dstA * (1-srcA)) */
		BLEND_PREMULTIPLIED = SDL_BLENDMODE_BLEND_PREMULTIPLIED,/**< pre-multiplied alpha blending: dstRGBA = srcRGBA + (dstRGBA * (1-srcA)) */
		ADD					= SDL_BLENDMODE_ADD,				/**< additive blending: dstRGB = (srcRGB * srcA) + dstRGB, dstA = dstA */
		ADD_PREMULTIPLIED	= SDL_BLENDMODE_ADD_PREMULTIPLIED,	/**< pre-multiplied additive blending: dstRGB = srcRGB + dstRGB, dstA = dstA */
		MOD					= SDL_BLENDMODE_MOD,				/**< color modulate: dstRGB = srcRGB * dstRGB, dstA = dstA */
		MUL					= SDL_BLENDMODE_MUL,				/**< color multiply: dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA)), dstA = dstA */
		INVALID				= SDL_BLENDMODE_INVALID
	};

	/**
	* @brief Returns the currently active SDL renderer.
	* @return Pointer to the currently active SDL_Renderer, or nullptr if none is set.
	*/
	SDL_Renderer* GetActiveRenderer();

	/**
	* @brief Returns the ID of the currently active Window.
	* @return ID of the active window.
	*/
	WindowID GetActiveWindowID();

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

	#pragma region ViewportAndClipping

	/**
	* @brief Returns the current render viewport.
	* @return Current viewport, or {0,0,0,0} if unavailable.
	*/
	SDLCore::Rect GetViewport();

	/**
	* @brief Sets the render viewport.
	* @param x Position X in pixels.
	* @param y Position Y in pixels.
	* @param w Width in pixels.
	* @param h Height in pixels.
	*/
	void SetViewport(int x, int y, int w, int h);

	/**
	* @brief Sets the render viewport.
	* @param pos Position in pixels.
	* @param w Width in pixels.
	* @param h Height in pixels.
	*/
	void SetViewport(const Vector2& pos, int w, int h);

	/**
	* @brief Sets the render viewport.
	* @param x Position X in pixels.
	* @param y Position Y in pixels.
	* @param size Size in pixels.
	*/
	void SetViewport(int x, int y, const Vector2& size);

	/**
	 * @brief Sets the render viewport.
	 * @param pos Position in pixels.
	 * @param size Size in pixels.
	 */
	void SetViewport(const Vector2& pos, const Vector2& size);

	/**
	* @brief Sets the render viewport.
	* @param transform (x, y, w, h)
	*/
	void SetViewport(const Vector4& transform);

	/**
	* @brief Sets the render viewport.
	* @param rect Viewport rectangle.
	*/
	void SetViewport(const SDLCore::Rect& rect);

	/**
	* @brief Resets the viewport to the full window.
	*/
	void ResetViewport();

	/**
	* @brief Returns the current clipping rectangle.
	* @return Current clip rect, or {0,0,0,0} if unavailable.
	*/
	SDLCore::Rect GetClipRect();

	/**
	* @brief Sets the clipping rectangle.
	* @param x Position X in pixels.
	* @param y Position Y in pixels.
	* @param w Width in pixels.
	* @param h Height in pixels.
	*/
	void SetClipRect(int x, int y, int w, int h);

	/**
	 * @brief Sets the clipping rectangle.
	 * @param pos Position in pixels.
	 * @param w Width in pixels.
	 * @param h Height in pixels.
	 */
	void SetClipRect(const Vector2& pos, int w, int h);

	/**
	* @brief Sets the clipping rectangle.
	* @param x Position X in pixels.
	* @param y Position Y in pixels.
	* @param size Size in pixels.
	*/
	void SetClipRect(int x, int y, const Vector2& size);

	/**
	* @brief Sets the clipping rectangle.
	* @param pos Position in pixels.
	* @param size Size in pixels.
	*/
	void SetClipRect(const Vector2& pos, const Vector2& size);

	/**
	* @brief Sets the clipping rectangle.
	* @param transform (x, y, w, h)
	*/
	void SetClipRect(const Vector4& transform);

	/**
	* @brief Sets the clipping rectangle.
	* @param rect Clip rectangle.
	*/
	void SetClipRect(const SDLCore::Rect& rect);

	/**
	* @brief Clears the clip rectangle (disables clipping).
	*/
	void ResetClipRect();

	#pragma endregion

	/**
	* @brief Enables or disables blending mode.
	* @param enabled True = blending enabled, false = disabled.
	*/
	void SetBlendMode(bool enabled);

	/**
	* @brief Sets the blending mode explicitly.
	* @param mode One of BlendMode values.
	*/
	void SetBlendMode(BlendMode mode);

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

	/**
	* @brief Sets the stroke width used for drawing outlined rectangles and lines.
	* @param width Minimum value is 1. Values below 1 will be clamped to 1.
	*/
	void SetStrokeWidth(float width);

	/**
	* @brief Sets whether strokes are drawn inward or outward relative to the shape bounds.
	* @param value True for inner stroke, false for outer stroke.
	*/
	void SetInnerStroke(bool value);

	#pragma region Primitives

	#pragma region Rectangle

	/**
	* @brief Draws a filled rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void FillRect(float x, float y, float w, float h);

	/**
	* @brief Draws a filled rectangle.
	* @param pos x,y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void FillRect(const Vector2& pos, float w, float h);

	/**
	* @brief Draws a filled rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void FillRect(float x, float y, const Vector2& size);

	/**
	* @brief Draws a filled rectangle.
	* @param pos x,y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void FillRect(const Vector2& pos, const Vector2& size);

	/**
	* @brief Draws a filled rectangle.
	* @param transform x, y, w, h in pixels.
	*/
	void FillRect(const Vector4& transform);


	/**
	* @brief Draws multiple filled rectangles in a single call.
	* @param transforms Pointer to an array of Vector4, each containing x, y, width, and height for a rectangle.
	* @param count Number of rectangles in the array.
	*/
	void FillRects(const Vector4* transforms, size_t count);

	/**
	* @brief Draws multiple filled rectangles in a single call.
	* @param transforms A vector of Vector4, each containing x, y, width, and height for a rectangle.
	*/
	void FillRects(std::vector<Vector4>& transforms);
	
	/**
	* @brief Draws an outlined rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void Rect(float x, float y, float w, float h);

	/**
	* @brief Draws an outlined rectangle.
	* @param pos x,y position in pixels.
	* @param w Rectangle width in pixels.
	* @param h Rectangle height in pixels.
	*/
	void Rect(const Vector2& pos, float w, float h);

	/**
	* @brief Draws an outlined rectangle.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void Rect(float x, float y, const Vector2& size);

	/**
	* @brief Draws an outlined rectangle.
	* @param pos x,y position in pixels.
	* @param size Rectangle Size in pixels.
	*/
	void Rect(const Vector2& pos, const Vector2& size);

	/**
	* @brief Draws an outlined rectangle.
	* @param transform x, y, w, h in pixels.
	*/
	void Rect(const Vector4& transform);

	/**
	* @brief Draws multiple outlined rectangles in a single call.
	* @param transforms Pointer to an array of Vector4, each containing x, y, width, and height for a rectangle.
	* @param count Number of rectangles in the array.
	*
	* The stroke width and inner/outer mode are applied to each rectangle.
	*/
	void Rects(const Vector4* transforms, size_t count);

	/**
	* @brief Draws multiple outlined rectangles in a single call.
	* @param transforms A vector of Vector4, each containing x, y, width, and height for a rectangle.
	*
	* The stroke width and inner/outer mode are applied to each rectangle.
	*/
	void Rects(const std::vector<Vector4>& transforms);

	#pragma endregion

	#pragma region Line

	/**
	* @brief Draws a line between two points in 2D space.
	* @param x1 Starting point X coordinate in pixels.
	* @param y1 Starting point Y coordinate in pixels.
	* @param x2 Ending point X coordinate in pixels.
	* @param y2 Ending point Y coordinate in pixels.
	*/
	void Line(float x1, float y1, float x2, float y2);

	/**
	* @brief Draws a line using a starting point vector and scalar end coordinates.
	* @param p1 Starting point as a Vector2.
	* @param x2 Ending point X coordinate in pixels.
	* @param y2 Ending point Y coordinate in pixels.
	*/
	void Line(const Vector2& p1, float x2, float y2);

	/**
	* @brief Draws a line using scalar start coordinates and a vector end point.
	* @param x1 Starting point X coordinate in pixels.
	* @param y1 Starting point Y coordinate in pixels.
	* @param p2 Ending point as a Vector2.
	*/
	void Line(float x1, float y1, const Vector2& p2);

	/**
	* @brief Draws a line between two 2D vectors.
	* @param p1 Starting point as a Vector2.
	* @param p2 Ending point as a Vector2.
	*/
	void Line(const Vector2& p1, const Vector2& p2);

	/**
	* @brief Draws a line using a Vector4 containing (x1, y1, x2, y2).
	* @param points Vector4 containing start and end coordinates.
	*               - x = start X
	*               - y = start Y
	*               - z = end X
	*               - w = end Y
	*/
	void Line(const Vector4& poins);

	#pragma endregion

	/**
	* @brief Draws a point on the screen.
	* @param x X coordinate in pixels.
	* @param y Y coordinate in pixels.
	*/
	void Point(float x, float y);

	void Polygon(const std::vector<Vertex>& vertices, const std::vector<int>& indices = {});

	#pragma endregion

	void Text(const std::string& text, float x, float y);
	void SetFont(const SystemFilePath& path);
	void SetFontSize(float size);
	float GetTextWidth(const std::string& text);
	float GetTextHeight(const std::string& text);

}