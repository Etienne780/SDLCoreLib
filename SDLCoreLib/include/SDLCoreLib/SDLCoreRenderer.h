#pragma once
#include <memory>
#include <array>
#include <vector>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector3.h>
#include <CoreLib/Math/Vector4.h>
#include <CoreLib/FormatUtils.h>

#include "SDLCoreTypes.h"
#include "Window.h"

namespace SDLCore {
	class Vertex;
}

namespace SDLCore::Render {

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

	/*
	* @brief Gets the current active color
	* @return current active color
	*/
	Vector4 GetActiveColor();

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
	* @param a Alpha component in the range 0-255.
	*/
	void SetColor(const Vector3& rgb, float a);

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

	/**
	* @brief Renders a polygon using the given vertex array. Rendering is always performed as a triangle list.
	* @param vertices Pointer to the first vertex element
	* @param vertexCount Number of vertices
	* @param texture Optional texture (default nullptr)
	* @param indices Optional index buffer (default nullptr). If provided, every 3 indices define exactly one triangle.
	* @param indexCount Number of indices (default 0)
	* @param xOffset X translation applied to all vertices (default 0.0f)
	* @param yOffset Y translation applied to all vertices (default 0.0f)
	* @param scaleX X scale applied to all vertices (default 1.0f)
	* @param scaleY Y scale applied to all vertices (default 1.0f)
	* @return true on success. Call SDLCore::GetError() for more information
	*
	* @note SDL_RenderGeometry always interprets the provided data as triangles.
	*       No automatic polygon creation or triangulation is performed.
	*/
	bool Polygon(const Vertex* vertices,
		size_t vertexCount,
		SDLCore::Texture* texture = nullptr,
		const int* indices = nullptr,
		size_t indexCount = 0,
		float xOffset = 0.0f,
		float yOffset = 0.0f,
		float scaleX = 1.0f,
		float scaleY = 1.0f);

	#pragma endregion

	#pragma region Texture

	/**
	* @brief Draws a texture to the current bound window.
	* @param texture The texture that will be drawn.
	* @param x Destination x-position in pixels.
	* @param y Destination y-position in pixels.
	* @param w Destination width in pixels (0 = use texture width).
	* @param h Destination height in pixels (0 = use texture height).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, float x, float y, float w = 0, float h = 0, const FRect* src = nullptr);

	/**
	 * @brief Draws a texture at a 2D position with specified width and height.
	* @param texture The texture that will be drawn.
	* @param pos Destination x,y position in pixels.
	* @param w Destination width in pixels (0 = use texture width).
	* @param h Destination height in pixels (0 = use texture height).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, const Vector2& pos, float w = 0, float h = 0, const FRect* src = nullptr);

	/**
	* @brief Draws a texture at a 2D position with a size vector.
	* @param texture The texture that will be drawn.
	* @param x Destination x-position in pixels.
	* @param y Destination y-position in pixels.
	* @param size Destination size (width = size.x, height = size.y).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, float x, float y, const Vector2& size, const FRect* src = nullptr);

	/**
	* @brief Draws a texture at a 2D position with a 2D size vector.
	* @param texture The texture that will be drawn.
	* @param pos Destination x,y position in pixels.
	* @param size Destination size (width = size.x, height = size.y).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, const Vector2& pos, const Vector2& size, const FRect* src = nullptr);

	/**
	* @brief Draws a texture using a Vector4 transform (x, y, width, height).
	* @param texture The texture that will be drawn.
	* @param transform Destination transform (x, y, width, height).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, const Vector4& transform, const FRect* src = nullptr);

	/**
	* @brief Draws a texture using an FRect transform.
	* @param texture The texture that will be drawn.
	* @param transform Destination rectangle (x, y, width, height).
	* @param src Optional source rectangle (nullptr = full texture).
	*/
	void Texture(SDLCore::Texture& texture, const FRect& transform, const FRect* src = nullptr);

	#pragma endregion

	#pragma region Text
	
	/**
	* @brief Draws a string of text at the specified position using the active font.
	* @param text The text to draw.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	*/
	void Text(const std::string& text, float x, float y);

