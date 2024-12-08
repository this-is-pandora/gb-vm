#pragma once
#include <stdint.h>
#include <memory>
#include "./memory.h"

/*
 * the GameBoy action/direction buttons are stored as a 2x4 matrix.
 * selecting a button/action will write to 0xFF00. The matrix looks like this:
 *          P14     P15
 *     P10  RIGHT   A
 *     P11  LEFT    B
 *     P12  UP      SELECT
 *     P13  DOWN    START
 * P10 = bit-0; 0 = pressed; 1 = not pressed
 * P11 = bit-1; 0 = pressed; 1 = not pressed
 * P12 = bit-2
 * etc.
 * bits 4-5 are set to indicate if a button or d-pad key has been pressed
 * then, bits 0-3 are read by the emulator to determine
 * if a the A btn or right has been pressed
 *
 * Alternatively:
 * Bits6-7                      etc...    Bit-1  Bit-0
 * |UNUSED|BTNS|D-PAD|DWN/START|UP/SELECT|B/LEFT|A/RIGHT|
 *
 */
#define JOYP 0xFF00 // AKA port P1
/*
 * bits for the joypad register
 * bits 5 and 4 tells whether the current key is a button or a direction
 */
#define SELECT_BTNS 5 // button AKA P15
#define SELECT_DPAD 4 // direction AKA P14
#define START_DWN 3   // P13
#define SELECT_UP 2   // P12
#define B_LEFT 1      // P11
#define A_RIGHT 0     // P10
/*
 * values for the 0xFF00 register. If we hold down
 * A (0x10), Start (0x80), and Up (0x4).
 * The value stored in reg A would be: 0x10 + 0x80 + 0x4 = 0x94
 */
#define KEY_A 0x10
#define KEY_B 0x20
#define KEY_LEFT 0x2
#define KEY_RIGHT 0x1

#define KEY_UP 0x4
#define KEY_DWN 0x8
#define KEY_SELECT 0x40
#define KEY_START 0x80

#define KEY_MASK 0x3F

typedef struct Input
{
    uint16_t previous = 0; // previous input from previous frame
    uint16_t current = 0;  // current input from current frame
};

class Joypad
{
private:
    std::shared_ptr<MMU> mmu;
    uint8_t joypad; // actual joypad value to write to memory
    Input inputs;

public:
    Joypad(std::shared_ptr<MMU> _mmu);

    void key_poll(); // sets the values in the Input struct

    uint8_t getJoypadState(int key); // returns the 'joypad' variable

    uint8_t keyIsPressed(int key);
    uint8_t keyIsReleased(int key);

    uint8_t keyWasPressed(int key);
    uint8_t keyWasReleased(int key);
};