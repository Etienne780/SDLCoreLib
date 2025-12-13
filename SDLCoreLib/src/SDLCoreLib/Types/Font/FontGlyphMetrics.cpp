#include <algorithm>
#include "Types/Font/FontGlyphMetrics.h"

namespace SDLCore {

    GlypeMetrics::GlypeMetrics(char ch)
        : code(ch)
    {
        // Constructor intentionally empty.
        // Metrics and atlas placement will be filled after TTF_GetGlyphMetrics() 
        // and atlas generation.
    }

    int GlypeMetrics::MetricsWidth() const {
        return maxX - minX;
    }

    int GlypeMetrics::MetricsHeight() const {
        return maxY - minY;
    }

    int GlypeMetrics::AscenderHeight() const {
        return std::max(0, maxY);
    }

    int GlypeMetrics::DescenderHeight() const {
        return std::max(0, -minY);
    }

    int GlypeMetrics::BearingX() const {
        return minX;
    }

    int GlypeMetrics::BearingY() const {
        return maxY;
    }

    bool GlypeMetrics::HasAtlasPlacement() const {
        return atlasWidth > 0 && atlasHeight > 0;
    }

}