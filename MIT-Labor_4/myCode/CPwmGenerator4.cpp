/*
 * CPwmGenerator4.cpp
 *
 *  Created on: Jun 26, 2024
 *      Author: Steven Atmodjo
 */

#include "CPwmGenerator4.h"

CPwmGenerator4::CPwmGenerator4(uint16_t period) {

	// Verwendete Peripheriegeraete werden fortlaufend konfiguriert.
	configureGpioPin();

	// Falls uebergegebene Wert weniger gleich als 20 ms ist....
	if (period <= 20000) {

		// wird Periode des Timers eingesetzt.
		configureTimer(period);
	}

	// Interrupt Handler wird aufgeruft.
	configureInterrupt();
}

void CPwmGenerator4::configureGpioPin() {
	// GPIO-Pin PC6 wird auf "Alternate Function Mode" konfiguriert.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);

	// GPIO-Pin PC6 wird auf Alternate Function 2 konfiguriert.
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator4::configureTimer(uint16_t period) {
	// Timer 3 wird zuerst mit Bus-Takt (vom APB1 Bus) versorgt.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// PSC wird auf berechneten Wert fuer ftimer = 1 MHz gesetzt.
	WRITE_REG(TIM3->PSC, 84 - 1);

	// ARR wird mit Periode des PWMs (als Mikrosek) gesetzt.
	WRITE_REG(TIM3->ARR, period * 1000 - 1);

	// Tastgrad wird zuerst mit 0 initalisiert (PWM Signal faengt mit LOW an).
	setDutyCycle(0);

	// CCMR1 von Timer 3 wird auf PWM4 konfiguriert.
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);

	// Enable Bit von CCER wird gesetzt, um die Ausgabe einzuschalten.
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 3 wird generiert.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);

	// UIF wird geloescht.
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

	// Timer wird gestartet.
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

void CPwmGenerator4::configureInterrupt() {
	// Interrupt wird im Nested Vectored Interrupt Controller eingeschaltet.
	__NVIC_EnableIRQ(TIM3_IRQn);

	// Timer 3 wird fuer Updates eingeschaltet, um Interrupts zu aktivieren.
	// (Update Interrupt Flag wird in DMA/Interrupt Enable Register gesetzt).
	SET_BIT(TIM3->DIER, TIM_DIER_UIE);

	// IE von CC1 wird im DIER (DMA/Interrupt Enable Register) gesetzt.
	// Ermoeglicht die Erzeugung von Interrupts bei CC1-Ereignisse auf TIM3.
	SET_BIT(TIM3->DIER, TIM_DIER_CC1IE);

	// Interrupt Handler wird an Funktionszeiger (activeTIM3_IRQHandler) mittels
	// "bind" gebundet.
	activeTIM3_IRQHandler = bind(&CPwmGenerator4::handleInterrupt, this);
}

void CPwmGenerator4::handleInterrupt() {
	// Falls UIF im SR gesetzt ist ...
	if (READ_BIT(TIM3->SR, TIM_SR_UIF)) {

		// wird UIF wieder zurueckgesetzt.
		CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

		// Falls Wert vom Compare Register 1 ungleich Null ist...
		if (READ_REG(TIM3->CCR1) != 0) {

			// wird uebergegegebene LED eingeschaltet.
			*m_pwmOut = 1;
		}
	}

	// Falls Interrupt Flag in CC1 gesetzt ist (Compare Interrupt)....
	else if (READ_BIT(TIM3->SR, TIM_SR_CC1IF)) {

		// wird der Update Flag geloescht.
		CLEAR_BIT(TIM3->SR, TIM_SR_CC1IF);

		// wird uebergegebene LED ausgeschaltet.
		*m_pwmOut = 0;
	}
}

void CPwmGenerator4::setDutyCycle(uint8_t percent) {
	// Falls der Tastgrad groesser als 100 ist...
	if (percent > 100) {

		// wird Tastgrad auf 100 gesetzt.
		percent = 100;
	}

	// Der eingegebene Wert wird als float zur Prozent berechnet.
	float dutyCycle = static_cast<float>(percent) / 100;

	// Das Tastgrad wird entsprechend Bitbreite des CCR1s von TIM3 angepasst.
	uint16_t val = static_cast<uint16_t>((READ_REG(TIM3->ARR) + 1) * dutyCycle);

	// Das Tastgrad wird ins CCR1 von Timer 3 zugewiesen.
	WRITE_REG(TIM3->CCR1, val);
}

void CPwmGenerator4::setPwmOut(DigitalOut *pin) {
	// Die uebergegebenen LED Pin fuer (Helligkeitansteuerung) wird gespeichert.
	m_pwmOut = pin;
}
