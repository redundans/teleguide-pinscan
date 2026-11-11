// Pin scanner for mapping an unknown key matrix.
// Every key press is typed out as the two Pico pins it connects, e.g. "GP3 GP14".
// Works without diodes: each pin is driven low in turn and all others are read.
#include "quantum.h"
#include "send_string.h"

// All GPIO pins exposed on a Raspberry Pi Pico
static const pin_t pins[] = {
    GP0,  GP1,  GP2,  GP3,  GP4,  GP5,  GP6,  GP7,  GP8,  GP9,  GP10, GP11, GP12,
    GP13, GP14, GP15, GP16, GP17, GP18, GP19, GP20, GP21, GP22, GP26, GP27, GP28,
};
static const uint8_t pin_numbers[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26, 27, 28,
};
#define PIN_COUNT (sizeof(pins) / sizeof(pins[0]))
#define SCAN_INTERVAL_MS 5

static uint32_t last_scan[PIN_COUNT + 1]; // raw result of the previous scan
static uint32_t stable[PIN_COUNT + 1]; // debounced connections

void matrix_init_custom(void) {
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        gpio_set_pin_input_high(pins[i]);
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    return false; // the real work happens in housekeeping_task_kb
}

// result[PIN_COUNT] holds pins that read low while nothing is driven,
// i.e. pins shorted to GND (or to a wire soldered to a GND pad).
static void scan(uint32_t result[]) {
    uint32_t grounded = 0;
    for (uint8_t j = 0; j < PIN_COUNT; j++) {
        if (!gpio_read_pin(pins[j])) grounded |= (1UL << j);
    }
    result[PIN_COUNT] = grounded;
    for (uint8_t i = 0; i < PIN_COUNT; i++) {
        gpio_set_pin_output(pins[i]);
        gpio_write_pin_low(pins[i]);
        wait_us(30);
        uint32_t bits = 0;
        for (uint8_t j = 0; j < PIN_COUNT; j++) {
            if (j != i && !gpio_read_pin(pins[j])) bits |= (1UL << j);
        }
        gpio_set_pin_input_high(pins[i]);
        result[i] = bits & ~grounded;
    }
}

static void type_pin(uint8_t index) {
    uint8_t n = pin_numbers[index];
    char buf[5] = {'G', 'P', 0, 0, 0};
    if (n >= 10) {
        buf[2] = '0' + n / 10;
        buf[3] = '0' + n % 10;
    } else {
        buf[2] = '0' + n;
    }
    send_string(buf);
}

void housekeeping_task_kb(void) {
    static uint32_t timer = 0;
    if (timer_elapsed32(timer) < SCAN_INTERVAL_MS) return;
    timer = timer_read32();

    uint32_t now[PIN_COUNT + 1];
    scan(now);

    for (uint8_t i = 0; i <= PIN_COUNT; i++) {
        // A change must be seen in two scans in a row to count
        uint32_t confirmed = now[i] & last_scan[i];
        uint32_t released  = ~now[i] & ~last_scan[i];
        uint32_t pressed   = confirmed & ~stable[i];
        stable[i] = (stable[i] | confirmed) & ~released;

        for (uint8_t j = (i == PIN_COUNT ? 0 : i + 1); j < PIN_COUNT; j++) {
            if (!(pressed & (1UL << j))) continue;
            if (i == PIN_COUNT) {
                type_pin(j);
                send_string(" GND\n");
            } else {
                type_pin(i);
                send_string(" ");
                type_pin(j);
                send_string("\n");
            }
        }
        last_scan[i] = now[i];
    }
}
