#include "graphics.h"
#include <iostream>
#include <windows.h>

GPU::GPU(std::shared_ptr<MMU> mmu) : mmu(mmu)
{
    // mmu = mmu;
    line = 0;
    g_clocksum = 0;
    mode = OAM_SCAN;
    lineFlag = false;
    frameFlag = false;
    // everything below is for testing
    /*
    memset(bgMapA, 0x0, FB_SIZE_A);
    memset(spriteMapA, 0x0, FB_SIZE_A);
    memset(winMapA, 0x0, FB_SIZE_A);
    scrollX = 0;
    scrollY = 0;
    mmu->writeByte(LCD_STAT_REG, 0x80);*/
}

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
    /* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H); */
    textureA = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                 SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    /* memset(bgMap, 0x0, FB_SIZE);
     memset(spriteMap, 0x69, FB_SIZE);
     memset(winMap, 0x0, FB_SIZE);*/
    memset(bgMapA, 0x0, FB_SIZE_A);
    memset(spriteMapA, 0x0, FB_SIZE_A);
    memset(winMapA, 0x0, FB_SIZE_A);

    /* GUI STUFF
    SDL_SetWindowTitle(window, "GameBoy");

    SDL_SysWMinfo windowInfo;
    SDL_VERSION(&windowInfo.version);
    HWND hwnd = windowInfo.info.win.window;
    HMENU hMenu = CreateMenu();
    HMENU hFile = CreateMenu();

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFile, "main");
    AppendMenu(hFile, MF_STRING, 9, "load");
    SetMenu(hwnd, hMenu);*/

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    scrollX = 0;
    scrollY = 0;
    mmu->writeByte(LCD_STAT_REG, 0x80);
}

uint16_t GPU::getTileMap(int bit)
{
    return (((mmu->readByte(LCD_CNTRL_REG) >> bit) & 1) == 1) ? 0x9C00 : 0x9800;
}
// consists of actual tiles, each of which are indexed by tile IDs (stored in tile maps)
uint16_t GPU::getTileData(int bit)
{
    return (((mmu->readByte(LCD_CNTRL_REG) >> bit) & 1) == 1) ? 0x8000 : 0x8800;
}

uint8_t GPU::read8000()
{
    return (mmu->readByte(tileData + (tileID * 0x10) + (y_off % 8 * 2)) >> (7 - (x_off % 8)) & 0x1) + (mmu->readByte(tileData + (tileID * 0x10) + (y_off % 8 * 2) + 1) >> (7 - (x_off % 8)) & 0x1) * 2;
}

// 0x8800 addressing mode
uint8_t GPU::read8800()
{
    return (mmu->readByte(tileData + 0x800 + ((int8_t)tileID * 0x10) + (y_off % 8 * 2)) >> (7 - (x_off % 8)) & 0x1) +
           ((mmu->readByte(tileData + 0x800 + ((int8_t)tileID * 0x10) + (y_off % 8 * 2) + 1) >> (7 - (x_off % 8)) & 0x1) * 2);
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
        y_off = line + scrollY;
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
             * to convert that 3D array into 1D: screen[row * height * pixels] = screen[256*256*4]
             * to index the 3rd row, 3rd col, 1st pixel (aka the '0th' pixel):
             * for 3D array: screen[2][2][0]
             * for 1D array: screen[(row * height * pixels) + (col * pixels) + ith_pixel] = screen[(2*256*4) + (2*4) + 0]
             * index formula: (x*height*depth) + (y*depth) + z
             */
            if (j == 3)
                bgMapA[(line * 256 * 4) + (i * 4) + j] = 0xFF;
            else
                bgMapA[(line * 256 * 4) + (i * 4) + j] = COLORS[color * 3 + j];
        }
    }
}

