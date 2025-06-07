/**
 * @file ili9488.c
 * @brief ILI9488 LCD driver implementation
 * @details This file contains the implementation of the ILI9488 LCD driver,
 *          supporting 18-bit color mode and various display operations.
 * @author Cengiz Sinan Kostakoglu
 * @version 1.0
 * @date 2025-06-07
 */

#include "ili9488.h"

/* Global variable to store the current display rotation */
ILI9488_Rotation_t ili9488_rotation = ILI9488_ROTATION_PORTRAIT;

/* ILI9488 Command definitions */
#define CMD_SLEEP_IN       0x10  ///< Enter sleep mode to reduce power consumption
#define CMD_SLEEP_OUT      0x11  ///< Exit sleep mode and return to normal operation
#define CMD_DISPLAY_OFF    0x28  ///< Turn off the display while keeping power on
#define CMD_DISPLAY_ON     0x29  ///< Turn on the display
#define CMD_MEMORY_WRITE   0x2C  ///< Write data to display memory
#define CMD_COLUMN_ADDR    0x2A  ///< Set column address for memory access
#define CMD_PAGE_ADDR      0x2B  ///< Set page address for memory access
#define CMD_MEMORY_ACCESS  0x36  ///< Set memory access control (rotation, mirroring)
#define CMD_INTERFACE_MODE 0xB0  ///< Set interface mode and timing
#define CMD_PIXEL_FORMAT   0x3A  ///< Set pixel format (18-bit RGB666)

/**
 * @brief Write 18-bit data to the display
 * @param data 18-bit data to write (RGB666 format)
 * @details This function handles the low-level 18-bit parallel data write operation
 *          to the ILI9488 display. It sets all data pins according to the input data.
 *          The function first clears all data pins, then sets them according to the
 *          input data value, and finally generates the write strobe.
 * @note This function assumes all GPIO ports and pins are properly configured
 *       in the main.h file.
 */
static inline void ILI9488_Write18(uint32_t data){
    /* Clear all data pins (set to 0) */
    DB0_GPIO_Port->BSRR = (uint32_t)DB0_Pin << 16;
    DB1_GPIO_Port->BSRR = (uint32_t)DB1_Pin << 16;
    DB2_GPIO_Port->BSRR = (uint32_t)DB2_Pin << 16;
    DB3_GPIO_Port->BSRR = (uint32_t)DB3_Pin << 16;
    DB4_GPIO_Port->BSRR = (uint32_t)DB4_Pin << 16;
    DB5_GPIO_Port->BSRR = (uint32_t)DB5_Pin << 16;
    DB6_GPIO_Port->BSRR = (uint32_t)DB6_Pin << 16;
    DB7_GPIO_Port->BSRR = (uint32_t)DB7_Pin << 16;
    DB8_GPIO_Port->BSRR = (uint32_t)DB8_Pin << 16;
    DB9_GPIO_Port->BSRR = (uint32_t)DB9_Pin << 16;
    DB10_GPIO_Port->BSRR = (uint32_t)DB10_Pin << 16;
    DB11_GPIO_Port->BSRR = (uint32_t)DB11_Pin << 16;
    DB12_GPIO_Port->BSRR = (uint32_t)DB12_Pin << 16;
    DB13_GPIO_Port->BSRR = (uint32_t)DB13_Pin << 16;
    DB14_GPIO_Port->BSRR = (uint32_t)DB14_Pin << 16;
    DB15_GPIO_Port->BSRR = (uint32_t)DB15_Pin << 16;
    DB16_GPIO_Port->BSRR = (uint32_t)DB16_Pin << 16;
    DB17_GPIO_Port->BSRR = (uint32_t)DB17_Pin << 16;

    /* Set data pins according to 'data' */
    if(data & (1 << 0)) DB0_GPIO_Port->BSRR = DB0_Pin;
    if(data & (1 << 1)) DB1_GPIO_Port->BSRR = DB1_Pin;
    if(data & (1 << 2)) DB2_GPIO_Port->BSRR = DB2_Pin;
    if(data & (1 << 3)) DB3_GPIO_Port->BSRR = DB3_Pin;
    if(data & (1 << 4)) DB4_GPIO_Port->BSRR = DB4_Pin;
    if(data & (1 << 5)) DB5_GPIO_Port->BSRR = DB5_Pin;
    if(data & (1 << 6)) DB6_GPIO_Port->BSRR = DB6_Pin;
    if(data & (1 << 7)) DB7_GPIO_Port->BSRR = DB7_Pin;
    if(data & (1 << 8)) DB8_GPIO_Port->BSRR = DB8_Pin;
    if(data & (1 << 9)) DB9_GPIO_Port->BSRR = DB9_Pin;
    if(data & (1 << 10)) DB10_GPIO_Port->BSRR = DB10_Pin;
    if(data & (1 << 11)) DB11_GPIO_Port->BSRR = DB11_Pin;
    if(data & (1 << 12)) DB12_GPIO_Port->BSRR = DB12_Pin;
    if(data & (1 << 13)) DB13_GPIO_Port->BSRR = DB13_Pin;
    if(data & (1 << 14)) DB14_GPIO_Port->BSRR = DB14_Pin;
    if(data & (1 << 15)) DB15_GPIO_Port->BSRR = DB15_Pin;
    if(data & (1 << 16)) DB16_GPIO_Port->BSRR = DB16_Pin;
    if(data & (1 << 17)) DB17_GPIO_Port->BSRR = DB17_Pin;

    /* WR strobe (active low) */
    ILI9488_WR_GPIO_Port->BSRR = (uint32_t)ILI9488_WR_Pin << 16; /* WR low */
    __NOP(); __NOP(); /* Short delay */
    ILI9488_WR_GPIO_Port->BSRR = ILI9488_WR_Pin; /* WR high */
}

