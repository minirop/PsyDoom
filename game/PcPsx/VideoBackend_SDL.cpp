#include "VideoBackend_SDL.h"

#include "Asserts.h"
#include "Config.h"
#include "Gpu.h"
#include "PsxVm.h"
#include "Video.h"

#include <algorithm>
#include <SDL.h>

BEGIN_NAMESPACE(Video)

//------------------------------------------------------------------------------------------------------------------------------------------
// Creates the backend with the SDL renderer uninitialized
//------------------------------------------------------------------------------------------------------------------------------------------
VideoBackend_SDL::VideoBackend_SDL() noexcept 
    : mpSdlWindow(nullptr)
    , mpRenderer(nullptr)
    , mpFramebufferTexture(nullptr)
    , mpFramebufferPixels(nullptr)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Ensures everything is cleaned up
//------------------------------------------------------------------------------------------------------------------------------------------
VideoBackend_SDL::~VideoBackend_SDL() noexcept {
    destroyRenderers();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Get the window create flags required for an SDL video backend window
//------------------------------------------------------------------------------------------------------------------------------------------
uint32_t VideoBackend_SDL::getSdlWindowCreateFlags() noexcept {
    // Use OpenGL where it is supported since that is the main implementation for SDL renderer.
    // On MacOS it's better to use Metal however since OpenGL is deprecated.
    #if __APPLE__
        return SDL_WINDOW_METAL;
    #else
        return SDL_WINDOW_OPENGL;
    #endif
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Initializes the SDL renderer used by this backend
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::initRenderers(SDL_Window* const pSdlWindow) noexcept {
    ASSERT(pSdlWindow);

    // Must not already be initialized
    ASSERT(!mpRenderer);
    ASSERT(!mpFramebufferTexture);
    ASSERT(!mpFramebufferPixels);

    // Create the renderer and framebuffer texture
    mpSdlWindow = pSdlWindow;
    mpRenderer = SDL_CreateRenderer(pSdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!mpRenderer) {
        FatalErrors::raise("Failed to create renderer!");
    }

    mpFramebufferTexture = SDL_CreateTexture(
        mpRenderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        ORIG_DRAW_RES_X,
        ORIG_DRAW_RES_Y
    );

    if (!mpFramebufferTexture) {
        FatalErrors::raise("Failed to create a framebuffer texture!");
    }

    // Clear the renderer to black
    SDL_SetRenderDrawColor(mpRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mpRenderer);

    // Immediately lock the framebuffer texture in preparation for the next update
    lockFramebufferTexture();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Cleans up and destroys the SDL renderer used by this video backend
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::destroyRenderers() noexcept {
    if (mpFramebufferPixels) {
        unlockFramebufferTexture();
    }

    if (mpFramebufferTexture) {
        SDL_DestroyTexture(mpFramebufferTexture);
        mpFramebufferTexture = nullptr;
    }

    if (mpRenderer) {
        SDL_DestroyRenderer(mpRenderer);
        mpRenderer = nullptr;
    }

    mpSdlWindow = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Copies the output from the classic renderer (PSX framebuffer) to an SDL texture and then blits that to the screen.
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::displayFramebuffer() noexcept {
    copyPsxToSdlFramebufferTexture();
    presentSdlFramebufferTexture();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Lock the SDL texture we upload the PSX framebuffer to for writing
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::lockFramebufferTexture() noexcept {
    ASSERT(mpFramebufferTexture);
    ASSERT(!mpFramebufferPixels);

    int pitch = 0;

    if (SDL_LockTexture(mpFramebufferTexture, nullptr, reinterpret_cast<void**>(&mpFramebufferPixels), &pitch) != 0) {
        FatalErrors::raise("Failed to lock the framebuffer texture for writing!");
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Unlock the SDL texture containing the PSX framebuffer after we finish writing to it
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::unlockFramebufferTexture() noexcept {
    ASSERT(mpFramebufferPixels);
    ASSERT(mpFramebufferTexture);

    SDL_UnlockTexture(mpFramebufferTexture);
    mpFramebufferPixels = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Copies the rendered PSX GPU framebuffer the locked SDL texture, in preparation for blitting to the screen
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::copyPsxToSdlFramebufferTexture() noexcept {
    // Sanity checks
    ASSERT(mpFramebufferPixels);

    // Copy the framebuffer
    Gpu::Core& gpu = PsxVm::gGpu;
    const uint16_t* const vramPixels = gpu.pRam;
    uint32_t* pDstPixel = mpFramebufferPixels;
    
    for (uint32_t y = 0; y < ORIG_DRAW_RES_Y; ++y) {
        const uint16_t* const rowPixels = vramPixels + ((intptr_t) y + gpu.displayAreaY) * gpu.ramPixelW;
        const uint32_t xStart = (uint32_t) gpu.displayAreaX;
        const uint32_t xEnd = xStart + ORIG_DRAW_RES_X;
        ASSERT(xEnd <= gpu.ramPixelW);

        // Note: don't bother doing multiple pixels at a time - compiler is smart and already optimizes this to use SIMD
        for (uint32_t x = xStart; x < xEnd; ++x) {
            const uint16_t srcPixel = rowPixels[x];
            const uint32_t r = ((srcPixel >> 10) & 0x1F) << 3;
            const uint32_t g = ((srcPixel >> 5 ) & 0x1F) << 3;
            const uint32_t b = ((srcPixel >> 0 ) & 0x1F) << 3;
            
            *pDstPixel = (
               0xFF000000 |
               (r << 16) |
               (g << 8 ) |
               (b << 0 )
            );

            ++pDstPixel;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Presents the SDL texture which contains a rendered frame from the PSX GPU to the screen
//------------------------------------------------------------------------------------------------------------------------------------------
void VideoBackend_SDL::presentSdlFramebufferTexture() noexcept {
    // Sanity checks
    ASSERT(mpSdlWindow);
    ASSERT(mpRenderer);
    ASSERT(mpFramebufferTexture);

    // Get the size of the window in pixels and don't bother outputting to it if zero sized
    int32_t windowW = {};
    int32_t windowH = {};
    SDL_GetRendererOutputSize(mpRenderer, &windowW, &windowH);

    if ((windowW <= 0) || (windowH <= 0))
        return;

    // Get the window area to output the PSX framebuffer to  and don't bother outputting if zero sized
    SDL_Rect outputRect = {};
    Video::getClassicFramebufferWindowRect(
        windowW,
        windowH,
        outputRect.x,
        outputRect.y,
        (uint32_t&) outputRect.w,   // Note: alias with different signedness isn't UB
        (uint32_t&) outputRect.h
    );

    if ((outputRect.w <= 0) || (outputRect.h <= 0))
        return;

    // Done writing to the locked framebuffer, update the texture with whatever writes we made
    unlockFramebufferTexture();

    // Need to clear the window if we are not filling the whole screen.
    // Some stuff like NVIDIA video recording notifications can leave marks in the unused regions otherwise...
    if ((outputRect.w != windowW) || (outputRect.h != windowH)) {
        SDL_RenderClear(mpRenderer);
    }
    
    // Blit the framebuffer to the display
    SDL_RenderCopy(mpRenderer, mpFramebufferTexture, nullptr, &outputRect);

    // Present the rendered frame and re-lock the framebuffer texture
    SDL_RenderPresent(mpRenderer);
    lockFramebufferTexture();
}

END_NAMESPACE(Video)