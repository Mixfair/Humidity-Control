/****
* Программа для МК STM32F100, реализующая полив растения в зависимости
* от показаний датчика влажности почвы. Взаимодействует с МК esp12e по 
* технологии UART. 
*
* Evgeny Olisevich
**/

#include "stdbool.h"
#include "main.h"
#include "string.h"
#include "stdlib.h"

uint32_t var;
char RxBuffer[256];					
char TxBuffer[256];					
bool ComReceived;
bool force_engine;
char cmd[256] = "turn_off"; 
char cmd2[256] = "turn_on"; 
char cmd3[256] = "sensor_val_get";
char cmd4[256] = "sensor_val_set";
char cmd5[256] = "sensor_val_current_get";
char msg1[256] = "turned off";
char msg2[256] = "turned on";
uint16_t sensor_val = 500;
uint16_t i;
uint16_t check;
uint16_t volatile tmp;




int main(void){
	initButtons();
	initUsart();
	initADC();
	initPWM();
	force_engine = false;

	
	while(1){

		if (ComReceived) ExecuteCommand();
		
		
		tmp = ADC1->JDR1;
		if ((tmp<sensor_val) && !(force_engine)){
				setPwm();
		} else if(tmp>sensor_val && !(force_engine)) {
				TIM1->CCR4 = 1000;
		}
	
	}
	
	
}

bool checkCmd(char *cmd_in){ // Обработчик команд с веб-сервера
	uint16_t	c = 0;
	uint16_t	j = 0;
	for (j=0;j<strlen(cmd_in);j++) c = c + (RxBuffer[j] == cmd_in[j]);
	if (c == j) return true;
	return false;
}

void initPWM(){ // Инициализация ШИМ
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_AFIOEN;
        
	GPIOA->CRH &= ~GPIO_CRH_CNF11;
	GPIOA->CRH |= GPIO_CRH_CNF11_1;

	GPIOA->CRH	&= ~GPIO_CRH_MODE11;
	GPIOA->CRH	|= GPIO_CRH_MODE11_1;
	
	TIM1->PSC = 72;
  	TIM1->ARR = 1000;
	TIM1->CCR4 = 900;
	TIM1->CCER |= TIM_CCER_CC4E | TIM_CCER_CC4P;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1; 
      
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->CR1 &= TIM_CR1_CMS_0;
	TIM1->CR1 |= TIM_CR1_CEN;
	
}

void setPwm(){ // Формирование ШИМ сигнала для включения помпы
	uint16_t as;
	uint16_t pwm;
	as=sensor_val-tmp;
	
	if (as < 500){
		pwm = 1000 - (as * 2);
				
	} else {
		pwm = 10;
	}
		TIM1->CCR4 = pwm;
	
}

void ExecuteCommand(){ // Реализация функционала команд
		
	if	(checkCmd(cmd)) {
			TIM1->CCR4 = 1000;
			force_engine = false;
			memset(TxBuffer,0,256);
			memset(RxBuffer,0,256);
			memmove(TxBuffer,"off\r",256);
			send_to_uart(TxBuffer);
			memset(TxBuffer,0,256);
			
			ComReceived = false;
			
			return;
	}
	
	if	(checkCmd(cmd2)) {
			TIM1->CCR4 = 10;
			force_engine = true;
			memset(TxBuffer,0,256);
			memset(RxBuffer,0,256);
			memmove(TxBuffer,"on\r",256);
			send_to_uart(TxBuffer);
			memset(TxBuffer,0,256);
			ComReceived = false;
			return;
	}
	
	if	(checkCmd(cmd3)) {
			memset(TxBuffer,0,256);
			memset(RxBuffer,0,256);
			char sensor_buffer[32];
			sprintf(sensor_buffer, "%d", sensor_val);
			sensor_buffer[strlen(sensor_buffer)]='\r';
			memmove(TxBuffer,sensor_buffer,256);
			send_to_uart(TxBuffer);
			memset(TxBuffer,0,256);
			ComReceived = false;
			return;
	}
	
	if	(checkCmd(cmd4)) {

			sensor_val = getVal(RxBuffer);
			memset(RxBuffer,0,256);
			
			memset(TxBuffer,0,256);
			memmove(TxBuffer,"ok\r",256);
			send_to_uart(TxBuffer);
			memset(TxBuffer,0,256);
			ComReceived = false;
			return;
	}
	
		if	(checkCmd(cmd5)) {

	
			memset(RxBuffer,0,256);
			
			memset(TxBuffer,0,256);
			char sensor_buffer[32];
			sprintf(sensor_buffer, "%d", tmp);
			sensor_buffer[strlen(sensor_buffer)]='\r';
			memmove(TxBuffer,sensor_buffer,256);
			send_to_uart(TxBuffer);
			memset(TxBuffer,0,256);
			ComReceived = false;
			return;
	}
	
	memset(RxBuffer,0,256);

	ComReceived = false;	;	
	
}

