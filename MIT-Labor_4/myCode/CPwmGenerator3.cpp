/*
 * CPwmGenerator3.cpp
 *
 *  Created on: Jun 26, 2024
 *      Author: Steven Atmodjo
 */

#include "CPwmGenerator3.h"

CPwmGenerator3::CPwmGenerator3(uint16_t period) {
	// Timer 3 wird zuerst mit Bus-Takt (vom APB1 Bus) versorgt.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Falls im Parameter uebergegebene Wert im gueltigen Bereich liegt...
	if (period > 0 && period < 63535) {

		m_period = period;
	}

	// Verwendete Peripheriegeraete werden konfiguriert.
	configureGpioPin();
	configureTimer();
}

void CPwmGenerator3::configureGpioPin() {
	// GPIO-Pin PC6 wird auf "Alternate Function Mode" konfiguriert.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);

	// GPIO-Pin PC6 wird auf Alternate Function 2 konfiguriert.
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator3::configureTimer() {
	// PSC wird auf berechneten Wert fuer ftimer = 1 MHz gesetzt.
	// (PSC = (fclk / ftimer) - 1 =  84 MHz/1MHz - 1 = 84 -1)
	WRITE_REG(TIM3->PSC , 84 - 1);

	// ARR fuer TIM3 wird entsprechend Periodendauer (T von PWM 3) gesetzt.
	WRITE_REG(TIM3->ARR , m_period - 1);

	// Tastgrad wird zuerst mit 0 initalisiert (PWM Signal faengt mit LOW an).
	setDutyCycle(0);

	// CCMR1 von Timer 3 wird auf PWM3 konfiguriert.
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);

	// Enable Bit von CCER wird gesetzt um die Ausgabe von OC1REF
	// einzuschalten.
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 3 wird generiert (Puffer wird aktualisiert).
	SET_BIT(TIM3->EGR, TIM_EGR_UG);

	// UIF (UpdateInterruptFlag) im SR wird geloescht.
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

	// Enable Bit von Control Register 1 wird gesetzt (Timer gestartet).
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

void CPwmGenerator3::setDutyCycle(uint8_t percent) {
	// Falls der Tastgrad groesser als 100 ist...
	if (percent > 100) {

		// wird Tastgrad auf 100 gesetzt.
		percent = 100;
	}

	// Der eingegebene Wert wird als float zur Prozent berechnet.
	float dutyCycle = static_cast<float>(percent) / 100.f;

	// Das Tastgrad wird entsprechend Bitbreite des CCR1s von TIM3 angepasst.
	uint16_t val = static_cast<uint16_t>((READ_REG(TIM3->ARR) + 1) * dutyCycle);

	// Das Tastgrad wird ins CCR1 von Timer 3 zugewiesen.
	WRITE_REG(TIM3->CCR1, val);
}
