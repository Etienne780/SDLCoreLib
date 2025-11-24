#pragma once
#include <string>
#include <SDL3/SDL.h>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector3.h>
#include <CoreLib/FormatUtils.h>
#include <CoreLib/File.h>

#include "SDLCoreTypes.h"

namespace SDLCore {
	
    class Window;
    
    inline constexpr bool TEXTURE_FALLBACK_TEXTURE = true;

    /**
    * @brief Manages texture loading, GPU upload, and rendering for multiple SDL windows.
    *
    * This class encapsulates SDL surface and texture management.
    * Each window can have its own GPU texture instance, stored per WindowID.
    * Textures can be static (loaded once) or dynamic (updated frequently).
    */
    class Texture {
    public:

        /**
        * @brief Specifies how the texture behaves.
        */
        enum class Type {
            STATIC,
            DYNAMIC
        };

        enum class Flip {
            NONE = SDL_FlipMode::SDL_FLIP_NONE,                                         /**< Do not flip */
            HORIZONTAL = SDL_FlipMode::SDL_FLIP_HORIZONTAL,                             /**< flip horizontally */
            VERTICAL = SDL_FlipMode::SDL_FLIP_VERTICAL,                                 /**< flip vertically */
            HORIZONTAL_AND_VERTICAL = SDL_FlipMode::SDL_FLIP_HORIZONTAL_AND_VERTICAL    /**< flip horizontally and vertically (not a diagonal flip) */
        };

        /**
        * @brief Default constructor (creates an empty texture or a fallback texture if requested).
        * @param fallbackTexture If true, generates a simple fallback texture instead of leaving empty.
        */
        Texture(bool fallbackTexture = false);

        /**
        * @brief Loads an image from a file path.
        * @param path Path to the image file.
        * @param type Texture type (default Type::STATIC).
        */
        Texture(const char* path, Type type = Type::STATIC);

        /**
        * @brief Loads an image from a file path.
        * @param path Path to the image file.
        * @param type Texture type (default Type::STATIC).
        */
        Texture(const std::string& path, Type type = Type::STATIC);

        /**
        * @brief Loads an image from a system file path wrapper.
        * @param path File path object.
        * @param type Texture type (default Type::STATIC).
        */
        Texture(const SystemFilePath& path, Type type = Type::STATIC);

        /**
        * @brief Destructor. Frees all associated GPU textures and surface memory.
        */
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        /**
        * @brief Move constructor. Transfers ownership of all GPU and CPU resources from another texture.
        * @param other Source texture whose resources are moved. After the move, @p other is left in a valid but empty state.
        */
        Texture(Texture&& other) noexcept;

        /**
        * @brief Move assignment operator. Releases current resources and takes ownership from another texture.
        * @param other Source texture whose resources are moved. After the move, @p other is left in a valid but empty state.
        * @return Reference to this texture after assignment.
        */
        Texture& operator=(Texture&& other) noexcept;

        /**
        * @brief Ensures a GPU texture exists for the specified window.
        *
        * If a texture is already present for the window, it will be destroyed and replaced.
        * If the underlying surface is null, a fallback texture will be loaded instead.
        * Also registers a callback with the window to automatically free the texture
        * when the window is closed.
        *
        * @param windowID The identifier of the window for which to create the texture.
        * @return True if the texture was created or replaced successfully, false otherwise.
        */
        bool CreateForWindow(WindowID windowID);

        /**
        * @brief Frees the GPU texture for a specific window.
        *
        * This removes the texture from the GPU and unregisters any on-close callbacks
        * associated with this window. After calling this, the texture will need
        * to be recreated via CreateForWindow if it should be used again.
        *
        * @param windowID ID of the window whose texture should be released.
        */
        void FreeForWindow(WindowID windowID);

        /**
        * @brief Renders this texture in the active window.
        * @param x Destination x-position.
        * @param y Destination y-position.
        * @param w Destination width (defaults to texture width if 0).
        * @param h Destination height (defaults to texture height if 0).
        * @param src Optional source rectangle (nullptr = full texture).
        */
        void Render(float x, float y, float w = 0, float h = 0, const FRect* src = nullptr);

