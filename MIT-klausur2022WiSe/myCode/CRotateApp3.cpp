#include "CRotateApp3.h"

/**
 * Implementiert (unter Verwendung des Timers 9 und seinem Interrupt)
 * ein Lauflicht mit folgendem Verhalten:
 *
 *  * Am Anfang leuchten die beiden LEDs ganz rechts.
 *
 *  * Dann wird das Muster im Sekundentakt nach links rotiert.
 *
 * Implementierungshinweis: Rufen Sie aus der Interrupt-Routine mit
 * dem in der Vorlesung/Labor eingeführten Mechanismus die Methode
 * CRotateApp3::updatePattern auf.
 */

//CPatternController *CPatternController::m_pc;
CRotateApp3 *CRotateApp3::m_instance;
/**
 * Wird immer nach Ablauf der Wartezeit aufgerufen,
 * um das angezeigte Muster zu aktualisieren.
 */
void CRotateApp3::updatePattern() {
	m_pc->rotate(1);
}

/**
 * Initialisiert den Timer 9 im Interruptbetrieb.
 */
void CRotateApp3::initTimer9() {
	// Timer 9 wird zuerst mit Bus-Takt (vom APB2 Bus) versorgt.
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM9EN);

	// PSC wird auf berechneten Wert fuer ftimer = 1 MHz gesetzt.
	// (PSC = (fclk / ftimer) - 1 =  84 MHz/1MHz - 1 = 84 -1)
	WRITE_REG(TIM9->PSC, 84 - 1);

	// ARR fuer TIM3 wird entsprechend Periodendauer (T von PWM 3) gesetzt.
	WRITE_REG(TIM9->ARR, 1000000 - 1);

	// Enable Bit von CCER wird gesetzt um die Ausgabe von OC1REF
	// einzuschalten.(nicht gebraucht)
//	SET_BIT(TIM9->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 3 wird generiert (Puffer wird aktualisiert).
	SET_BIT(TIM9->EGR, TIM_EGR_UG);

	// UIF (UpdateInterruptFlag) im SR wird geloescht.
	CLEAR_BIT(TIM9->SR, TIM_SR_UIF);

	// Interrupt Handler wird zu NVIC zugewiesen.
	__NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

	// Timer 10 wird fuer Updates eingeschaltet, um Interrupts zu aktivieren.
	// (Update Interrupt Flag wird in DMA/Interrupt Enable Register gesetzt).
	SET_BIT(TIM9->DIER, TIM_DIER_UIE);

	// Enable Bit von Control Register 1 wird gesetzt (Timer gestartet).
	SET_BIT(TIM9->CR1, TIM_CR1_CEN);

	// IE von CC1 wird im DIER (DMA/Interrupt Enable Register) gesetzt.
	// Ermoeglicht die Erzeugung von Interrupts bei CC1-Ereignisse auf TIM10.
//	SET_BIT(TIM9->DIER, TIM_DIER_CC1IE); (nicht gebraucht, keine Aenderungen an CCR1.

}

/**
 * Initialisiert ein neues Exemplar der App.
 *
 * @param pc Zeiger auf den Pattern Controller
 */
CRotateApp3::CRotateApp3(CPatternController *pc) {
	m_pc = pc;

	m_instance = this;

	initTimer9();
}

/**
 * Implementiert die für die Klasse beschriebene Funktion
 * unter Nutzung der privaten Methoden.
 */
void CRotateApp3::run() {

// Am Anfang erst 2 LEDs ganz rechts leuchten.
	m_pc->set(0, 1);
	m_pc->set(1, 1);

	while (true) {

	}
}

void TIM1_BRK_TIM9_IRQHandler() {

// Falls UIF im SR gesetzt ist ...
	if (READ_BIT(TIM9->SR, TIM_SR_UIF)) {

		// wird UIF wieder zurueckgesetzt.
		CLEAR_BIT(TIM9->SR, TIM_SR_UIF);

		CRotateApp3::m_instance->updatePattern();

	}
}
