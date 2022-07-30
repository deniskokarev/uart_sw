//
// Created by Denis Kokarev on 7/27/22.
//

#include "modes.h"
#include "usbd_cdc_if.h"

volatile char input_ch = 0;

volatile int8_t cdcConnected = 0;

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

/**
 * Something is wrong with subsequent CDC_Transmit_FS()s.
 * Works fine when buffing multiple lines in one transmission.
 */
#define OBUF_SZ     127
static char obuf[OBUF_SZ + 1];

// cannot print from interrupt handler
static void usb_transmit(const char *s, int len) {
    if (cdcConnected) {
        CDC_Transmit_FS((uint8_t *) s, len);
    }
}

#define min(A,B) ((A<B)?A:B)
#define max(A,B) ((A>B)?A:B)

static char *str_append(char *dst, const char *src, int *rem) {
    int l = strlen(src);
    strncat(dst, src, *rem);
    char *r = dst + min(l, *rem);
    *rem = max(*rem - l, 0);
    return r;
}

void print_help() {
    char *p = obuf;
    int rem = OBUF_SZ;
    *p = 0;

    p = str_append(p, "Select from the following switch modes:\r\n", &rem);
    for (int i = 0; i< MODE_SZ; i++) {
        p = str_append(p, "\t", &rem);
        p = str_append(p, smode[i], &rem);
        p = str_append(p, "\r\n", &rem);
    }
    usb_transmit(obuf, p - obuf);
}

static void print_mode() {
    char *p = obuf;
    int rem = OBUF_SZ;
    *p = 0;

    p = str_append(p, "Mode: ", &rem);
    p = str_append(p, smode[mode], &rem);
    p = str_append(p, "\r\n", &rem);
    usb_transmit(obuf, p - obuf);
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
    // posting the input char to main thread for set_mode() there
    input_ch = '0' + mode;
}