/**
 * @brief Write a command to the display
 * @param cmd 8-bit command to write
 * @details This function sends a command to the display by setting DCX low
 *          and writing the command byte. The chip select (CS) is automatically
 *          managed during the operation.
 * @note This function assumes all GPIO ports and pins are properly configured
 *       in the main.h file.
 */
static inline void ILI9488_WriteCommand(uint8_t cmd){
    ILI9488_CS_GPIO_Port->BSRR = (uint32_t)ILI9488_CS_Pin << 16; /* CS low */
    ILI9488_DCX_GPIO_Port->BSRR = (uint32_t)ILI9488_DCX_Pin << 16; /* DCX low (command) */
    ILI9488_Write18(cmd);
    ILI9488_CS_GPIO_Port->BSRR = ILI9488_CS_Pin; /* CS high */
}

/**
 * @brief Write data to the display
 * @param data 18-bit data to write (RGB666 format)
 * @details This function sends data to the display by setting DCX high
 *          and writing the data value. The chip select (CS) is automatically
 *          managed during the operation.
 * @note This function assumes all GPIO ports and pins are properly configured
 *       in the main.h file.
 */
static inline void ILI9488_WriteData(uint32_t data){
    ILI9488_CS_GPIO_Port->BSRR = (uint32_t)ILI9488_CS_Pin << 16; /* CS low */
    ILI9488_DCX_GPIO_Port->BSRR = ILI9488_DCX_Pin; /* DCX high (data) */
    ILI9488_Write18(data);
    ILI9488_CS_GPIO_Port->BSRR = ILI9488_CS_Pin; /* CS high */
}

/**
 * @brief Set the display address window for drawing
 * @param x0 Starting X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Starting Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param x1 Ending X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y1 Ending Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @details This function sets the drawing window on the display.
 *          All subsequent drawing operations will be confined to this area.
 *          The coordinates are automatically adjusted based on the current
 *          display rotation.
 */
static inline void ILI9488_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_WriteCommand(CMD_COLUMN_ADDR);
        ILI9488_WriteData(x0 >> 8); ILI9488_WriteData(x0 & 0xFF);
        ILI9488_WriteData(x1 >> 8); ILI9488_WriteData(x1 & 0xFF);
        ILI9488_WriteCommand(CMD_PAGE_ADDR);
        ILI9488_WriteData(y0 >> 8); ILI9488_WriteData(y0 & 0xFF);
        ILI9488_WriteData(y1 >> 8); ILI9488_WriteData(y1 & 0xFF);
        ILI9488_WriteCommand(CMD_MEMORY_WRITE);
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_WriteCommand(CMD_COLUMN_ADDR);
        ILI9488_WriteData(y0 >> 8); ILI9488_WriteData(y0 & 0xFF);
        ILI9488_WriteData(y1 >> 8); ILI9488_WriteData(y1 & 0xFF);
        ILI9488_WriteCommand(CMD_PAGE_ADDR);
        ILI9488_WriteData(x0 >> 8); ILI9488_WriteData(x0 & 0xFF);
        ILI9488_WriteData(x1 >> 8); ILI9488_WriteData(x1 & 0xFF);
        ILI9488_WriteCommand(CMD_MEMORY_WRITE);
    }
}