        /**
        * @brief Updates the pixel data of a dynamic texture.
        * @param windowID The window whose texture should be updated.
        * @param pixels Pointer to pixel data.
        * @param pitch Pitch (bytes per row) of the pixel data.
        */
        void Update(WindowID windowID, const void* pixels, int pitch);

        Texture* SetRotation(float rotation);
        Texture* SetCenter(const Vector2& center);
        Texture* SetColorTint(const Vector3& color);
        Texture* SetFlip(Flip flip);

        float GetRotation() const;
        Vector2 GetCenter() const;
        Vector3 GetColorTint() const;
        Flip GetFlip() const;

        /*
        * @param Resets all params to
        */
        Texture* Texture::Reset(TextureParams ignoreMask = TextureParams::NONE);
        
    private:
        struct SDLTexture {
            SDL_Texture* tex = nullptr;
            Uint8 lastR = 0, lastG = 0, lastB = 0;

            SDLTexture() = default;
            SDLTexture(SDL_Texture* texture) : tex(texture) {
            }
        };

        SDL_Surface* m_surface = nullptr;
        std::unordered_map<WindowID, SDLTexture> m_textures;

        std::unordered_map<WindowID, WindowCallbackID> m_windowSDLRendererDestroyCallbacks;

        int m_width = 0;
        int m_height = 0;
        float m_rotation = 0.0f;
        Vector2 m_center { 0.0f, 0.0f };
        Vector3 m_colorTint { 255.0f, 255.0f, 255.0f };
        Flip m_flip = Flip::NONE;
        Type m_type = Type::STATIC;

        /**
        * @brief Retrieves the SDL_Renderer for a given window.
        * @param winID The window ID.
        * @return Pointer to the renderer or nullptr on failure.
        */
        static SDL_Renderer* GetRenderer(WindowID winID);
        static SDL_Renderer* GetRenderer(WindowID winID, Window*& OutWin);

        /**
        * @brief Frees all SDL resources owned by this texture (CPU surface and GPU textures).
        *
        * This function destroys all associated GPU textures and the CPU surface,
        * resetting the object to an uninitialized state.
        */
        void Cleanup();

        /**
        * @brief Internal helper for moving texture resources from another instance.
        * @param other Source texture whose internal data is transferred. After the call, @p other no longer owns any resources.
        */
        void MoveFrom(Texture&& other) noexcept;

        /*
        * @brief loads the fallback texture in to the sdl surface
        */
        void LoadFallback();

        /**
        * @brief Removes the registered "on-close" callback for a specific window.
        *
        * This will unregister the callback from the window and remove it from
        * the internal map of window callbacks.
        *
        * If the application instance or the window does not exist, the function
        * will silently return.
        *
        * @param winID The ID of the window whose callback should be removed.
        */
        void RemoveSDLRendererDestroyCallbackForWindow(WindowID winID);
    };

}

template<>
static inline std::string FormatUtils::toString<SDLCore::Texture::Type>(SDLCore::Texture::Type type) {
    switch (type)
    {
    case SDLCore::Texture::Type::STATIC: return "STATIC";
    case SDLCore::Texture::Type::DYNAMIC: return "DYNAMIC";
    default: return "UNKNWON";
    }
}

template<>
static inline std::string FormatUtils::toString<SDLCore::Texture::Flip>(SDLCore::Texture::Flip flip) {
    switch (flip)
    {
    case SDLCore::Texture::Flip::NONE: return  "NONE";
    case SDLCore::Texture::Flip::HORIZONTAL: return "HORIZONTAL";
    case SDLCore::Texture::Flip::VERTICAL: return "VERTICAL";
    case SDLCore::Texture::Flip::HORIZONTAL_AND_VERTICAL: return "HORIZONTAL_AND_VERTICAL";
    default: return "UNKNWON";
    }
}