#pragma once

#define IER 0xFF0F // interrupt request register

enum Interrupts {
    VBLANK,
    LCD,
    TIMER,
    SERIAL,
    JOYPAD
};

//TODO: define interrupt routines
void requestInterrupt(int id);
void checkInterrupt();
void serviceInterrupt();

// interrupt handlers
void vblank_handler();
void lcd_handler();
void serial_handler();
void joypad_handler();