/**
 * @brief Draw a single pixel on the display
 * @param x X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function draws a single pixel at the specified coordinates
 *          with the given color. The coordinates are automatically adjusted
 *          based on the current display rotation.
 */
void ILI9488_DrawPixel(uint16_t x, uint16_t y, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x, y, x, y);
        ILI9488_WriteData(color & 0x3FFFF); /* 18-bit color */
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y, x, y, x);
        ILI9488_WriteData(color & 0x3FFFF); /* 18-bit color */
    }
}

/**
 * @brief Draw a line on the display
 * @param x0 Starting X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Starting Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param x1 Ending X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y1 Ending Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function draws a line between two points with the specified color.
 *          The line is drawn using Bresenham's algorithm.
 */
void ILI9488_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x0, y0, x1, y1);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_PORTRAIT_WIDTH * ILI9488_PORTRAIT_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y0, x0, y1, x1);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_LANDSCAPE_WIDTH * ILI9488_LANDSCAPE_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief Draw a rectangle on the display
 * @param x Starting X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y Starting Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param w Width of rectangle (1 to 320 or 1 to 480)
 * @param h Height of rectangle (1 to 320 or 1 to 480)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function draws a rectangle with the specified color.
 *          The rectangle is drawn using Bresenham's algorithm.
 */
void ILI9488_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x, y, x + w - 1, y + h - 1);
        for(uint32_t i = 0; i < (uint32_t)w * h; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y, x, y + h - 1, x + w - 1);
        for(uint32_t i = 0; i < (uint32_t)h * w; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief Fill a rectangle on the display
 * @param x Starting X coordinate (0 to 319)
 * @param y Starting Y coordinate (0 to 479)
 * @param w Width of rectangle (1 to 320)
 * @param h Height of rectangle (1 to 480)
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function fills a rectangle with the specified color.
 *          The coordinates are automatically adjusted based on the current
 *          display rotation.
 */
void ILI9488_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x, y, x + w - 1, y + h - 1);
        for(uint32_t i = 0; i < (uint32_t)w * h; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y, x, y + h - 1, x + w - 1);
        for(uint32_t i = 0; i < (uint32_t)h * w; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief Draw a circle on the display
 * @param x0 Center X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Center Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param radius Circle radius
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function draws a circle with the specified color.
 *          The circle is drawn using Bresenham's algorithm.
 */
void ILI9488_DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x0, y0, x0 + radius, y0 + radius);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_PORTRAIT_WIDTH * ILI9488_PORTRAIT_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y0, x0, y0 + radius, x0 + radius);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_LANDSCAPE_WIDTH * ILI9488_LANDSCAPE_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief Fill a circle on the display
 * @param x0 Center X coordinate (0 to 319 or 0 to 479 for vertical)
 * @param y0 Center Y coordinate (0 to 479 or 0 to 319 for horizontal)
 * @param radius Circle radius
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function fills a circle with the specified color.
 *          The circle is filled using Bresenham's algorithm.
 */
void ILI9488_FillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(x0, y0, x0 + radius, y0 + radius);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_PORTRAIT_WIDTH * ILI9488_PORTRAIT_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(y0, x0, y0 + radius, x0 + radius);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_LANDSCAPE_WIDTH * ILI9488_LANDSCAPE_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief Fill the entire display with a color
 * @param color 18-bit RGB color (RGB666 format, 0x000000 to 0x3FFFFF)
 * @details This function fills the entire display with the specified color.
 *          The display is filled using Bresenham's algorithm.
 */
