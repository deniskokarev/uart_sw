//
// Created by Denis Kokarev on 7/27/22.
//

#ifndef FIRMWARE_MODES_H
#define FIRMWARE_MODES_H

extern volatile char input_ch;

enum MODE {
    MODE_NONE = 0,
    MODE_SOC = 1,
    MODE_USBDBG = 2,
    MODE_SZ = 3
};

void print_help();
void set_mode(enum MODE new_mode);
void next_mode();

#endif //FIRMWARE_MODES_H
