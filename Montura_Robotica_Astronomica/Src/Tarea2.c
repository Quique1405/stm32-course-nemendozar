/**
 ******************************************************************************
 * @file           : tarea2.c
 * @author         : Nestor Enrique Mendoza Rueda
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
volatile uint32_t EVENTOS = 0;

/*#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif*/

uint32_t contador=0;
// Variable para controlar qué dígito se enciende en cada ráfaga del TIM3
volatile uint8_t posicion_digito = 0;

/******************************************
 ***FUNCION PARA CONTROLAR LOS SEGMENTOS***
 ******************************************/
void mostrar_numero(uint8_t valor) {
	// Lógica de Ánodo Común: 0 para encender el segmento, 1 para apagarlo.
	// Segmentos: A(PC3), B(PB7), C(PB10), D(PA10), E(PC4), F(PC2), G(PA8)

	switch(valor) {
		case 0:
			// Enciende todos menos G
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 4) | (0b1 << 2)); // A, E, F = 0
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR &= ~(0b1 << 10);                           // D = 0
			GPIOA->ODR |= (0b1 << 8);                              // G = 1
			break;
		case 1:
			// Enciende solo B y C
			GPIOC->ODR |= ((0b1 << 3) | (0b1 << 4) | (0b1 << 2));  // A, E, F = 1
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR |= ((0b1 << 10) | (0b1 << 8));             // D, G = 1
			break;
		case 2:
			// Enciende A, B, G, E, D
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 4));             // A, E = 0
			GPIOC->ODR |= (0b1 << 2);                             // F = 1
			GPIOB->ODR &= ~(0b1 << 7);                            // B = 0
			GPIOB->ODR |= (0b1 << 10);                            // C = 1
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		case 3:
			// Enciende A, B, G, C, D
			GPIOC->ODR &= ~(0b1 << 3);                            // A = 0
			GPIOC->ODR |= ((0b1 << 4) | (0b1 << 2));              // E, F = 1
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		case 4:
			// Enciende F, G, B, C
			GPIOC->ODR |= ((0b1 << 3) | (0b1 << 4));              // A, E = 1
			GPIOC->ODR &= ~(0b1 << 2);                            // F = 0
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR &= ~(0b1 << 8);                            // G = 0
			GPIOA->ODR |= (0b1 << 10);                            // D = 1
			break;
		case 5:
			// Enciende A, F, G, C, D
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 2));             // A, F = 0
			GPIOC->ODR |= (0b1 << 4);                             // E = 1
			GPIOB->ODR |= (0b1 << 7);                             // B = 1
			GPIOB->ODR &= ~(0b1 << 10);                           // C = 0
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		case 6:
			// Enciende A, F, E, D, C, G
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 4) | (0b1 << 2)); // A, E, F = 0
			GPIOB->ODR |= (0b1 << 7);                             // B = 1
			GPIOB->ODR &= ~(0b1 << 10);                           // C = 0
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		case 7:
			// Enciende A, B, C
			GPIOC->ODR &= ~(0b1 << 3);                            // A = 0
			GPIOC->ODR |= ((0b1 << 4) | (0b1 << 2));              // E, F = 1
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR |= ((0b1 << 10) | (0b1 << 8));             // D, G = 1
			break;
		case 8:
			// Enciende todos los segmentos
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 4) | (0b1 << 2)); // A, E, F = 0
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		case 9:
			// Enciende todos menos E
			GPIOC->ODR &= ~((0b1 << 3) | (0b1 << 2));             // A, F = 0
			GPIOC->ODR |= (0b1 << 4);                             // E = 1
			GPIOB->ODR &= ~((0b1 << 7) | (0b1 << 10));            // B, C = 0
			GPIOA->ODR &= ~((0b1 << 10) | (0b1 << 8));            // D, G = 0
			break;
		default:
			break;
	}
}

