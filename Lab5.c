/*Alan Carlton*/

#include "STM32L1xx.h"
/* Define global variables */
uint16_t button = 0x00FF;
uint16_t colrow = 0x00;
unsigned char row;
unsigned char col;
int16_t output = 0xFFFF;

int arrValue = 209.60;//2096 = 1kHz, 20960 = 100Hz, 209.60 = 10kHz (switching)
int pscValue = 0;
double duty;

/* PinSetup function - Sets the pins to their desired values */

void PinSetup() {
//Clock for GPIOA
RCC->AHBENR |= 0x01;
//Clear bits for GPIOA
GPIOA->MODER &= ~(0x000003000);
GPIOA->MODER |= 0x00002000; //PA6 = AF mode
GPIOA->AFR[0] &= ~(0x0F000000); //clear AFRL6
GPIOA->AFR[0] |= 0x03000000; //PA6 = AF2

//Clock for GPIOB
RCC->AHBENR |= 0x02;
//Clear bits for GPIOB
GPIOB->MODER &= ~(0x0000FFFF);
GPIOB->PUPDR &= ~(0x0000FFFF);
GPIOB->PUPDR |=  (0x00005500); // pull up resistors for PB4-7, pull down for resistors PB3-0
GPIOB->MODER |= (0x00000055); // sets columns as outputs
GPIOB->ODR &= 0xFFFFFF00;

//Clock for GPIOC
RCC->AHBENR |= 0x04;
//Clears bits that you want to use for GPIOC
GPIOC->MODER &= ~(0x0000FFFF); // Last 4 hex =(%1111 1111 1111 1111)
//Overwrite the bits to do what you want
GPIOC->MODER |= (0x00005555); // Last 5 hex =(%0101 0101 0101 0101)
GPIOC->BSRR = 0x00FF; // sets GPIOC0-7 high


NVIC_EnableIRQ(EXTI1_IRQn);
NVIC_ClearPendingIRQ(EXTI1_IRQn);

EXTI->FTSR |= 0x0002;
EXTI->IMR |= 0x0002;
EXTI->PR |= 0x0002;
SYSCFG->EXTICR[0] &= 0xFF0F; //Clears EXTI1 bit field
SYSCFG->EXTICR[0] |= 0x0010;

RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
/*IRQ for the timer
TIM10->DIER |= 0x1;*/

TIM10->CNT = 0x0;
TIM10->CCMR1 &= ~(0xF0);
TIM10->CCMR1 |= 0x60;
TIM10->CCER &= ~(0xF);
TIM10->CCER |= 0x1;
TIM10->CR1 &= ~(0xF);
TIM10->CR1 |= 0x1;
TIM10->PSC = pscValue;
TIM10->ARR = arrValue;

NVIC_EnableIRQ(TIM10_IRQn);
NVIC_ClearPendingIRQ(TIM10_IRQn);

}

/* Delay function - do nothing for half a second */

void delay() {
int i,j,n;
for (i=0; i<20; i++) { //outer loop
for (j=0; j<10000; j++) { //inner loop
n = j; //single-step test
} //do nothing
}

}


//ISR1

void EXTI1_IRQHandler() {
	
colrow = GPIOB->IDR;
colrow &= 0x00F0 ;
	
// determine row pressed
switch (colrow) {
	case 0x0070:
		row = 4; 
		break;
	case 0x00B0:
		row = 3; 
		break; 
	case 0x00D0:
		row = 2; 
		break; 
	case 0x00E0:
		row = 1; 
		break; 
	default:
		row = 5; 
		break;
}
GPIOB->MODER &= ~(0x0000FFFF);
GPIOB->PUPDR &= ~(0x000000FF);
GPIOB->PUPDR |=  (0x00000055); // pull up resistors for PB4-7
GPIOB->MODER |= (0x00005500); // sets columns as outputs
GPIOB->ODR &= 0xFFFF00FF;

colrow = GPIOB->IDR;
colrow &= 0x000F ;
	
// determine col pressed
switch (colrow) {
	case 0x0007:
		col = 4; 
		break;
	case 0x000B:
		col = 3; 
		break; 
	case 0x000D:
		col = 2; 
		break; 
	case 0x000E:
		col = 1; 
		break; 
	default:
		col = 4;
		break;
}
	


uint16_t array[5][4] = {
	{0x0001,0x0002,0x0003,0x000A},
	{0x0004,0x0005,0x0006,0x000B},
	{0x0007,0x0008,0x0009,0x000C},
	{0x000E,0x0000,0x000F,0x000D}, 
	{0x00FF,0x00FF,0x00FF,0x00FF}
	};
// 0x27 is '*' and 0x23 is '#'
	button = array[row-1][col-1];
	for(int k=0;k<4;k++); // does nothing just a baby delay
	//GPIOC->ODR = button; 
	
//Clear bits for GPIOB
GPIOB->MODER &= ~(0x0000FFFF);
GPIOB->PUPDR &= ~(0x0000FF00);
GPIOB->PUPDR |=  (0x00005500); // pull up resistors for PB4-7
GPIOB->MODER |= (0x00000055); // sets columns as outputs
GPIOB->ODR &= 0xFFFFFF00;
	

// New Code Below
int result = array[row-1][col-1];
unsigned char dummy = 0;
uint16_t j = 0x0A;
int temp;
for(uint16_t i = 0x00; i <= j; i++)
{
if (button == i){
temp = (int)i;
duty = 0.1* temp;
dummy++;
}

}
if (dummy) {
GPIOC->ODR &= ~(0xFF); //clear bits
GPIOC->ODR |= (button); //Write bits
}
//////////////////////////end




//Clear bits for GPIOB
//GPIOB->MODER &= ~(0x0000FFFF);
//GPIOB->PUPDR &= ~(0x0000FF00);
//GPIOB->PUPDR |=  (0x00005500); // pull up resistors for PB4-7
//GPIOB->MODER |= (0x00000055); // sets columns as outputs
//GPIOB->ODR &= 0xFFFFFF00;

NVIC_ClearPendingIRQ(EXTI1_IRQn);
EXTI->PR |= 0x0002;
NVIC_ClearPendingIRQ(EXTI1_IRQn);
EXTI->PR |= 0x0002;
}
void TIM10_TRQHandler() {
TIM10->SR &= ~(0x01);
}

/* Main program */
int main(void) {
PinSetup();     //Runs PinSetup()
duty = 0; //initializes the counter as stopped
__enable_irq();

while(1) {   //A while loop that never ends
//output &= button;
//output |= button;
//delay();   //Puts in the delay of 0.5 seconds
TIM10->CCR1 = (int)((arrValue + 1) * (pscValue + 1) * duty);
}
}
