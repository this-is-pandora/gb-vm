#pragma once

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define PIXEL_SIZE 4
#define MAX_SPRITES 40
#define MAX_SPRITES_PER_LINE 10
// TODO: graphics stuff
//  include SDL

class GPU
{
private:
    uint8_t screenData[SCREEN_WIDTH][SCREEN_HEIGHT][3];

public:
    GPU()
    {
        screenData[160 / 2][144 / 2][0] = 0xFF;
        screenData[160 / 2][144 / 2][1] = 0;
        screenData[160 / 2][144 / 2][2] = 0;
    }
};