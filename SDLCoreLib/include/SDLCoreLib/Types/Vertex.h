#pragma once
#include <SDL3/SDL.h>
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>

namespace SDLCore {

    class Vertex {
    public:
        Vector2 position;       /**< Vertex position, in SDL_Renderer coordinates  */
        Vector4 color;          /**< Vertex color (0-255)*/
        Vector2 texCoordinate;  /**< Normalized texture coordinates, if needed */

        Vertex() = default;

        Vertex(float x, float y);
        Vertex(float x, float y, float r, float g, float b, float a);
        Vertex(float x, float y, float r, float g, float b, float a, float ux, float vy);

        Vertex(const Vector2& pos);
        Vertex(const Vector2& pos, const Vector4& col);
        Vertex(const Vector2& pos, const Vector4& col, const Vector2& tex);

        /**
        * @brief Conversion from SDL_Vertex to Vertex.
        */
        Vertex(const SDL_Vertex& sdlV);

        /**
        * @brief Implicit conversion to SDL_Vertex for direct SDL compatibility.
        */
        operator SDL_Vertex() const noexcept;
    };

}