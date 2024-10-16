/*
 * CPwmTest2.cpp
 */

#include "mbed.h"
#include "CPwmGenerator2.h"

CPwmGenerator2::CPwmGenerator2() {
	// Timer 3 wird zuerst mit Bus-Takt (vom APB1 Bus) versorgt.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Verwendete Peripheriegeraete werden konfiguriert.
	configureGpioPin();
	configureTimer();
}

void CPwmGenerator2::configureGpioPin() {
	// GPIO-Pin PC6 wird auf "Alternate Function Mode" konfiguriert.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);

	// GPIO-Pin PC6 wird auf Alternate Function 2 konfiguriert.
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator2::configureTimer() {
	// PSC wird fuer ftimer = 2048Hz gesetzt (damit wird die Systemtaktfrequenz
	// geteilt, um die Zaehlrate des Timers zu verlangsamen).
	WRITE_REG(TIM3->PSC , 41016 - 1);

	// Auto-Reload-Register wird gesetzt (ARR = T [von PWM1 = 16s] * ftimer -1).
	WRITE_REG(TIM3->ARR , 16 * 2048 - 1);

	// Tastgrad wird zuerst mit 0 initalisiert (PWM Signal faengt mit LOW an).
	setDutyCycle(0);

	// CCMR1 von Timer 3 wird auf Timermodus von PWM2 (OC1M) konfiguriert.
	// OC1M wird um 6 Bits an die richtige Stelle in des Registers verschoben.
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);

	// Enable Bit von CCER wird gesetzt um die Ausgabe von OC1REF (PWM Output)
	// einzuschalten (um Peripheriegeraete z.B. LEDs anzusteuern).
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 3 wird generiert, um die Werte für PSC und ARR zu
	// laden. Hardware-aktionen werden damit ermoeglicht.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);

	// Dadurch entstehtes UIF (UpdateInterruptFlag) im SR wird geloescht.
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

	// Enable Bit von Control Register 1 wird gesetzt (Timer gestartet).
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

void CPwmGenerator2::setDutyCycle(uint8_t percent) {
	// Falls das Tastgrad groesser als 100 ist...
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

