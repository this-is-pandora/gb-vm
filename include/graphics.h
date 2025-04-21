#pragma once
#include <stdint.h>
#include <memory>
#include "memory.h"
#include "SDL.h"
#include "SDL_syswm.h"

/* 
 * General notes:
 * the true resolution is actually 256x256 (or 32x32 tiles).
 * Of that 256x256, only 160x144 is what's actually seen by the player on screen
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
 * A tile is 8x8 pixels. Each row of pixels is 2-Bytes & each pixel is 2-bits (so 16-bits per row)
 * each tile is 16bytes in memory
 * The process:
 * 1. read tileIDs in tileMap
 * 2. use tileID to get the graphic data in tileData
 * 3. Store graphic data in the BG layer
 * VRAM data is stored from 0x8000 - 0x97FF w/ each tile being 16 bytes.
 * This can be divided into 3 blocks, 128 tiles in each block
 * bit-4 of the LCD control register, decides whether to use 0x8000 or 0x8800 as the addressing mode
 * A frame consists of 154 scanlines & during the 1st 144 lines,
 * the screen is drawn from top to bottom, left to right
 * A scanline is simply a row of pixels; the ppu draws each pixel one at a time,
 * then continues to the next scanline
 */
#define SCREEN_W 160
#define SCREEN_H 144
#define PIXEL_SIZE 4
#define MAX_SPRITES 40
#define MAX_SPRITES_PER_LINE 10

/* Notes for the LCDC
 * the bits of the LCD control register control what's displayed on screen
 * bit-0: BG & Window enable
 * bit-1: OBJ enable
 * bit-2: OBJ size; 0 = 8×8; 1 = 8×16
 * bit-3: BG tilemap; 0 = 9800–9BFF; 1 = 9C00–9FFF
 * bit-4: BG & Window tiles; 0 = 0x8800–0x97FF; 1 = 0x8000–0x8FFF
 * bit-5: Window enable
 * bit-6: Window tilemap; 0 = 0x9800-0x9BFF; 1 = 0x9C00-0x9FFF
 * bit-7 LCD & PPU enable
 *
 * Notes for LCD STAT register
 * it contains the status of the LCD
 * 00 - horizontal blank
 * 01 - v blank
 * 10 - searching for sprites
 * 11 - accessing vram
 */
#define LCD_CNTRL_REG 0xFF40
#define LCD_STAT_REG 0xFF41

// sprite stuff
#define OAM 0xFE00
#define OAM_END 0xFE9F
#define SPT 0x8000 // sprite pattern table
// positioning stuff
#define SCY 0xFF42 // scroll y
#define SCX 0xFF43 // scroll x
#define WY 0xFF4A  // window Y position
#define WX 0xFF4B  // window X position

// LCD Status Register
//  holds values from 0 - 153 w/ 144 - 153 indicating the VBLANK period
#define CURRENT_SCANLINE 0xFF44 // AKA the LY register. Stores the current scanline
#define LYC 0xFF45              // if LY = LYC, the STAT interrupt is requested

// color palettes
#define BG_COLOR_PALETTE 0xFF47
#define SPRITE_PALETTE_1 0xFF48
#define SPRITE_PALETTE_2 0xFF49

/* for the LCD methods */
#define BG_WIN_ENABLE 0
#define OBJ_ENABLE 1
#define OBJ_SIZE 2
#define BG_TILEMAP 3
#define BG_WIN_TILES 4
#define WIN_ENABLE 5
#define WIN_TILEMAP 6
#define LCD_PPU_ENABLE 7
// the true resolution is 256 x 256 pixels and each pixel is 3 or 4 bytes each
// in RGB format
#define FB_SIZE 196608   // f_buffer size: 256 * 256 * 3
#define FB_SIZE_A 262144 // f_bufferA size: 256 * 256 * 4

enum PPU_MODES
{
    H_BLANK,
    V_BLANK,
    OAM_SCAN, // search for objects
    DRAW_PIXELS
};
// TODO: refactor code to include this
enum COLOR_PALETTE
{
    WHITE, // for objects/sprites, ID = 0 means transparent
    LIGHT_GREY,
    DARK_GRAY,
    BLACK
};

class GPU
{
private:
    std::shared_ptr<MMU> mmu;

    int g_clocksum, line, tileMap, tileID, tileData;
    int scrollX, scrollY, STAT, LY, LY_CMP, LCDC, winX, winY;
    int x_off, y_off, x_offS, y_offS, x_offA, y_offA;
    // TODO: create different color palettes
    int COLORS[12] = {
        0xFF, 0xFF, 0XFF,  // white = (255, 255, 255)
        0xAA, 0xAA, 0xAA,  // dark gray = (170, 170, 170)
        0x55, 0x55, 0x55,  // light gray = (85, 85, 85)
        0x00, 0x00, 0x00}; // black = (0, 0, 0)
    // uint8_t f_buffer[SCREEN_W * SCREEN_H * 3];  // RGB
    uint8_t f_bufferA[SCREEN_W * SCREEN_H * 4]; // RGBA

    uint8_t bgMapA[FB_SIZE_A];
    uint8_t winMapA[FB_SIZE_A];
    uint8_t spriteMapA[FB_SIZE_A];
    // flags if the line has been calculated and the frame has been drawn already
    bool lineFlag, frameFlag;

    PPU_MODES mode;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *textureA;

    void changeMode(PPU_MODES mode);

public:
    GPU(std::shared_ptr<MMU> mmu);
    ~GPU();
    void initGraphics();
    bool lcdEnabled();
    void setPalette(); // TODO: create different color palettes

    void setTileMap(int id);
    uint16_t getTileMap(int bit); // 2 32x32 tilemaps to choose from in the VRAM
    uint16_t getTileData(int bit);
    uint8_t read8000(); // 0x8000 addressing mode
    uint8_t read8800(); // 0x8800 addressing mode
    uint8_t getMode();
    void calculateBG();
    void calculateWindowMap();
    void calculateSpriteMap();
    void drawScanline();

    // draw frame buffer to SDL_texture and then draw the texture w/ renderer
    void drawFrame();      // ~60 frames per second
    void tick(int cycles); // step the GPU
};