	/**
	* @brief Draws formatted text at the specified position using the active font.
	* @param x X position in pixels.
	* @param y Y position in pixels.
	* @param args Variadic list of values inserted into the text format pattern ({} markers).
	* @note Each {} placeholder is replaced by the corresponding value. This approach is simple but
	*       may not be optimal for performance in frequent or high-volume calls.
	*/
	template<typename... Args>
	void TextF(float x, float y, Args&&... args) {
		Text(Log::GetFormattedString(std::forward<Args>(args)...), x, y);
	}

	// font size 16
	// text align start
	// ellipse = "..."
	// MaxLines = 0
	// TextLimit = 0 type::none
	// TextClipWidth = -1
	/**
	* @brief Resets all text-related rendering parameters to their default values.
	*
	* This resets font size(16), text alignment(START), ellipsis string("..."), maximum line count(0),
	* text truncation settings(0, NONE) and clip width(-1) to their default states.
	*/
	void ResetTextParams();

	/**
	* @brief Sets the active font using a shared pointer.
	* @param font The font to set as active.
	*/
	void SetFont(std::shared_ptr<Font> font);

	/**
	* @brief Sets the active font by loading it from a file path.
	* @param path Path to the font file.
	*/
	void SetFont(const SystemFilePath& path);

	/**
	* @brief Sets the active font size in pixels.
	* @param size Font size in pixels. Values less than 0 will be clamped to 0.
	*/
	void SetFontSize(float size);

	/**
	* @brief Retrieves the currently active font size.
	* @return The size of the active font in pixels.
	*/
	float GetActiveFontSize();

	/**
	* @brief Retrieves the currently active font object.
	* @return A shared pointer to the active font.
	*/
	std::shared_ptr<Font> GetActiveFont();

	/**
	* @brief Sets the horizontal text alignment mode.
	* @param horizontal Horizontal alignment value.
	*/
	void SetTextAlignHor(Align horizontal);

	/**
	* @brief Sets the vertical text alignment mode.
	* @param vertical Vertical alignment value.
	*/
	void SetTextAlignVer(Align vertical);

	/**
	* @brief Sets both horizontal and vertical text alignment modes.
	* @param horizontal Horizontal alignment.
	* @param vertical Vertical alignment.
	*/
	void SetTextAlign(Align horizontal, Align vertical);

	/**
	* @brief Sets both horizontal and vertical alignment to the same value.
	* @param aligment Alignment applied to both axes.
	*/
	void SetTextAlign(Align aligment);

	/**
	* @brief Returns the current horizontal text alignment.
	* @return Horizontal alignment value.
	*/
	Align GetTextAlignHor();

	/**
	* @brief Returns the current vertical text alignment.
	* @return Vertical alignment value.
	*/
	Align GetTextAlignVer();

	/**
	* @brief Sets the line height multiplier applied between text lines.
	*
	* The multiplier scales the additional spacing based on the active font size.
	*
	* @param multiplier Line height multiplier.
	*/
	void SetLineHeightMultiplier(float multiplier);

	/**
	* @brief Returns the currently configured line height multiplier.
	* @return Line height multiplier value.
	*/
	float GetLineHeightMultiplier();

	/**
	* @brief Sets the maximum number of lines rendered for a text block.
	*
	* A value of 0 disables line limiting.
	*
	* @param lines Maximum number of lines to render.
	*/
	void SetMaxLines(size_t lines);

	/**
	* @brief Returns the maximum number of lines allowed for text rendering.
	* @return Maximum line count, or 0 if unlimited.
	*/
	size_t GetMaxLines();

	/**
	* @brief Sets the ellipsis string used when text is truncated.
	* @param ellipsis Ellipsis string appended to truncated text.
	*/
	void SetTextEllipsis(const std::string& ellipsis);

	/**
	* @brief Returns the currently configured ellipsis string.
	* @return Ellipsis string.
	*/
	std::string GetEllipsis();

	/**
	* @brief Configures text truncation behavior.
	*
	* A value of 0 or type NONE disables truncation.
	*
	* @param amount Maximum character or pixel count.
	* @param type Truncation mode (characters or pixels).
	*/
	void SetTextLimit(size_t amount, UnitType type = UnitType::CHARACTERS);

	/**
	* @brief Returns the configured text truncation amount.
	* @return Truncation limit value.
	*/
	size_t GetTextLimitAmount();

