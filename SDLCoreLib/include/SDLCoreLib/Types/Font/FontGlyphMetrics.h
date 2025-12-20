#pragma once

namespace SDLCore {

    /**
    * @brief Represents all metrics and atlas-placement properties for a single font glyph.
    *
    * The glyph metrics originate from SDL_ttf and follow baseline-relative coordinates:
    * - minX / maxX: horizontal bounding box relative to pen position.
    * - minY / maxY: vertical bounding box relative to baseline.
    * - advance: pen movement after rendering this glyph.
    *
    * Additionally, this structure stores atlas placement coordinates and dimensions.
    */
    class GlyphMetrics {
    public:
        GlyphMetrics() = default;

        /**
        * @brief Constructs glyph metrics for a specific codepoint.
        * @param ch Character code stored as unsigned integer (UTF-32 scalar).
        */
        GlyphMetrics(char ch);

        char code = 'a';          /**< Character code associated with this glyph. */

        // --- Font metrics (baseline-relative) ---
        int minX = 0;       /**< Minimum horizontal offset from pen position. */
        int maxX = 0;       /**< Maximum horizontal offset from pen position. */
        int minY = 0;       /**< Minimum vertical offset from baseline. */
        int maxY = 0;       /**< Maximum vertical offset from baseline. */
        int advance = 0;    /**< Pen advance after rendering the glyph. */

        // --- Atlas placement data ---
        int atlasX = 0;     /**< X coordinate inside the glyph atlas texture. */
        int atlasY = 0;     /**< Y coordinate inside the glyph atlas texture. */
        int atlasWidth = 0; /**< Actual rendered glyph width in atlas. */
        int atlasHeight = 0;/**< Actual rendered glyph height in atlas. */

        /**
        * @brief Returns the glyph width based on metrics (maxX - minX).
        */
        int MetricsWidth() const;

        /**
        * @brief Returns the glyph height based on metrics (maxY - minY).
        */
        int MetricsHeight() const;

        int AscenderHeight() const;

        int DescenderHeight() const;

        /**
        * @brief Returns the horizontal bearing (left side bearing).
        *
        * Defined as the offset from the pen position to the actual glyph bitmap.
        */
        int BearingX() const;

        /**
        * @brief Returns the vertical bearing (top side bearing).
        *
        * Defines the distance from baseline to the top of the glyph.
        */
        int BearingY() const;

        /**
        * @brief Returns the exact atlas rectangle as width/height.
        * @return True if atlas dimensions are valid.
        */
        bool HasAtlasPlacement() const;
    };

}