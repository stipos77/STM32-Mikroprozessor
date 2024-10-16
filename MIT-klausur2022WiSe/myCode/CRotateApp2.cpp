#include "CRotateApp2.h"

/**
 * Wird immer nach Ablauf der Wartezeit aufgerufen,
 * um das angezeigte Muster zu aktualisieren.
 */
void CRotateApp2::updatePattern() {

	// kein CC1IF, da wir nur Periode des Timers nutzen.
	if (READ_BIT(TIM9->SR, TIM_SR_UIF)) {

		CLEAR_BIT(TIM9->SR, TIM_SR_UIF);

		m_pc->rotate(1);
	}
}

/**
 * Initialisiert den Timer 9.
 */
void CRotateApp2::initTimer9() {

	// PSC wird auf berechneten Wert fuer ftimer = 1 MHz gesetzt.
	// (PSC = (fclk / ftimer) - 1 =  84 MHz/1MHz - 1 = 84 -1)
	WRITE_REG(TIM9->PSC, 84 - 1);

	// ARR fuer TIM3 wird entsprechend Periodendauer (T von PWM 3) gesetzt.
	WRITE_REG(TIM9->ARR, 1000000 - 1);

	// Capture/Compare Register 1 von Timer 3 wird auf 10 * ARR gesetzt.
//	WRITE_REG(TIM9->CCR1, READ_REG(TIM9->ARR));

	// Enable Bit von CCER wird gesetzt um die Ausgabe von OC1REF
	// einzuschalten.
//	SET_BIT(TIM9->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 3 wird generiert (Puffer wird aktualisiert).
	SET_BIT(TIM9->EGR, TIM_EGR_UG);

	// UIF (UpdateInterruptFlag) im SR wird geloescht.
	CLEAR_BIT(TIM9->SR, TIM_SR_UIF);

	// Enable Bit von Control Register 1 wird gesetzt (Timer gestartet).
	SET_BIT(TIM9->CR1, TIM_CR1_CEN);

	// Timer 9 wird fuer Updates eingeschaltet, um Interrupts zu aktivieren.
	// (Update Interrupt Flag wird in DMA/Interrupt Enable Register gesetzt).
//	SET_BIT(TIM9->DIER, TIM_DIER_UIE);

	// IE von CC1 wird im DIER (DMA/Interrupt Enable Register) gesetzt.
	// Ermoeglicht die Erzeugung von Interrupts bei CC1-Ereignisse auf TIM10.
//	SET_BIT(TIM9->DIER, TIM_DIER_CC1IE);
}

/**
 * Initialisiert ein neues Exemplar der App.
 *
 * @param pc Zeiger auf den Pattern Controller
 */
CRotateApp2::CRotateApp2(CPatternController *pc) {
	m_pc = pc;
	// Timer 9 wird zuerst mit Bus-Takt (vom APB2 Bus) versorgt.
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM9EN);
	initTimer9();
}

/**
 * Implementiert die für die Klasse beschriebene Funktion
 * unter Nutzung der privaten Methoden.
 */
void CRotateApp2::run() {

	// Am Anfang erst 2 LEDs ganz rechts leuchten.
	m_pc->set(0, 1);
	m_pc->set(1, 1);

	while(true){
//		while(READ_BIT(TIM9->SR, TIM_SR_UIF)== 0){//Zeitverzoegerung fuer Timer
//			}
		// Ausfuehrung von Interrupt Handlern verhindern und LED0 ausschalten.
				__disable_irq();

				// Auf einen Interrupt warten. Wenn er auftritt wird die CPU wieder
				// aktiviert, der Interrupt Handler wird aber noch nicht ausgefuehrt.
				// Der Interrupt "steht aus" ("is pending").
				__WFI();

				// LED0 einschalten und die Ausfuehrung von Interrupt Handlern wieder
				// zulassen. Der Handler wird damit sofort nach __enable_irq()
				// aufgerufen.
				updatePattern();
				__enable_irq();

//	updatePattern();
	}
}

