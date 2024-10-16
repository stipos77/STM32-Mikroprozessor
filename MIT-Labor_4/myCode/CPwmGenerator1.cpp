/*
 * CPwmTest1.cpp
 */

#include "CPwmGenerator1.h"
#include "mbed.h"

CPwmGenerator1::CPwmGenerator1() {
	// Timer 3 wird zuerst mit Bus-Takt (vom APB1 Bus) versorgt.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Verwendete Peripheriegeraete werden fortlaufend konfiguriert.
	configureGpioPin();
	configureTimer();
}

void CPwmGenerator1::configureGpioPin() {
	// GPIO-Pin PC6 wird auf "Alternate Function Mode" konfiguriert.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);

	// GPIO-Pin PC6 wird auf Alternate Function 2 konfiguriert.
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6, GPIO_AF2_TIM3
			<< GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator1::configureTimer() {
	// Prescaler-Register von Timer 3 wird mit 2048Hz (fuer ftimer) eingestellt.
	// (PSC = (fclk / ftimer) - 1 =  84 MHz/2048 - 1 = 41016 -1)
	WRITE_REG(TIM3->PSC , 41016 - 1);

	// Auto-Reload-Register wird gesetzt (ARR = T [von PWM1 = 16s] * ftimer -1).
	WRITE_REG(TIM3->ARR , 16 * 2048 - 1);

	// Capture/Compare Register 1 von Timer 3 wird auf 10 * 2048 gesetzt.
	WRITE_REG(TIM3->CCR1 , 10 * 2048 );

	// Capture/Compare Mode Register 1 von Timer 3 wird auf PWM1 konfiguriert.
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);

	// Enable-Bit von CCER (Capture/Compare Enable) Register wird gesetzt, um
	// die Ausgabe von OC1REF einzuschalten.
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// Enable-Bit von Event Generation Register des Timers zum Aktualiesieren
	// der geprufferten Register wird gesetzt.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);

	// Das dadurch im SR gesetztes UpdateInterruptFlag (UIF) wird geloescht.
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

	// Enable-Bit von Control Register 1 des Timers wird gesetzt
	// (Timer gestartet).
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}
