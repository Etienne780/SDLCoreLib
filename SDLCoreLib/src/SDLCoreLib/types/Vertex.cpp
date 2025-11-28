#include <CoreLib/Math/MathUtil.h>
#include "types/Vertex.h"

namespace SDLCore {

    Vertex::Vertex(float x, float y) 
        : position(Vector2(x, y)) {
    }

    Vertex::Vertex(float x, float y, float r, float g, float b, float a) 
        : position(Vector2(x, y)), color(Vector4(r, g, b, a)) {
    }

    Vertex::Vertex(float x, float y, float r, float g, float b, float a, float ux, float vy) 
        : position(Vector2(x, y)), color(Vector4(r, g, b, a)), texCoordinate(Vector2(ux, vy)) {
    }

    Vertex::Vertex(const Vector2& pos)
        : position(pos) {
    }

    Vertex::Vertex(const Vector2& pos, const Vector4& col)
        : position(pos), color(col) {
    }

    Vertex::Vertex(const Vector2& pos, const Vector4& col, const Vector2& tex)
        : position(pos), color(col), texCoordinate(tex) {
    }

    /**
    * @brief Conversion from SDL_Vertex to Vertex.
    */
    Vertex::Vertex(const SDL_Vertex& sdlV)
        : position(sdlV.position.x, sdlV.position.y),
        color(sdlV.color.r, sdlV.color.g, sdlV.color.b, sdlV.color.a),
        texCoordinate(sdlV.tex_coord.x, sdlV.tex_coord.y) {
    }
    
    Vertex::operator SDL_Vertex() const noexcept {
        SDL_Vertex v{};
        v.position = SDL_FPoint{ position.x, position.y };
        v.color = SDL_FColor{
            MathUtil::ClampValue(color.x, 0, 255),
            MathUtil::ClampValue(color.y, 0, 255),
            MathUtil::ClampValue(color.z, 0, 255),
            MathUtil::ClampValue(color.w, 0, 255)
        };
        v.tex_coord = SDL_FPoint{ texCoordinate.x, texCoordinate.y };
        return v;
    }

}