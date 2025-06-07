# ILI9488 8080 STM32 Library

A lightweight, versatile STM32 library for driving the ILI9488 TFT LCD controller via the 8080 parallel interface. This library supports software bit-banged GPIO mode for embedded GUIs and graphics applications.

## Features

- **Display Initialization**: Configure ILI9488 orientation and color mode.
- **Hardware/Software Interface**: Use STM32 GPIO bit-bang for command/data transfers.
- **Drawing Primitives**: Pixel, rectangle.

## Prerequisites

- STM32 microcontroller with sufficient GPIO pins.
- STM32Cube HAL drivers installed in your project.
- 18 GPIO lines for D0–D17 (data bus) plus control pins: CS, DCX, WR, RST.
- Power supply and backlight control per ILI9488 datasheet.

## Installation

1. Clone this repository into your project directory:
   ```bash
   git clone https://github.com/SnoopyNomad/ILI9488_8080_STM32_Library.git
   ```

2. Copy `ili9488.c` and `ili9488.h` into your source and include folders.

3. Configure your pin definitions in `main.h`.

## Configuration

The library is configured with the following default settings:
- Display orientation: Portrait
- Color mode: 18-bit per pixel

## Usage

### Initialization

Before using the ILI9488, you need to:

1. Include `ili9488.h` in your `main.c`:
   ```c
   #include "ili9488.h"
   ```

2. Initialize GPIO in your STM32 project:
   ```c
   void SystemClock_Config(void);
   static void MX_GPIO_Init(void);

   int main(void)
   {
       HAL_Init();
       SystemClock_Config();
       MX_GPIO_Init();
       // ... rest of your initialization code
   }
   ```

3. Initialize the ILI9488:
   ```c
   ILI9488_Init(ILI9488_ROTATION_PORTRAIT);
   ```

### Drawing Primitives

The library provides functions to draw basic shapes:

```c
// Draw a pixel at (x, y) with color
ILI9488_DrawPixel(10, 80, ILI9488_RED);

// Fill a rectangle from (x1, y1) to (x2, y2) with color
ILI9488_FillRect(10, 10, 50, 80, ILI9488_BLUE);
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- ILI9488 datasheet for technical details
- STMicroelectronics for the STM32 HAL drivers