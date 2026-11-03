# ECE 353 — PSoC Control MCU coursework

Embedded-systems coursework for ECE 353 at UW–Madison, built on an Infineon PSoC Control MCU
board with ModusToolbox and FreeRTOS.

## Final project: two-player code-cracking game

Two boards play against each other over a UART link. Each player secretly picks a 4-digit
cipher (digits 0–7). Players take turns guessing the opponent's cipher and get back
**exact** (right digit, right place) and **near** (right digit, wrong place) counts, like
Mastermind. First to crack the other board's cipher wins, and both boards then show the
guess count.

What's in it:

- **Custom IPC protocol** over UART: start byte, command type, sequence number, payload and
  CRC. Commands cover discovery, ACK, cipher-ready, guess, feedback, reset and win
  (`src/tasks/task_ipc.h`).
- **Turn-order negotiation:** each board sends a random token and the higher one goes first,
  retrying on a tie.
- **Per-position feedback** packed into a single byte (2 bits × 4 positions).
- **FreeRTOS task structure:** separate tasks for IPC receive/transmit, LCD, buttons, joystick,
  capacitive touch, IMU, buzzer, EEPROM and the sensors, synchronised with event groups
  (`src/tasks/`).
- **Peripheral drivers** (`src/drivers/`): LCD, SPI, I2C, IMU, EEPROM, joystick, buttons, LEDs,
  buzzer, capacitive touch and a circular buffer.

## Other coursework

- `src/ice/` — in-class exercises (ICE 01–14)
- `src/examples/` — example programs

## Building

Requires [ModusToolbox](https://www.infineon.com/modustoolbox) 3.3 or later.

```
make getlibs     # downloads the libraries listed in deps/ into libs/
make build
make program
```

`libs/` and `build/` are not committed. The original Infineon template README is kept in
`README-infineon-template.md`.

## Credits

The project skeleton and the starter drivers and templates come from the course staff and
Infineon; their credits and licenses are kept in the file headers. The final-project game
logic and IPC protocol are my own work.