int main(void){

	/**************************
	 ***CONFIGURACIÓN LED_OK***
	 **************************/

	// Habilitamos señal de reloj GPIOC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// Limpiamos el estado de los bits reservados para el puerto 0 del registro MODER del GPIOC
	GPIOC->MODER &= ~(0b11 << 0);
	// Establecemos PC0 como salida (01)
	GPIOC->MODER |= (0b01 << 0);
	// Establecemos el tipo de salida como push pull (0)
	GPIOC->OTYPER &= ~(0b1 << 0);
	// Limpiamos el estado de la velocidad del pin
	GPIOC->OSPEEDR &= ~(0b11 << 0);
	// Establecemos la velocidad de conmutación en medio (01)
	GPIOC->OSPEEDR |= (0b01 << 0);
	// Limpiamos la configuración del registro PUPDR para establecer el estado no pull up no pull down
	GPIOC->PUPDR &= ~(0b11 << 0);


	/************************
	 ***CONFIGURACIÓN TIM2***
	 ************************/

	// Encender la señal de reloj del tim2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// Establecer el prescaler para que genere un pulso de 1ms
	TIM2->PSC = 15999;
	// Contar 500 pulsos para generar una señal de 2Hz
	TIM2->ARR = 499;
	// Limpiamos el contador
	TIM2->CNT = 0;
	// limpiamos la bandera del event-update
	TIM2->SR &= ~TIM_SR_UIF;
	// Activamos la interrupción del TIM2 por event-update
	TIM2->DIER |= TIM_DIER_UIE;
	// Matriculamos la interrupción del TIM2 en el NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
	// Activamos el contador con timer 2
	TIM2->CR1 |= TIM_CR1_CEN;


	/************************
	 ***CONFIGURACIÓN TIM3***
	 ************************/

	// Encender la señal de reloj del tim3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	// Establecer el prescaler para que genere un pulso de 1ms (mismo reloj base de 16MHz)
	TIM3->PSC = 15999;
	// Contar 4 pulsos (0 a 3) para generar una señal de ~250Hz
	TIM3->ARR = 3;
	// Limpiamos el contador
	TIM3->CNT = 0;
	// limpiamos la bandera del event-update
	TIM3->SR &= ~TIM_SR_UIF;
	// Activamos la interrupción del TIM3 por event-update
	TIM3->DIER |= TIM_DIER_UIE;
	// Matriculamos la interrupción del TIM3 en el NVIC
	NVIC_EnableIRQ(TIM3_IRQn);
	// Activamos el contador con timer 3
	TIM3->CR1 |= TIM_CR1_CEN;

	/*****************************
	 ***CONFIGURACIÓN PB1 y PB2***
	 *****************************/
	//Encender la señal de reloj para GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	// Limpiamos el estado de los bits reservados para el puerto 1 del registro MODER del GPIOB y lo establecemos como entrada
	GPIOB->MODER &= ~(0b11 <<2);
	// Limpiamos la configuración de resistencias internas y la establecemos como flotante
	GPIOB->PUPDR &= ~(0b11 << 2);
	// Limpiamos el estado de los bits reservados para el puerto 2 del registro MODER del GPIOB y lo establecemos como entrada
	GPIOB->MODER &= ~(0b11 << 4);
	// Limpiamos la configuración de resistencias internas y la establecemos como flotante
	GPIOB->PUPDR &= ~(0b11 << 4);


	/**************************************
	 *CONFIGURACIÓN EXTI - SYSCFG PARA PB1*
	 **************************************/
	// Habilitamos la señal de reloj para el periférico SYSCFG
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// Limpiamos la configuración del EXTI01
	SYSCFG->EXTICR[0] &= ~(0b1111 << 4);
	// Habilitamos la configuración el puerto PB del EXTI01
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB;
	// Habilitamos la detección de flancos de subida para PB1
	EXTI->RTSR |= EXTI_RTSR_TR1;
	// Deshabilitamos la detección de flancos de bajada para PB1
	EXTI->FTSR &= ~EXTI_FTSR_TR1;
	// Habilitamos la interrupción para la línea EXTI01
	EXTI->IMR |= EXTI_IMR_MR1;


	/**************************************
	 *CONFIGURACIÓN EXTI - SYSCFG PARA PB2*
	 **************************************/
	// Limpiamos la configuración del EXTI02
	SYSCFG->EXTICR[0] &= ~(0b1111 << 8);
	// Habilitamos la configuración el puerto PB del EXTI02
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PB;
	// Deshabilitamos la detección de flancos de subida para PB2
	EXTI->RTSR &= ~EXTI_RTSR_TR2;
	// Habilitamos la detección de flancos de bajada para PB2
	EXTI->FTSR |= EXTI_FTSR_TR2;
	// Habilitamos la interrupción para la línea EXTI02
	EXTI->IMR |= EXTI_IMR_MR2;

	/**************************************
	* CONFIGURACIÓN DEL NVIC PARA PB1  *
	**************************************/
	// Matriculamos la interrupción EXTI01 en el NVIC
	NVIC_EnableIRQ(EXTI1_IRQn);

	/**************************************
	* CONFIGURACIÓN DEL NVIC PARA PB2  *
	**************************************/
	// Matriculamos la interrupción EXTI02 en el NVIC
	NVIC_EnableIRQ(EXTI2_IRQn);

	/***********************************
	***CONFIGURACIÓN DÍGITO 1 DISPLAY***
	************************************/
	// Habilitamos la señal de reloj para el puerto GPIOC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// Limpiamos el estado de los bits reservados para el puerto 11 del registro MODER del GPIOC
	GPIOC->MODER &= ~(0b11 << 22);
	// Establecemos PC11 como salida (01)
	GPIOC->MODER |= (0b01 << 22);
	// Establecemos el tipo de salida como push pull (0)
	GPIOC->OTYPER &= ~(0b1 << 11);
	// Limpiamos el estado de la velocidad del pin
	GPIOC->OSPEEDR &= ~(0b11 << 22);
	// Establecemos la velocidad de conmutación en medio (01)
	GPIOC->OSPEEDR |= (0b01 << 22);
	// Limpiamos la configuración del registro PUPDR para establecer el estado no pull up no pull down
	GPIOC->PUPDR &= ~(0b11 << 22);

	/***********************************
	***CONFIGURACIÓN DÍGITO 2 DISPLAY***
	************************************/
	// Limpiamos el estado de los bits reservados para el puerto 10 del registro MODER del GPIOC
	GPIOC->MODER &= ~(0b11 << 20);
	// Establecemos PC10 como salida (01)
	GPIOC->MODER |= (0b01 << 20);
	// Establecemos el tipo de salida como push pull (0)
	GPIOC->OTYPER &= ~(0b1 << 10);
	// Limpiamos el estado de la velocidad del pin
	GPIOC->OSPEEDR &= ~(0b11 << 20);
	// Establecemos la velocidad de conmutación en medio (01)
	GPIOC->OSPEEDR |= (0b01 << 20);
	// Limpiamos la configuración del registro PUPDR para establecer el estado no pull up no pull down
	GPIOC->PUPDR &= ~(0b11 << 20);

	/***********************************
	***CONFIGURACIÓN DÍGITO 3 DISPLAY***
	************************************/
	// Limpiamos el estado de los bits reservados para el puerto 12 del registro MODER del GPIOC
	GPIOC->MODER &= ~(0b11 << 24);
	// Establecemos PC12 como salida (01)
	GPIOC->MODER |= (0b01 << 24);
	// Establecemos el tipo de salida como push pull (0)
	GPIOC->OTYPER &= ~(0b1 << 12);
	// Limpiamos el estado de la velocidad del pin
	GPIOC->OSPEEDR &= ~(0b11 << 24);
	// Establecemos la velocidad de conmutación en medio (01)
	GPIOC->OSPEEDR |= (0b01 << 24);
	// Limpiamos la configuración del registro PUPDR para establecer el estado no pull up no pull down
	GPIOC->PUPDR &= ~(0b11 << 24);

	/***********************************
	***CONFIGURACIÓN DÍGITO 4 DISPLAY***
	************************************/
	// Limpiamos el estado de los bits reservados para el puerto 7 del registro MODER del GPIOC
	GPIOC->MODER &= ~(0b11 << 14);
	// Establecemos PC7 como salida (01)
	GPIOC->MODER |= (0b01 << 14);
	// Establecemos el tipo de salida como push pull (0)
	GPIOC->OTYPER &= ~(0b1 << 7);
	// Limpiamos el estado de la velocidad del pin
	GPIOC->OSPEEDR &= ~(0b11 << 14);
	// Establecemos la velocidad de conmutación en medio (01)
	GPIOC->OSPEEDR |= (0b01 << 14);
	// Limpiamos la configuración del registro PUPDR para establecer el estado no pull up no pull down
	GPIOC->PUPDR &= ~(0b11 << 14);

	/*******************************************
	 ***CONFIGURACIÓN PINES SEGMENTOS DISPLAY***
	 *******************************************/
	// Configurar PC2(F), PC3(A) y PC4(E) como salidas
	GPIOC->MODER &= ~((0b11 << 4) | (0b11 << 6) | (0b11 << 8));
	GPIOC->MODER |=  ((0b01 << 4) | (0b01 << 6) | (0b01 << 8));
	GPIOC->OTYPER &= ~((0b1 << 2) | (0b1 << 3) | (0b1 << 4));
	GPIOC->OSPEEDR |= ((0b01 << 4) | (0b01 << 6) | (0b01 << 8));
	GPIOC->PUPDR &= ~((0b11 << 4) | (0b11 << 6) | (0b11 << 8));

	// Configurar PA8(G) y PA10(D) como salidas
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~((0b11 << 16) | (0b11 << 20));
	GPIOA->MODER |=  ((0b01 << 16) | (0b01 << 20));
	GPIOA->OTYPER &= ~((0b1 << 8) | (0b1 << 10));
	GPIOA->OSPEEDR |= ((0b01 << 16) | (0b01 << 20));
	GPIOA->PUPDR &= ~((0b11 << 16) | (0b11 << 20));

	// Configurar PB7(B) y PB10(C) como salidas
	GPIOB->MODER &= ~((0b11 << 14) | (0b11 << 20));
	GPIOB->MODER |=  ((0b01 << 14) | (0b01 << 20));
	GPIOB->OTYPER &= ~((0b1 << 7) | (0b1 << 10));
	GPIOB->OSPEEDR |= ((0b01 << 14) | (0b01 << 20));
	GPIOB->PUPDR &= ~((0b11 << 14) | (0b11 << 20));

	while(1){
		// Sincronizamos la variable contador con la variable EVENTOS modificada por los botones
		contador = EVENTOS;
	}
}


