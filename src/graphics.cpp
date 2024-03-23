#include "../include/graphics.h"

GPU::~GPU()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GPU::initGraphics()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer);
    SDL_SetWindowSize(window, 480, 432);
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);
    SDL_SetWindowResizable(window, SDL_TRUE);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    textureA = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                 SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);

    memset(bgMap, 0x0, FB_SIZE);
    memset(spriteMap, 0x69, FB_SIZE);
    memset(winMap, 0x0, FB_SIZE);
    memset(bgMapA, 0x0, FB_SIZE_A);
    memset(spriteMapA, 0x0, FB_SIZE_A);
    memset(winMapA, 0x0, FB_SIZE_A);

    scrollX = 0;
    scrollY = 0;

    mmu->writeByte(LCD_STAT_REG, 0x80);
}
// TODO: a tile map consists of tile IDs
uint16_t GPU::getTileMap(int bit)
{
    return (((mmu->readByte(LCD_CNTRL_REG) >> BG_TILEMAP) & 1) == 1) ? 0x9C00 : 0x9800;
}
// consists of actual tiles, each of which are indexed by tile IDs (stored in tile maps)
uint16_t GPU::getTileData(int bit)
{
    return (((mmu->readByte(LCD_CNTRL_REG) >> BG_WIN_TILES) & 1) == 1) ? 0x8000 : 0x8800;
}

uint8_t GPU::read8000()
{
    return (mmu->readByte(tileData + (tileID * 0x10) + (y_off % 8 * 2)) >> (7 - (x_off % 8)) & 0x1) + (mmu->readByte(tileData + (tileID * 0x10) + (y_off % 8 * 2) + 1) >> (7 - (x_off % 8)) & 0x1) * 2;
}

// 0x8800 addressing mode
uint8_t GPU::read8800()
{
    return (mmu->readByte(tileData + 0x800 + ((uint8_t)tileID * 0x10) + (y_off % 8 * 2)) >> (7 - (x_off % 8)) & 0x1) +
           ((mmu->readByte(tileData + 0x800 + ((int8_t)tileID * 0x10) + (y_off % 8 * 2) + 1) >> (7 - (x_off % 8)) & 0x1) * 2);
}

void GPU::createBG()
{
    int color_val;
    tileMap = getTileMap(BG_TILEMAP);
    tileData = getTileData(BG_WIN_TILES);
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            tileID = mmu->readByte(tileMap + (i / 8 * 32) + (j / 8));
            color_val = mmu->readByte(tileData + (tileID * 0x10));
        }
    }
}

// fill the background map (w/ opacity) array
void GPU::calculateBG()
{
    scrollX = mmu->readByte(SCX);
    scrollY = mmu->readByte(SCY);
    // get tile map & data
    tileMap = getTileMap(BG_TILEMAP);     // set of tile IDs
    tileData = getTileData(BG_WIN_TILES); // actual tiles, each indexed by tile IDs
    uint8_t bg_palette = mmu->readByte(BG_COLOR_PALETTE);
    /*
     * handle wrapping
     * retrieve wanted tile ID
     * retrieve tile data w/ tile ID
     * get color from palette
     */
    // loop through the pixels of each line
    for (int i = 0; i < 256; i++)
    {
        int color_val;
        y_off = row + scrollY;
        x_off = i + scrollX;
        tileID = mmu->readByte(tileMap + (y_off / 8 * 32) + (x_off / 8));
        if (tileData == 0x8800)
            color_val = read8800();
        else
            color_val = read8000();
        // because the code we use to render graphics uses RGB values
        // we must convert each pixel's color values (0-3) to RGB values (0-255, 0-255, 0-255)
        int color = (bg_palette >> (2 * color_val)) & 3;
        // fill the bg map array
        for (int j = 0; j < 4; j++)
        {
            /*
             * Notes on array mathematics:
             * say we have a 3D array: screen[row][height][pixels]
             * in our case: screen[256][256][4]
             * to convert that 3D array into 1D: screen[row * height * pixels]
             * to index the 3rd row, 3rd col, 1st pixel (really the '0th' pixel):
             * for 3D array: screen[2][2][0]
             * for 1D array: screen[(2 * height * 4) + (2 * 4) + 0]
             * index = (x*height*depth) + (y*depth) + z
             */
            if (j == 3)
                bgMapA[(row * 256 * 4) + (i * 4) + j] = 0xFF;
            else
                bgMapA[(row * 256 * 4) + (i * 4) + j] = COLORS[color * 3 + j];
        }
    }
}