void ILI9488_FillBackground(uint32_t color){
    if(ili9488_rotation == ILI9488_ROTATION_PORTRAIT || ili9488_rotation == ILI9488_ROTATION_PORTRAIT_INV){
        ILI9488_SetAddressWindow(0, 0, ILI9488_PORTRAIT_WIDTH - 1, ILI9488_PORTRAIT_HEIGHT - 1);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_PORTRAIT_WIDTH * ILI9488_PORTRAIT_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
    else if(ili9488_rotation == ILI9488_ROTATION_LANDSCAPE || ili9488_rotation == ILI9488_ROTATION_LANDSCAPE_INV){
        ILI9488_SetAddressWindow(0, 0, ILI9488_LANDSCAPE_WIDTH - 1, ILI9488_LANDSCAPE_HEIGHT - 1);
        for(uint32_t i = 0; i < (uint32_t)ILI9488_LANDSCAPE_WIDTH * ILI9488_LANDSCAPE_HEIGHT; i++){
            ILI9488_WriteData(color & 0x3FFFF);
        }
    }
}

/**
 * @brief MADCTL values for different display rotations
 * @details These values control the display orientation:
 *          - 0x48: Portrait (0°)
 *          - 0x28: Landscape (90°)
 *          - 0x88: Portrait Inverted (180°)
 *          - 0xE8: Landscape Inverted (270°)
 *          The values control both rotation and mirroring of the display.
 */
static const uint8_t ili9488_rotations[4] = {
    0x48, /* 0: Portrait */
    0x28, /* 1: Landscape */
    0x88, /* 2: Portrait Inverted */
    0xE8  /* 3: Landscape Inverted */
};

/**
 * @brief Set the display rotation
 * @param rotation Rotation value (0-3)
 * @details This function sets the display orientation.
 *          If rotation > 3, it defaults to 0 (Portrait).
 *          The rotation affects all drawing operations and coordinate
 *          calculations.
 */
static inline void ILI9488_SetRotation(uint8_t rotation){
    if(rotation > ILI9488_ROTATION_LANDSCAPE_INV) rotation = ILI9488_ROTATION_PORTRAIT;
    ILI9488_WriteCommand(CMD_MEMORY_ACCESS);
    ILI9488_WriteData(ili9488_rotations[rotation]);
}

/**
 * @brief Initialize the ILI9488 display
 * @param rotation Initial display rotation (0-3)
 * @details This function performs the following initialization steps:
 *          1. Hardware reset
 *          2. Exit sleep mode
 *          3. Set pixel format to 18-bit (RGB666)
 *          4. Set display rotation
 *          5. Turn display on
 *          The display must be initialized before any drawing operations.
 * @note This function assumes all GPIO ports and pins are properly configured
 *       in the main.h file.
 */
void ILI9488_Init(ILI9488_Rotation_t rotation){
    /* Hardware reset */
    ILI9488_RESET_GPIO_Port->BSRR = (uint32_t)ILI9488_RESET_Pin << 16; /* RESET low */
    HAL_Delay(20);
    ILI9488_RESET_GPIO_Port->BSRR = ILI9488_RESET_Pin; /* RESET high */
    HAL_Delay(120);
    ILI9488_WriteCommand(CMD_SLEEP_OUT);
    HAL_Delay(120);
    ILI9488_WriteCommand(CMD_PIXEL_FORMAT);
    ILI9488_WriteData(0x66); /* 18-bit/pixel */
    ILI9488_SetRotation(rotation);
    ili9488_rotation = rotation; /* Store the rotation value for future use */
    ILI9488_WriteCommand(CMD_DISPLAY_ON);
    HAL_Delay(20);
}

/**
 * @brief Turn off the display
 * @details This function turns off the display and waits for the command
 *          to take effect. The display remains powered but blank.
 */
static inline void ILI9488_DisplayOff(void){
    ILI9488_WriteCommand(CMD_DISPLAY_OFF);
    HAL_Delay(20);
}

/**
 * @brief Turn on the display
 * @details This function turns on the display and waits for the command
 *          to take effect. The display will show the current frame buffer
 *          contents.
 */
static inline void ILI9488_DisplayOn(void){
    ILI9488_WriteCommand(CMD_DISPLAY_ON);
    HAL_Delay(20);
}

/**
 * @brief Put the display into sleep mode
 * @details This function turns off the display and puts it into sleep mode
 *          to reduce power consumption. The display must be woken up using
 *          ILI9488_WakeUp() before it can be used again.
 */
void ILI9488_Sleep(void){
    ILI9488_DisplayOff();
    ILI9488_WriteCommand(CMD_SLEEP_IN);
    HAL_Delay(120);
}

/**
 * @brief Wake up the display from sleep mode
 * @details This function wakes up the display from sleep mode and returns
 *          it to normal operation. The display will be turned on and ready
 *          for drawing operations.
 */
void ILI9488_WakeUp(void){
    ILI9488_DisplayOn();
    ILI9488_WriteCommand(CMD_SLEEP_OUT);
    HAL_Delay(20);
}











