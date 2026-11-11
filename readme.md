# Teleguide pinscan

A helper tool for mapping the key matrix of an old keyboard from a Teleguide terminal, wired to a Raspberry Pi Pico.

The firmware acts as a USB keyboard. When you press a key, it **types out which two Pico pins that key connects**, for example `GP3 GP14`, into whatever text field is focused. It also works without diodes, since every pin is tested against every other pin. If a key connects a pin to ground, it types `GPx GND`, which reveals a wire sitting on a GND pin.

The actual keyboard firmware with Vial support is in [`keyboards/handwired/teleguide`](keyboards/handwired/teleguide/).

## Installing the firmware

### Importing
Place the files inside of [`keyboards/handwired`](keyboards/handwired/) inside of [`vial-kb/vial-qmk`](https://github.com/vial-kb/vial-qmk).

### Building

Build from the root of `vial-qmk`.

```sh
make handwired/teleguide_pinscan:default   # pin scanner
make handwired/teleguide:vial              # keyboard with Vial
```

The finished files end up in the root of `vial-qmk`:

- `handwired_teleguide_pinscan_default.uf2`
- `handwired_teleguide_vial.uf2`

### Flashing the Pico

1. Hold down **BOOTSEL** on the Pico and plug in the USB cable.
2. A drive named `RPI-RP2` appears.
3. Drag the `.uf2` file onto `RPI-RP2`. The Pico restarts by itself.

With the Vial firmware you can also hold down **TG** while plugging in USB. This clears the saved settings and puts the Pico straight into flashing mode.

## Using the pin scanner

1. Flash `handwired_teleguide_pinscan_default.uf2`.
2. Open a text editor.
3. For each key: type the key's name, for example `A: `, then press the key. The line becomes something like `A: GP4 GP10`.

Reading the output:

| Output | Meaning |
|---|---|
| `GP4 GP10` | The key connects GP4 and GP10. |
| `GP9 GND` | The pin is pulled to ground, so a wire is probably sitting on a GND pin. |
| nothing | At least one of the key's wires is not connected or has a bad contact. |

It can also be used to check the breadboard. Put a jumper in a GND column and touch another column. The scanner then types which GP pin that column is.

## Keys and GP pins

The keyboard is a matrix of **10 rows and 12 columns** without diodes. Each key connects a row pin to a column pin.

| Row \ Column | GP3 | GP4 | GP5 | GP6 | GP7 | GP8 | GP16 | GP14 | GP18 | GP13 | GP15 | GP0 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **GP12** | RADERA | 1 | 2 | 3 | 4 | 5 | 6 | 0 | 8 | 9 | 7 |  |
| **GP11** | Å | Q | W | E | R | T | Y | P | I | O | U |  |
| **GP10** | Ä | A | S | D | F | G | H | Ö | K | L | J |  |
| **GP9** | < | Z | X | C | V | B | N | - | , | . | M |  |
| **GP20** | SLUT | « | » | = | KONTRAST | UP | DOWN | RETURN | LEFT | RIGHT | SPACE |  |
| **GP17** | * | TG | HJÄLP | MENY | F4 | F5 | F6 | + | FÖNSTER | Ü | VISA |  |
| **GP19** | Num # | JA | Num 3 | Num 4 | Num 6 | Num 7 | Num 9 | Num * | SPAR | Num 1 | É |  |
| **GP21** | Num 0 | TELEFON | Num 2 | ¤ | Num 5 | SKRIVARE | Num 8 | R |  | NEJ |  |  |
| **GP1** |  |  |  |  |  |  |  |  |  |  |  | LOCK |
| **GP2** |  |  |  |  |  |  |  |  |  |  |  | SHIFT (both) |

Key names are the Swedish labels printed on the keycaps:

| Label | Meaning | Location |
|---|---|---|
| RADERA | delete | key with a crossed-out box |
| SLUT | end | top row |
| HJÄLP | help | top row |
| MENY | menu | top row |
| SPAR | save | top row |
| VISA | show | top row |
| FÖNSTER | window | key with two boxes, top row |
| TELEFON | telephone | key with a phone symbol, top right |
| KONTRAST | contrast | bottom left |
| SKRIVARE | printer | right of `*` |
| JA / NEJ | yes / no | right end of the number row |

- `=` in row GP20 is the `=` key in the top right. `-` in row GP9 is the `=`/`-` key left of the right Shift.
- `R` in row GP21 is the R key in the top right, not the letter R.
- Both Shift keys are on the same wire and cannot be told apart.
- **Uncertain:** `+`, `É` and `VISA` gave different results in different scans. Check them in Vial's *Matrix tester*.
- Without diodes, an extra key can sometimes register when three or more keys are held down at the same time.

## Wiring the keyboard to the Pico

### Keyboard connectors

The keyboard's flex cables end in four blocks of screw terminals:

| Block | Pins | Contents |
|---|---|---|
| 1 | 14 | key matrix |
| 2 | 13 | key matrix |
| 3 | 6 | key matrix and other |
| 4 | 3 | probably the LEDs (telephone, LOCK, TILL), not connected |

The terminal pins sit in a fixed row and cannot be plugged in right next to the Pico. If they are, some of them end up on the Pico's GND pins. Put the terminals on a separate breadboard instead and run jumper wires to the correct Pico pin.

### Wiring order

Terminal pins are numbered left to right with the flex cable facing you. Pico pins are counted from the USB end. Pins 1–20 are on one side and 21–40 on the other, with pin 21 opposite pin 20.

| Terminal block | Pin | GP | Pico pin |
|---|---|---|---|
| 14-block | 1 | GP0 | 1 |
| | 2 | GP1 | 2 |
| | 3 | GP2 | 4 |
| | 4 | GP3 | 5 |
| | 5 | GP4 | 6 |
| | 6 | GP5 | 7 |
| | 7 | GP6 | 9 |
| | 8 | GP7 | 10 |
| | 9 | GP8 | 11 |
| | 10 | GP9 | 12 |
| | 11 | GP10 | 14 |
| | 12 | GP11 | 15 |
| | 13 | GP12 | 16 |
| | 14 | GP13 | 17 |
| 13-block | 1 | GP14 | 19 |
| | 2 | GP15 | 20 |
| | 3 | GP16 | 21 |
| | 4 | GP17 | 22 |
| | 5 | GP18 | 24 |
| | 6 | GP19 | 25 |
| | 7 | GP20 | 26 |
| | 8 | GP21 | 27 |
| | 9 | GP22 | 29 |
| | 10 | GP26 | 31 |
| | 11 | GP27 | 32 |
| | 12 | GP28 | 34 |
| | 13 | (not connected) | |
| 6-block | 1–6 | (see below) | |
| 3-block | 1–3 | (not connected) | |

> This table shows the planned wiring. Check it against your actual setup, especially how the 6-block and the 13th pin are connected. The firmware only uses GP0–GP21. GP22 and GP26–GP28 have not produced any keys.
>
> The wires to **GP14 and GP15** (pins 19 and 20) sit next to each other and are easy to mix up. If U/P, J/Ö or M/- come out swapped, check those first.

### Pins that must never be used

| Pico pin | Function |
|---|---|
| 3, 8, 13, 18, 23, 28, 33, 38 | GND (square solder pads) |
| 30 | RUN (reset) |
| 35 | ADC_VREF |
| 36 | 3V3 |
| 37 | 3V3_EN |
| 39 | VSYS |
| 40 | VBUS (5 V from USB) |

If a keyboard wire sits on GND, several keys merge and cannot be told apart. If one sits on 3V3, VSYS or VBUS, a key press can short the power supply.
