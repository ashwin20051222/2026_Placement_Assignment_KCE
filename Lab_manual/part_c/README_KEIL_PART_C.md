## PART C (STM32F103RB, Keil uVision) - Source Package

These examples are written for **STM32F103RB** (Cortex‑M3) and intended to be built in **Keil uVision** using ST device packs / CMSIS headers.

### Notes (matching your style request)
- No `static` helper functions in the `.c` files here (only normal `void`/`int` functions).
- No `typedef` or `#define` are introduced in these example sources.
- ST/CMSIS headers will still contain typedefs/macros internally (that’s normal for STM32).

### How to use in Keil
For each question file:
- Create a new Keil project for STM32F103RB.
- Add the matching `main_XX_*.c` file (and the `.s` for Q1–Q5 if you want assembly).
- Make sure the project uses the correct device startup and CMSIS pack.
- Select an output interface (ST‑Link) and flash.

### Files
- `01_add_32bit.s` + `main_01_add_32bit.c`
- `02_fibonacci_10_terms.s` + `main_02_fibonacci.c`
- `03_gpio_toggle_bitops.s` + `main_03_gpio_toggle.c`
- `04_delay_loop.s` + `main_04_delay_loop.c`
- `05_mul_div.s` + `main_05_mul_div.c`
- `main_06_led_blink.c`
- `main_07_lcd_16x2_4bit_hello.c`
- `main_08_dc_motor_direction.c`
- `main_09_keypad_4x4_lcd.c`
- `main_10_relay_toggle.c`
- `main_11_dht11_lcd.c`
- `main_12_i2c_lcd_sensor_values.c`
- `main_13_pwm_timer_motor_speed.c`
- `main_14_uart_two_boards_echo.c`
- `main_15_adc_read_analog.c`
