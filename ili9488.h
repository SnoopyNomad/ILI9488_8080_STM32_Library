/**
 * @file ili9488.h
 * @brief ILI9488 LCD driver header
 * @details This header file contains the declarations and definitions for the
 *          ILI9488 LCD driver, supporting 18-bit color mode and various
 *          display operations.
 * @author Cengiz Sinan Kostakoglu
 * @version 1.0
 * @date 2025-06-07
 */

#ifndef __ILI9488_H
#define __ILI9488_H

#ifdef __cplusplus
extern "C" {
#endif

/* For uint8_t, uint16_t, uint32_t */
#include <stdint.h> 
 /* For GPIO definitions */
#include "main.h"

/* Display dimensions */
#define ILI9488_PORTRAIT_WIDTH       320
#define ILI9488_PORTRAIT_HEIGHT      480
#define ILI9488_LANDSCAPE_WIDTH      480
#define ILI9488_LANDSCAPE_HEIGHT     320

/* Color definitions (RGB666 format) */
#define ILI9488_BLACK       0x000000
#define ILI9488_WHITE       0x3FFFFF
#define ILI9488_RED         0x3F0000
#define ILI9488_GREEN       0x003F00
#define ILI9488_BLUE        0x00003F
#define ILI9488_YELLOW      0x3F3F00
#define ILI9488_CYAN        0x003F3F
#define ILI9488_MAGENTA     0x3F003F

/* Display rotation values */
typedef enum {
    ILI9488_ROTATION_PORTRAIT = 0,
    ILI9488_ROTATION_LANDSCAPE = 1,
    ILI9488_ROTATION_PORTRAIT_INV = 2,
    ILI9488_ROTATION_LANDSCAPE_INV = 3
} ILI9488_Rotation_t;

/**
 * @brief Initialize the ILI9488 display
 * @param rotation Initial display rotation (0-3)
 * @details This function performs the following initialization steps:
 *          1. Hardware reset
 *          2. Exit sleep mode
 *          3. Set pixel format to 18-bit (RGB666)
 *          4. Set display rotation
 *          5. Turn display on
 */
void ILI9488_Init(uint8_t rotation);

/**
 * @brief Draw a single pixel on the display
 * @param x X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_DrawPixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Draw a line on the display
 * @param x0 Starting X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Starting Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param x1 Ending X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y1 Ending Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);

/**
 * @brief Draw a rectangle on the display
 * @param x Starting X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y Starting Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param w Width of rectangle (1 to 320 or 1 to 480)
 * @param h Height of rectangle (1 to 320 or 1 to 480)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);

/**
 * @brief Fill a rectangle on the display
 * @param x Starting X coordinate (0 to 319)
 * @param y Starting Y coordinate (0 to 479)
 * @param w Width of rectangle (1 to 320)
 * @param h Height of rectangle (1 to 480)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);

/**
 * @brief Draw a circle on the display
 * @param x0 Center X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Center Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param radius Circle radius
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color);

/**
 * @brief Fill a circle on the display
 * @param x0 Center X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Center Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param radius Circle radius
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_FillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color);

/**
 * @brief Fill the entire display with a color
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 */
void ILI9488_FillBackground(uint32_t color);

/**
 * @brief Put the display into sleep mode
 * @details This function turns off the display and puts it into sleep mode
 *          to reduce power consumption.
 */
void ILI9488_Sleep(void);

/**
 * @brief Wake up the display from sleep mode
 * @details This function wakes up the display from sleep mode and returns
 *          it to normal operation.
 */
void ILI9488_WakeUp(void);

#ifdef __cplusplus
}
#endif

#endif /* __ILI9488_H */
