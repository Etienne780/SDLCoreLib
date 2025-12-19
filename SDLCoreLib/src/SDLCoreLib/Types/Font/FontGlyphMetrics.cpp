#include <algorithm>
#include "Types/Font/FontGlyphMetrics.h"

namespace SDLCore {

    GlyphMetrics::GlyphMetrics(char ch)
        : code(ch)
    {
        // Constructor intentionally empty.
        // Metrics and atlas placement will be filled after TTF_GetGlyphMetrics() 
        // and atlas generation.
    }

    int GlyphMetrics::MetricsWidth() const {
        return maxX - minX;
    }

    int GlyphMetrics::MetricsHeight() const {
        return maxY - minY;
    }

    int GlyphMetrics::AscenderHeight() const {
        return std::max(0, maxY);
    }

    int GlyphMetrics::DescenderHeight() const {
        return std::max(0, -minY);
    }

    int GlyphMetrics::BearingX() const {
        return minX;
    }

    int GlyphMetrics::BearingY() const {
        return maxY;
    }

    bool GlyphMetrics::HasAtlasPlacement() const {
        return atlasWidth > 0 && atlasHeight > 0;
    }

}