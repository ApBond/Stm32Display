#ifndef __SSD1306
#define __SSD1306

#include "stm32f10x.h"
#include "stdlib.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define BUFFER_SIZE 1024

#define CS_SET GPIOA->BSRR|=GPIO_BSRR_BS2
#define CS_RES GPIOA->BSRR|=GPIO_BSRR_BR2
#define RESET_SET GPIOA->BSRR|=GPIO_BSRR_BS1
#define RESET_RES GPIOA->BSRR|=GPIO_BSRR_BR1
#define DATA GPIOA->BSRR|=GPIO_BSRR_BS3
#define COMMAND GPIOA->BSRR|=GPIO_BSRR_BR3

typedef enum COLOR
{
	BLACK,
	WHITE
}COLOR;

void spi1Init(void);
void spiTransmit(uint8_t data);
void ssd1306RunDisplayUPD(void);
void ssd1306StopDispayUPD(void);
void ssd1306SendCommand(uint8_t command);
void ssd1306SendData(uint8_t data);
void ssd1306Init(void);
void ssd1306DrawPixel(uint16_t x, uint16_t y,COLOR color);
void ssd1306FillDisplay(COLOR color);
#endif