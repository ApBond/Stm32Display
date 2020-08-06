#include "main.h"


int main(void)
{
	uint8_t i,j;
	RccClockInit();
	spi1Init();
	ssd1306Init();
	ssd1306RunDisplayUPD();
	
	for(i=0;i<SSD1306_WIDTH;i++)
	{
		for(j=0;j<SSD1306_HEIGHT;j++)
		{
			if(j%10==0 || i%10==0)
				ssd1306DrawPixel(i,j,WHITE);
		}
	}
	while(1)
	{
		
	}
}