/******************
 ***HANDLER TIM2***
 ******************/
void TIM2_IRQHandler(){
	if (TIM2->SR & TIM_SR_UIF){

		// Conmutación limpia y exclusiva del pin PC0
		GPIOC->ODR ^= (0b1 << 0);

		// limpiamos la bandera del event-update
		TIM2->SR &= ~TIM_SR_UIF;
	}
}

/******************
 ***HANDLER TIM3***
 ******************/
void TIM3_IRQHandler(){
	if (TIM3->SR & TIM_SR_UIF){

		// Apagamos los 4 transistores NPN mandando un 0 lógico (PC7, PC10, PC11, PC12) antes de conmutar
		GPIOC->ODR &= ~((0b1 << 7) | (0b1 << 10) | (0b1 << 11) | (0b1 << 12));

		// Descomposición matemática dinámica de la variable contador
		uint8_t miles    = (contador / 1000) % 10;
		uint8_t centenas = (contador / 100) % 10;
		uint8_t decenas  = (contador / 10) % 10;
		uint8_t unidades = contador % 10;

		// Multiplexamos aplicando la matemática a cada transistor correspondiente
		if(posicion_digito == 0){
			mostrar_numero(miles);      // Muestra la cifra de los miles en el Dígito 1
			GPIOC->ODR |= (0b1 << 11);  // Enciende Dígito 1 (PC11)
			posicion_digito = 1;
		}
		else if(posicion_digito == 1){
			mostrar_numero(centenas);   // Muestra la cifra de las centenas en el Dígito 2
			GPIOC->ODR |= (0b1 << 10);  // Enciende Dígito 2 (PC10)
			posicion_digito = 2;
		}
		else if(posicion_digito == 2){
			mostrar_numero(decenas);    // Muestra la cifra de las decenas en el Dígito 3
			GPIOC->ODR |= (0b1 << 12);  // Enciende Dígito 3 (PC12)
			posicion_digito = 3;
		}
		else if(posicion_digito == 3){
			mostrar_numero(unidades);   // Muestra la cifra de las unidades en el Dígito 4
			GPIOC->ODR |= (0b1 << 7);   // Enciende Dígito 4 (PC7)
			posicion_digito = 0;
		}

		// limpiamos la bandera del event-update
		TIM3->SR &= ~TIM_SR_UIF;
	}
}

/******************
 ***HANDLER EXTI1***
 ******************/
void EXTI1_IRQHandler(void){
	// Verificamos que la interrupción provenga del EXTI01 (Botón de restar)
	if (EXTI->PR & EXTI_PR_PR1){

		// COMPORTAMIENTO CÍCLICO: Si está en cero, salta al tope de 4 dígitos
		if (EVENTOS == 0) {
			EVENTOS = 9999;
		} else {
			EVENTOS --;
		}

		// Limpiamos la bandera
		EXTI->PR |= EXTI_PR_PR1;
	}
}

/******************
 ***HANDLER EXTI2***
 ******************/
void EXTI2_IRQHandler(void){
	// Verificamos que la interrupción provenga del EXTI02 (Botón de sumar)
	if (EXTI->PR & EXTI_PR_PR2){

		// COMPORTAMIENTO CÍCLICO: Si está en el tope, da la vuelta a cero
		if (EVENTOS == 9999) {
			EVENTOS = 0;
		} else {
			EVENTOS ++;
		}

		// Limpiamos la bandera
		EXTI->PR |= EXTI_PR_PR2;
	}
}