void GPU::calculateWindowMap()
{
    int color, color_val, bg_palette;
    tileMap = getTileData(WIN_ENABLE);
    tileData = getTileData(BG_WIN_TILES);
    LCDC = mmu->readByte(LCD_CNTRL_REG);
    winX = mmu->readByte(WX) - 7;
    winY = mmu->readByte(WY);
    bg_palette = mmu->readByte(BG_COLOR_PALETTE);
    // check if window is enabled
    if (((LCDC >> WIN_ENABLE) & 0x01) == 1)
    {
        for (int j = 0; j < 256; j++)
        {
            tileID = mmu->readByte(tileMap + ((line / 8 * 32) + (j / 8)));
            color_val = 0; // TODO
            color = (bg_palette >> (2 * color_val)) & 3;
            for (int i = 0; i < 4; i++)
            {
                if (i == 3)
                    winMapA[(line * 256 * 4) + (j * 4) + i] = 0xFF;
                else
                    winMapA[(line * 256 * 4) + (j * 4) + i] = COLORS[color * 3 + i];
            }
        }
    }
}
void GPU::calculateSpriteMap()
{
    uint8_t flags, x, y, height, color_val;
    uint16_t palette, color;
    if ((mmu->readByte(LCD_CNTRL_REG) >> OBJ_ENABLE) & 0x01)
    {
        // loop through each object (4 bytes each) in the OAM
        /*
         * byte 0: y-position
         * byte 1: x-position
         * byte 2: tile ID
         * byte 3: attributes/flags
         *   bit 0-2: CGB only
         *   bit 3: CGB only
         *   bit 4: DMG palette
         *   bit 5: X flip
         *   bit 6: y flip
         *   bit 7: priority
         */
        for (uint16_t i = OAM; i <= OAM_END; i += 4)
        {
            y = mmu->readByte(i);
            x = mmu->readByte(i + 1);
            tileID = mmu->readByte(i + 2);
            flags = mmu->readByte(i + 3);
            // checks whether it is a regular sprite or a 'tall' sprite
            // a regular sprite is 8x8 pixels, a tall sprite is 8x16 pixels
            height = ((mmu->readByte(LCD_CNTRL_REG) >> OBJ_SIZE) & 0x01) ? 16 : 8;
            if (line >= (y - 16) && line <= ((y - 16) + height))
            {
                for (int k = 0; k < height; k++)
                {
                    for (int u = 0; u < 8; u++)
                    {
                        switch (flags & 0x60)
                        {
                        case 0x00: // no flip
                            color_val = (mmu->readByte(SPT + (tileID * 0x10) + (u * 2)) >> (7 - u) & 0x01) + (mmu->readByte(SPT + (tileID * 0x10) + (u * 2) + 1) >> u & 0x01) * 2;
                            break;
                        case 0x20: // x-flip
                            color_val = (mmu->readByte(SPT + (tileID * 0x10) + (k * 2)) >> u & 0x1) + (mmu->readByte(SPT + (tileID * 0x10) + (k * 2) + 1) >> u & 0x1) * 2;
                            break;
                        case 0x40: // y-flip
                            color_val = (mmu->readByte(SPT + (tileID * 0x10) + ((height - k - 1) * 2)) >> (7 - u) & 0x1) + (mmu->readByte(SPT + (tileID * 0x10) + ((height - k - 1) * 2) + 1) >> (7 - u) & 0x1) * 2;
                            break;
                        case 0x60: // both x-flip and y-flip
                            color_val = (mmu->readByte(SPT + (tileID * 0x10) + ((height - k - 1) * 2)) >> u & 0x1) + (mmu->readByte(SPT + (tileID * 0x10) + ((height - k - 1) * 2) + 1) >> u & 0x1) * 2;
                            break;
                        default:
                            break;
                        }
                        // 1. get color value
                        // 2. get RGB color from palette
                        // 3. store RGB data in the sprite map
                        palette = ((flags >> 4) & 1) ? SPRITE_PALETTE_1 : SPRITE_PALETTE_2;
                        color = (mmu->readByte(palette) >> (2 * color_val)) & 3;
                        // if color_val == 0, then it is transparent
                        if (color_val != 0 && ((y + k) <= 0xFF) && ((x + u) <= 0xFF))
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                if (j == 3)
                                    spriteMapA[((y + k - 16) * 256 * 4) + ((x + u - 8) * 4) + j] = 0xFF;
                                else
                                    spriteMapA[((y + k - 16) * 256 * 4) + ((x + u - 8) * 4) + j] = COLORS[color * 3 + j];
                            }
                        }
                    }
                }
            }
        }
    }
}

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