void GPU::calculateWindowMap()
{
    // TODO: fix this
    tileMap = getTileData(WIN_ENABLE);
    tileData = getTileData(BG_WIN_TILES);
    for (int r = 0; r < 256; r++)
    {
        for (int c = 0; c < 256; c++)
        {
            tileID = mmu->readByte(tileMap + ((r / 8 * 32) + (c / 8)));
            // color val
        }
    }
}
void GPU::calculateSpriteMap() {}
/*
void GPU::drawScanline()
{
    LCDC = mmu->readByte(LCD_CNTRL_REG);
    if (((LCDC >> BG_WIN_ENABLE) & 0x1) == 1)
    {
        // render tiles
    }
    if (((LCDC >> OBJ_ENABLE) & 0x1) == 1)
    {
        // render sprites
    }
    // etc.
}*/

/*
 * takes the calculated data stored in the frame buffers,
 * draws it into our textures, then calls the renderer to render the textures
 * into actual graphics that we can see on screen
 */
void GPU::drawFrame()
{
    // draw it line by line, pixel by pixel
    for (int row = 0; row < SCREEN_H; row++)
    {
        for (int col = 0; col < SCREEN_W; col++)
        {
            y_offA = (row * 256 * 4);
            x_offA = (col * 4);
            // TODO: draw bg
            // TODO: overite bg & draw window on top
            // TODO: overwrite window/bg & draw sprites on top
            for (int i = 0; i < 4; i++) // draw BG
            {
                f_bufferA[(row * SCREEN_W * 4) + (col * 4) + i] = bgMapA[y_offA + x_offA + i];
            }
            if (winMapA[y_offA + x_offA + 3] != 0) // draw window if not transparent
            {
                for (int i = 0; i < 4; i++)
                {
                    f_bufferA[(row * SCREEN_W * 4) + (col * 4) + i] = winMapA[y_offA + x_offA + i];
                }
            }
            if (spriteMapA[y_offA + x_offA + 3] != 0) // draw sprites
            {
                for (int i = 0; i < 4; i++)
                {
                    f_bufferA[(row * SCREEN_W * 4) + (col * 4) + i] = spriteMapA[y_offA + x_offA + i];
                }
            }
        }
    }
    SDL_UpdateTexture(textureA, NULL, f_bufferA, 160 * sizeof(uint8_t) * 4);
    SDL_RenderCopy(renderer, textureA, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void GPU::tick(int cycles)
{
    g_clocksum += cycles;
    // TODO: implement modes
    switch (mode)
    {
    case H_BLANK:
        handle_HBlank();
        break;
    case V_BLANK:
        handle_VBlank();
        break;
    case OAM_SCAN:
        scanOAM();
        break;
    case DRAW_PIXELS:
        drawPixels();
        break;
    default:
        break;
    }
    // set a STAT interrupt
    if (LY == LY_CMP)
    {
        if ((mmu->readByte(LCD_STAT_REG) >> 6) & 0x01)
            if (((mmu->readByte(LCD_STAT_REG) >> 2) & 0x01) == 0)
            {
                mmu->writeByte(0xFF0F, mmu->readByte(0xFF0F) | 2);
                mmu->writeByte(LCD_STAT_REG, mmu->readByte(LCD_STAT_REG) | 4);
            }
    }
    else
    {
        mmu->writeByte(LCD_STAT_REG, mmu->readByte(LCD_STAT_REG) & ~4);
    }
}