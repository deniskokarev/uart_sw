//
// Created by Denis Kokarev on 7/27/22.
//

#include "modes.h"
#include "usbd_cdc_if.h"

volatile char input_ch = 0;

enum MODE mode = MODE_NONE;

typedef struct {
    int soc;
    int usbdbg;
} MODE_CTL;

const char *smode[MODE_SZ] = {"0 - None", "1 - SOC On", "2 - USB Debug On"};
const MODE_CTL mode_ctl[MODE_SZ] = {{.soc = 0, .usbdbg = 0},
                                    {.soc = 1, .usbdbg = 0},
                                    {.soc = 0, .usbdbg = 1}};

void set_mode(enum MODE new_mode);

static int my_strlen(const char *s) {
    int len = 0;
    for (const char *p = s; *p; p++, len++);
    return len;
}

// cannot print from interrupt handler
static void my_puts(const char *s) {
    int rc;
    do {
        rc = CDC_Transmit_FS((uint8_t *) s, my_strlen(s));
    } while (rc == USBD_BUSY);
}

void print_help() {
    my_puts("Select from the following switch modes:\r\n");
    for (int i = 0; i< MODE_SZ; i++) {
        my_puts("\t");
        my_puts(smode[i]);
        my_puts("\r\n");
    }
}

static void print_mode() {
    my_puts("Mode: ");
    my_puts(smode[mode]);
    my_puts("\r\n");
}

static void soc_ctl_on() {
    HAL_GPIO_WritePin(GPIOA, SOC_CTL_Pin, GPIO_PIN_SET);
}

static void soc_ctl_off() {
    HAL_GPIO_WritePin(GPIOA, SOC_CTL_Pin, GPIO_PIN_RESET);
}

static void usbdbg_ctl_on() {
    HAL_GPIO_WritePin(GPIOA, USBDBG_CTL_Pin, GPIO_PIN_SET);
}

static void usbdbg_ctl_off() {
    HAL_GPIO_WritePin(GPIOA, USBDBG_CTL_Pin, GPIO_PIN_RESET);
}

// use only from main thread, not from interrupt
void set_mode(enum MODE new_mode) {
    // turn both off
    soc_ctl_off();
    usbdbg_ctl_off();
    mode = new_mode;
    mode %= MODE_SZ;
    // selectively turn them on depending on the mode
    if (mode_ctl[mode].soc)
        soc_ctl_on();
    if (mode_ctl[mode].usbdbg)
        usbdbg_ctl_on();
    print_mode();
}

void next_mode() {
    mode = (mode + 1) % MODE_SZ;
    input_ch = '0' + mode; // will become set_mode() in main thread
}