void GPU::changeMode(PPU_MODES m)
{
    mode = m;
    uint8_t n;
    int bit = (int)m & 1;
    int bit2 = ((int)m >> 1) & 1;
    if (bit)
    {
        n = mmu->readByte(LCD_STAT_REG);
        n |= 1;
        mmu->writeByte(LCD_STAT_REG, n);
    }
    else
    {
        n = mmu->readByte(LCD_STAT_REG);
        n &= ~((uint8_t)1);
        mmu->writeByte(LCD_STAT_REG, n);
    }

    if (bit2)
    {
        n = mmu->readByte(LCD_STAT_REG);
        n |= (1 << 1);
        mmu->writeByte(LCD_STAT_REG, n);
    }
    else
    {
        n = mmu->readByte(LCD_STAT_REG);
        n &= ~((uint8_t)(1 << 1));
        mmu->writeByte(LCD_STAT_REG, n);
    }
}

uint8_t GPU::getMode()
{
    return mode;
}

void GPU::tick(int cycles)
{
    g_clocksum += cycles;
    line = mmu->readByte(CURRENT_SCANLINE);
    // set GPU mode depending on current # of cycles passed
    // mode 0: H_BLANK (204 cycles), mode 2: OAM_SCAN (80cycles), mode 3: DRAW_PIXELS (172cycles)
    if (g_clocksum < 81)
    {
        changeMode(OAM_SCAN);
    }
    else if (g_clocksum < 253)
    {
        changeMode(DRAW_PIXELS);
    }
    else if (g_clocksum < 457)
    {
        changeMode(H_BLANK);
    }
    // check if in VBLANK
    if (!frameFlag && line == 144)
    {
        changeMode(V_BLANK);
    }

    if (g_clocksum > 252 && line < 145 && !lineFlag)
    {
        calculateBG();
        calculateWindowMap();
        calculateSpriteMap();
        lineFlag = true;
    }
    if (line == 144 && !frameFlag)
    {
        if (mmu->readByte(LCD_CNTRL_REG) >> 7)
        {
            mmu->writeByte(0xFF0F, mmu->readByte(0xFF0F) | 1);
            drawFrame();
            frameFlag = true;
            memset(bgMapA, 0, FB_SIZE);
            memset(winMapA, 0, FB_SIZE);
            memset(spriteMapA, 0, FB_SIZE);
        }
    }
    if (g_clocksum > 456)
    {
        g_clocksum -= 456;
        line++;
        mmu->incAddr(CURRENT_SCANLINE);
        // incScanLine();
        lineFlag = false;
    }
    LY_CMP = mmu->readByte(LYC);
    if (line == LY_CMP)
    {
        if ((mmu->readByte(LCD_STAT_REG) >> 6) & 0x01)
            if (((mmu->readByte(LCD_STAT_REG) >> 2) & 0x01) == 0)
            {
                mmu->writeByte(0xFF0F, mmu->readByte(0xFF0F) | 2); // TODO: rewrite
                mmu->writeByte(LCD_STAT_REG, mmu->readByte(LCD_STAT_REG) | 4);
            }
    }
    else
    {
        mmu->writeByte(LCD_STAT_REG, mmu->readByte(LCD_STAT_REG) & ~4);
    }

    if (line > 154)
    {
        line = 0;
        mmu->writeByte(CURRENT_SCANLINE, 0);
        frameFlag = false;
    }
}