#include "ssd1306.h"

static uint8_t displayBuff[BUFFER_SIZE];//Буфер экрана

void spi1Init()
{
	//PA5-SCK
	//PA7-MOSI
	RCC->APB2ENR|=RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;//Включить тактирование SPI1 и GPIOA
	RCC->AHBENR|=RCC_AHBENR_DMA1EN;//Включить тактирование DMA
	GPIOA->CRL|= GPIO_CRL_MODE5 | GPIO_CRL_MODE7;//PA4,PA5,PA7 в режим выходов 50MHz
	GPIOA->CRL&= ~(GPIO_CRL_CNF5 | GPIO_CRL_CNF7);
	GPIOA->CRL|=  GPIO_CRL_CNF5_1 | GPIO_CRL_CNF7_1;//PA5,PA7 - выход с альтернативной функцией push-pull, PA4 - выход push-pull
	SPI1->CR1|=SPI_CR1_MSTR;//Режим ведущего
	SPI1->CR1|= (0x00 & SPI_CR1_BR);//Делитель частоты на 2
	SPI1->CR1|=SPI_CR1_SSM;//Программный NSS
	SPI1->CR1|=SPI_CR1_SSI;//NSS - high
	SPI1->CR2|=SPI_CR2_TXDMAEN;//Разрешить запросы DMA
	SPI1->CR1|=SPI_CR1_SPE;//включить SPI1
	//Настройка DMA
	DMA1_Channel3->CCR|=DMA_CCR1_PSIZE_0;//Размер периферии 1байт
	DMA1_Channel3->CCR|=DMA_CCR1_DIR;//Режим DMA из памяти в переферию
	DMA1_Channel3->CCR|=DMA_CCR1_MINC;//Включить инкремент памяти
	DMA1_Channel3->CCR|=DMA_CCR1_PL;//Высокий приоритет DMA
	//DMA1_Channel3->CCR|=DMA_CCR1_TCIE;//Enable interrupt "End of transmitt"
	//DMA1->IFCR&=~DMA_IFCR_CTCIF3;//Reset flag
	//NVIC_EnableIRQ(DMA1_Channel3_IRQn);//Enable interrupt
	
}

void spiTransmit(uint8_t data)
{
	CS_RES;	
	SPI1->DR = data;
	while((SPI1->SR & SPI_SR_BSY))
	{};
	CS_SET;
}

void ssd1306RunDisplayUPD()
{
	DATA;
	DMA1_Channel3->CCR&=~(DMA_CCR1_EN);//Выключить DMA
	DMA1_Channel3->CPAR=(uint32_t)(&SPI1->DR);//Занесем в DMA адрес регистра данных SPI1
	DMA1_Channel3->CMAR=(uint32_t)&displayBuff;//Адрес данных
	DMA1_Channel3->CNDTR=sizeof(displayBuff);//Размер данных
	DMA1->IFCR&=~(DMA_IFCR_CGIF3);
	CS_RES;//Выбор устройства на шине
	DMA1_Channel3->CCR|=DMA_CCR1_CIRC;//Циклический режим DMA
	DMA1_Channel3->CCR|=DMA_CCR1_EN;//Включить DMA
}

void ssd1306StopDispayUPD()
{
	CS_SET;//Дезактивация устройства на шине
	DMA1_Channel3->CCR&=~(DMA_CCR1_EN);//Выключить DMA
	DMA1_Channel3->CCR&=~DMA_CCR1_CIRC;//Выключить циклический режим
}

void ssd1306SendCommand(uint8_t command)
{
	COMMAND;
	spiTransmit(command);
	DATA;
}

void ssd1306SendData(uint8_t data)
{
	spiTransmit(data);
}

void ssd1306Init()
{
	//PA1-reset
	//PA2 - CS
	//PA3-data/command
	uint16_t i;
	GPIOA->CRL|= GPIO_CRL_MODE2 |GPIO_CRL_MODE1 | GPIO_CRL_MODE3;
	GPIOA->CRL&= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3);//PA1,PA2,PA3 в режим выходоы
	//Сброс экрана и очистка буфера
	RESET_RES;
	for(i=0;i<BUFFER_SIZE;i++)
	{
		displayBuff[i]=0;
	}
	RESET_SET;
	CS_SET;//Выбор устройства на шине
	//Конфигурация экрана
	ssd1306SendCommand(0xAE); //display off
	ssd1306SendCommand(0xD5); //Set Memory Addressing Mode
	ssd1306SendCommand(0x80); //00,Horizontal Addressing Mode;01,Vertical
	ssd1306SendCommand(0xA8); //Set Page Start Address for Page Addressing
	ssd1306SendCommand(0x3F); //Set COM Output Scan Direction
	ssd1306SendCommand(0xD3); //set low column address
	ssd1306SendCommand(0x00); //set high column address
	ssd1306SendCommand(0x40); //set start line address
	ssd1306SendCommand(0x8D); //set contrast control register
	ssd1306SendCommand(0x14);
	ssd1306SendCommand(0x20); //set segment re-map 0 to 127
	ssd1306SendCommand(0x00); //set normal display
	ssd1306SendCommand(0xA1); //set multiplex ratio(1 to 64)
	ssd1306SendCommand(0xC8); //
	ssd1306SendCommand(0xDA); //0xa4,Output follows RAM
	ssd1306SendCommand(0x12); //set display offset
	ssd1306SendCommand(0x81); //not offset
	ssd1306SendCommand(0x8F); //set display clock divide ratio/oscillator frequency
	ssd1306SendCommand(0xD9); //set divide ratio
	ssd1306SendCommand(0xF1); //set pre-charge period
	ssd1306SendCommand(0xDB); 
	ssd1306SendCommand(0x40); //set com pins hardware configuration
	ssd1306SendCommand(0xA4);
	ssd1306SendCommand(0xA6); //set vcomh
	ssd1306SendCommand(0xAF); //0x20,0.77xVcc
}

void ssd1306DrawPixel(uint16_t x, uint16_t y,COLOR color){
	if(x<SSD1306_WIDTH && y <SSD1306_HEIGHT && x>=0 && y>=0)
	{
		if(color==WHITE)
		{
			displayBuff[x+(y/8)*SSD1306_WIDTH]|=(1<<(y%8));
		}
		else if(color==BLACK)
		{
			displayBuff[x+(y/8)*SSD1306_WIDTH]&=~(1<<(y%8));
		}
	}
}

void ssd1306FillDisplay(COLOR color)
{
	uint16_t i;
	for(i=0;i<SSD1306_HEIGHT*SSD1306_WIDTH;i++)
	{
		if(color==WHITE)
			displayBuff[i]=0xFF;
		else if(color==BLACK)
			displayBuff[i]=0;
	}
}
