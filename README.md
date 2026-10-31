# Binary Clock

A clock that shows the time in binary-coded decimal (BCD). Each of the four columns of LEDs is one digit of HH:MM, and each column is a different color so it can be read from across the room.

Board: Binary Clock REV1.1, designed by [Just Different Creations](https://justdif.com) in Lufkin, Texas. See it on the [PCB-Only Builds](https://justdif.com/pcb-only-builds/) page.

## How it works

- An **ATtiny84A** reads the time from a **DS3231** real-time clock over I2C (bit-banged on the USI peripheral).
- The four digits (tens of hours, hours, tens of minutes, minutes) are each sent as 4 bits to a chain of shift registers that drive the LED columns.
- A CR1225 coin cell keeps the DS3231 running when power is off. The board is powered over USB-C.
- Time is shown in 24-hour format.

## Pinout (ATtiny84A)

| Pin | Function |
| --- | --- |
| PA0 | Shift register data |
| PA1 | Shift register latch |
| PA2 | Shift register clock |
| PA4 | I2C SCL (DS3231) |
| PA6 | I2C SDA (DS3231) |
| PB0 | Hour button (adds 1 hour, internal pull-up) |
| PB1 | Minute button (adds 1 minute, internal pull-up) |

## Building and flashing

- MPLAB X IDE with the XC8 compiler (built with v2.45)
- Device: ATtiny84A, running at 1 MHz (`F_CPU 1000000UL`)
- Programmer: PICkit 5 (any ISP programmer that MPLAB X supports for the ATtiny84A will do)

Open the `.X` folder in MPLAB X, build, and program the board.

## Credits

`DS3231.c`, `DS3231.h`, `USI_I2C_Master.c` and `USI_I2C_Master.h` come from [Pesc0/Clock](https://github.com/Pesc0/Clock), which is licensed under GPL-3.0.

## License

GPL-3.0. See [LICENSE](LICENSE).
