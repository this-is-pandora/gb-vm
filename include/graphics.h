#pragma once
#include <stdint.h>
#include "memory.h"
#include "../SDL2/include/SDL2/SDL.h"

/*
 * the true resolution is actually 256x256 (or 32x32 tiles).
 * Of that 256x256,
 * only 160x144 is what's actually seen by the player on screen
 *
 * There are 3 layers to the gameboy's graphics:
 * 1. Background: a tilemap, i.e. a large grid of tiles.
 *      A tile is 8x8 pixels. A basic unit in gameboy graphics.
 *      Each pixel is 2-bits that store color codes 1 - 4
 * 2. Window: A rectangle above the Background layer. For static elements.
 *      Each color code is associated with a palette for the Background & Window layer
 * 3. Objects: Entries in the OAM (0xFE00-0xFE9F).
 *      Color codes 1-3 is associated with a palette, but 0 means transparent
 *      An object consists of 1-2 tiles and can move independently,
 *      e.g. the player character
 *
 * A tile is 8x8 pixels. Each row of pixels is 2-Bytes & each pixel is 2-bits
 * The process:
 * 1. read tileIDs in tileMap
 * 2. use tileID to get graphic data in tileData
 * 3. Store graphic data in the BG layer
 */
#define SCREEN_W 160
#define SCREEN_H 144
#define PIXEL_SIZE 4
#define MAX_SPRITES 40
#define MAX_SPRITES_PER_LINE 10

#define LCD_CNTRL_REG 0xFF40
#define LCD_STAT_REG 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define WY 0xFF4A
#define WX 0xFF4B

// TODO: implement methods for LCD registers
// TODO: VRAM methods
// TODO: implement OAM methods
// TODO: implement BG, Window, & Sprites layer
//  TODO: implement Tile Data Table
//  TODO: implement GUI code
enum COLOR
{
    WHITE,
    LIGHT_GREY,
    DARK_GRAY,
    BLACK
};

class GPU
{
private:
    uint8_t screenData[SCREEN_W][SCREEN_H][3];
    MMU *memory;
    int g_clocksum, mode, line;
    uint8_t f_buffer[SCREEN_W * SCREEN_H * 3];  // RGB
    uint8_t f_bufferA[SCREEN_W * SCREEN_H * 4]; // RGBA

    // GUI items
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

public:
    GPU(MMU *memory)
    {
        memory = memory;
        screenData[160 / 2][144 / 2][0] = 0xFF;
        screenData[160 / 2][144 / 2][1] = 0;
        screenData[160 / 2][144 / 2][2] = 0;
    }
    void initialize();
    void createWindow();
    int getColor();
    // LCD methods
    bool lcdEnabled();
    void renderTiles();
    void drawPixels();
    // draw frame buffer to SDL_texture and texture to renderer
    void drawFrame();
};