int getVal(char *str){
		uint16_t i;
		char value[256];
		uint16_t result;
	
		for (i = strlen(str); i >= 0; i--)
		{
			
			if (str[i] == '_') {
				break;
			}
			value[strlen(str)-i-1] = str[i];
		}
		value[strlen(value)] = '\0';
		reverseStr(value);
		
		result = atoi(value);
		memset(value, 0, 256);
		return result;
}

void reverseStr(char *str) {
	uint16_t i;
	char change;
	
	for (i = 0; (i < (strlen(str) / 2)); i++) {
		change = str[i];
		str[i] = str[strlen(str)-i-1];
		str[strlen(str)-i-1] = change;
	}
	
}

void send_to_uart(char *str) {  // Отправить команду на веб-сервер
  uint16_t i;

	for (i = 0; i < strlen(str); i++)
	{
		USART1->DR = str[i];						
		while ((USART1->SR & USART_SR_TC)==0) {};
	}
}

void initUsart(void){
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
	GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1; //tx
	
	GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);		//rx-
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	

	USART1->BRR=0x9c4;



	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
	USART1->CR1 |= USART_CR1_RXNEIE;
	
	NVIC_EnableIRQ(USART1_IRQn);

}

void initADC(){  // Инициализация ЦАП для получения данных с датчика
	
	GPIOA->CRL 	 &= ~GPIO_CRL_MODE0; 
	GPIOA->CRL   &= ~GPIO_CRL_CNF0;
	
	RCC->APB2ENR |=  RCC_APB2ENR_ADC1EN;
	RCC->CFGR &= ~RCC_CFGR_ADCPRE; 
	ADC1->CR1 =  0;
	ADC1->CR2 |=  ADC_CR2_CAL;
	while (!(ADC1->CR2 & ADC_CR2_CAL)){};
	ADC1->CR2     =  ADC_CR2_JEXTSEL;    
  ADC1->CR2    |=  ADC_CR2_JEXTTRIG;    
  ADC1->CR2    |=  ADC_CR2_CONT;     
  ADC1->CR1    |=  ADC_CR1_SCAN;        
  ADC1->CR1    |=  ADC_CR1_JAUTO;
	ADC1->JSQR    =  (uint32_t)(1-1)<<20;  
  ADC1->JSQR   |=  (uint32_t)0<<(5*0);  
	ADC1->CR2    |=  ADC_CR2_ADON;
	ADC1->CR2    |=  ADC_CR2_JSWSTART; 
}

void USART1_IRQHandler(void)
	{
		char tmp;

		if ((USART1->SR & USART_SR_RXNE)!=0)		
		{
			
		tmp=USART1->DR;	
		if (tmp==0x0D)							
		{
			
			ComReceived=true;
			
			return;							
		} 
		RxBuffer[strlen(RxBuffer)] = tmp;		
	}

}

void initButtons(void) {
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	

		GPIOB->CRL = (GPIO_CRL_MODE0  | GPIO_CRL_MODE1  );
		
	
		GPIOB->BSRR = GPIO_BSRR_BR0;
		GPIOB->BSRR = GPIO_BSRR_BS1;

	
}

void delay(uint32_t times){
	uint32_t i;
	i=0;
	while(i < times){
		i++;
	}
}