	/**
	* @brief Returns the current text truncation mode.
	* @return Truncation type.
	*/
	UnitType GetTextLimitType();

	/**
	* @brief Returns a truncated version of the given text according to the
	* currently configured truncation settings.
	*
	* @param text Input text.
	* @return Truncated text if limits are enabled, otherwise the original text.
	*/
	std::string GetTruncatedText(const std::string& text);

	/**
	* @brief Sets the clip width used for automatic word-based line breaking.
	*
	* A value of -1 disables width-based line breaking.
	*
	* @param width Maximum line width in pixels.
	*/
	void SetTextClipWidth(float width);

	/**
	* @brief Returns the currently active text clip width.
	* @return Clip width in pixels, or -1 if disabled.
	*/
	float GetTextClipWidth();

	/**
	* @brief Disables width-based text clipping and line breaking.
	*/
	void ResetTextClipWidth();

	/**
	* @brief Estimates a font size that allows the given text to fit within specified width and height bounds.
	*
	* The calculation is based on the current font and its size. Ensure a valid font size is set prior to calling this function.
	* This function only computes a suitable size; it does not modify the active font size.
	*
	* @param text The text string to fit within the bounds.
	* @param targetW Maximum allowed width in pixels.
	* @param targetH Maximum allowed height in pixels.
	* @return Estimated font size in pixels that best fits the target dimensions.
	*/
	float CalculateFontSizeForBounds(const std::string& text, float targetW, float targetH);

	/**
	* @brief Returns the advance width of a single character.
	* @param c Character to measure.
	* @return Character width in pixels.
	*/
	float GetCharWidth(char c);

	/**
	* @brief Returns the height of a single character.
	*
	* @param c Character to measure.
	* @param ignoreBelowBaseline If true, descenders are excluded.
	* @return Character height in pixels.
	*/
	float GetCharHeight(char c, bool ignoreBelowBaseline = false);

	/**
	* @brief Calculates the width of a text string using the active font.
	* @param text Text to measure.
	* @return Width in pixels.
	*/
	float GetTextWidth(const std::string& text);

	/**
	* @brief Calculates the height of a text string using the active font.
	*
	* The height is based on the maximum ascender and optionally descender
	* found in the string.
	*
	* @param text Text to measure.
	* @param ignoreBelowBaseline If true, descenders are excluded.
	* @return Text height in pixels.
	*/
	float GetTextHeight(const std::string& text, bool ignoreBelowBaseline = false);

	/**
	* @brief Calculates the maximum width of a multi-line text block.
	* @param text Text block to measure.
	* @return Maximum line width in pixels.
	*/
	float GetTextBlockWidth(const std::string& text);

	/**
	* @brief Calculates the total height of a multi-line text block.
	*
	* Line height multipliers and maximum line limits are applied.
	*
	* @param text Text block to measure.
	* @param ignoreBelowBaseline If true, descenders are excluded.
	* @return Total text block height in pixels.
	*/
	float GetTextBlockHeight(const std::string& text, bool ignoreBelowBaseline = false);

	/**
	* @brief Calculates the height of a single rendered text line.
	*
	* Includes the configured line height multiplier.
	*
	* @param line Line of text to measure.
	* @param ignoreBelowBaseline If true, descenders are excluded.
	* @return Line height in pixels.
	*/
	float GetLineHeight(const std::string& line, bool ignoreBelowBaseline = false);

	#pragma endregion

}

template<>
static inline std::string FormatUtils::toString<SDLCore::Render::BlendMode>(SDLCore::Render::BlendMode blendMode) {
	switch (blendMode)
	{
	case SDLCore::Render::BlendMode::NONE:					return "None";
	case SDLCore::Render::BlendMode::BLEND:					return "Blend";
	case SDLCore::Render::BlendMode::BLEND_PREMULTIPLIED:	return "BlendPremultiplied";
	case SDLCore::Render::BlendMode::ADD:					return "Add";
	case SDLCore::Render::BlendMode::ADD_PREMULTIPLIED:		return "AddPremultiplied";
	case SDLCore::Render::BlendMode::MOD:					return "Mod";
	case SDLCore::Render::BlendMode::MUL:					return "Mul";
	case SDLCore::Render::BlendMode::INVALID:				return "Invalid";
	default:												return "UNKOWN";
	}
}