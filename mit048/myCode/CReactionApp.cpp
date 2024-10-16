/*
 * Name: Samuel Alpha Gian
 * Matrikelnummer: 769570
 * Rechnernummer: 08
 */

#include "CDebouncer.h"
#include "CPolledTimer.h"
#include "CReactionApp.h"

CReactionApp *CReactionApp::m_instance;

CReactionApp::CReactionApp(BusIn *keys, DigitalOut *startLed,
		BusOut *resultLeds) {
	// Mitgliedsvariable werden initialisiert.
	m_keys = keys;
	m_startLed = startLed;
	m_resultLeds = resultLeds;

	//Zeiger auf Instanzen dieser Klasse.
	m_instance = this;
}

void CReactionApp::waitAndTurnOn() {

	thread_sleep_for((rand()+1) % 7 * 1000);
	m_startLed->write(1);
}

bool CReactionApp::waitForKeyWithTimeout() {
	// Timer mit 3 s WarteZeit
	CPolledTimer timer(3000);

	while (true) {

		// Check if the specific key is pressed
		if (m_keys->read() & 0x80) {
			return true;  // Key pressed
		}

		else if(timer.timeReached()){
			return false;
		}
	}

	return false;
}

uint64_t CReactionApp::toBcd(uint32_t value) {
	uint64_t bcdResult = 0;  // Variable zur Speicherung des BCD-Ergebnisses
	uint64_t shift = 0; // Verschiebungszähler, um die BCD-Ziffern korrekt zu platzieren

	// Gehe durch jede Dezimalziffer und konvertiere sie in BCD
	while (value > 0) {
		uint8_t digit = value % 10;     // Hole die letzte Dezimalziffer
		bcdResult |= (static_cast<uint64_t>(digit) << shift); // Setze die BCD-Ziffer an die richtige Position
		value /= 10;    // Entferne die letzte Dezimalziffer
		shift += 4; // Verschiebe um 4 Bit (jede Ziffer wird durch 4 Bit repräsentiert 0...F)
	}

	return bcdResult;
}

void CReactionApp::displayTime(uint64_t timeInMs) {

	uint32_t decimalValue = 0;
	uint32_t factor = 1;

	// Extract each BCD digit from the 64-bit BCD number
	for (int i = 0; i < 16; ++i) {

		uint8_t digit = (timeInMs >> (i * 4)) & 0xF;  // Extract 4 bits

		if (digit <= 9) {
			decimalValue += digit * factor;
		}

		factor *= 10;
	}

	uint8_t bcd8 = ((decimalValue / 10) << 4) | (decimalValue % 10);

	if (timeInMs <= 990) {
		// Hundertstel Sekunden in BCD umwandeln und auf LEDs anzeigen.
		m_resultLeds->write(bcd8);
	}

	else {
		m_resultLeds->write(0xF);
	}
}

void CReactionApp::initTimer() {

	// Timer 3 wird zuerst mit Bus-Takt (vom APB2 Bus) versorgt.
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);

	// PSC fuer ftimer = 1 MHz (84 MHz/1MHz - 1 = 84 -1)
	TIM3->PSC = 84 - 1;

	// ARR (Periode von Tim3) zu 1 ms einstellen.
	TIM3->ARR = 1000 - 1;

	// Update Event fuer TIM 3 wird generiert.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);

	// UIF wird geloescht.
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

	// Interrupt wird im Nested Vectored Interrupt Controller eingeschaltet.
	__NVIC_EnableIRQ(TIM3_IRQn); // (Update Interrupt Enable Bit wird in DMA/Interrupt Enable Register gesetzt).

	// Interrupts gestartet..
	SET_BIT(TIM3->DIER, TIM_DIER_UIE);

}

void CReactionApp::startTimer() {

	// Timer kofigurieren, bzw. Interrupt-Handler richten.
	initTimer();

	// Timer (Timerzaehler) wird gestartet.
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

void CReactionApp::stopTimer() {

	// Timer 3 (Timerzaehler) ausschalten.
	CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

	// Timer/Update Interrupt ausschalten (jetzt kein Interrupt bei CC1 Erreignisse).
	CLEAR_BIT(TIM3->DIER, TIM_DIER_UIE);

	// Timer 3 in NVIC ausschalten.
	__NVIC_DisableIRQ (TIM3_IRQn);

	// Update Interrupt Flags in SR loeschen.
	TIM3->SR &= ~TIM_SR_UIF;

}

void TIM3_IRQHandler() {

	// Falls UIF im SR gesetzt ist ...
	if (READ_BIT(TIM3->SR, TIM_SR_UIF)) {

		// wird UIF wieder zurueckgesetzt.
		CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

		if (CReactionApp::m_instance != nullptr) {

			//Hilfsvariable...
			int64_t sinceStart = CReactionApp::m_instance->millisSinceStart();

			// wird uebergegegebene LED eingeschaltet.
			CReactionApp::m_instance->m_elapsedTime = sinceStart;

			// Anzeige wird aktualisiert.
			CReactionApp::m_instance->displayTime(sinceStart);
		}
	}

}

void CReactionApp::run1() {

	// Warte zw. 1...6 Sek, danach wird Start-Led leuchten.
//	waitAndTurnOn();
//
//	// TIM3 wird entsprechend Konfiguration gestartet.
//	startTimer();
//
////	bool timeOut = waitForKeyWithTimeout();
//
//	if (waitForKeyWithTimeout()) {
//
//		// wird Zeit dauerhaft an LEDs dargestellt.
//		displayTime(toBcd(millisSinceStart() * 10));
//
//		// timer mit 5 Sekunden Wartezeit
//		CPolledTimer timer(5000);
//
//		// Solange noch nicht 5 Sekunden...
//		if (timer.timeReached()) {
//
//			// Damit LEDs dauerhaft leuchten.
//			stopTimer();
//
//			// Timer und Interrupts wieder einschalten.
//			startTimer();
//
//			m_startLed->write(0);
//
//			displayTime(0);
//		}
//	}
//
//	else {
//		displayTime(0);
//
//	}

//	 while (true) {
//	        waitAndTurnOn();
//	        startTimer();
//
//	        if (waitForKeyWithTimeout()) {
//	            uint64_t reactionTime = m_elapsedTime;
//	            stopTimer();
//	            displayTime(toBcd(reactionTime));
//	        } else {
//	            stopTimer();
//	            startTimer();
//	        }
//
//	        thread_sleep_for(5000);  // Display result for 5 seconds
//	        m_startLed->write(0);
//	        m_resultLeds->write(0);
//	    }



	// Step (3): Display the result on the LEDs.
	CPolledTimer checkTimer(1000);

	while (true) {

		// Step (1): Wait for a random time between 1 and 6 seconds and turn on the start LED.
		waitAndTurnOn();

		// Step (2): Start the timer and wait for the leftmost key press or timeout.
		startTimer();

		bool onTime = waitForKeyWithTimeout();

		if (!waitForKeyWithTimeout() || checkTimer.timeReached()) {
			// Timeout or reaction time exceeds 0.99 seconds, turn on all LEDs.
			displayTime(0);
		}

		else if (waitForKeyWithTimeout() && !checkTimer.timeReached()) {
			// Display the reaction time in hundredths of a second in BCD.
//			displayTime(toBcd(m_elapsedTime * 10));
			displayTime(97);
		}

		// Step (4): Display the result for 5 seconds.
		thread_sleep_for(5000);

		// Turn off the start LED and the result LEDs.
		m_startLed->write(0);
		m_resultLeds->write(0);

		stopTimer();
	}
}

void CReactionApp::run2() {

	// Warte zw. 1...6 Sek, danach wird Start-Led leuchten.
	waitAndTurnOn();

	// timer mit 3 Sekunden Wartezeit
	CPolledTimer timer(3000);

	if (!timer.timeReached()) {

		displayTime(toBcd(millisSinceStart() * 10));
	}

	if (waitForKeyWithTimeout()) {

		// Solange noch nicht 5 Sekunden...
		if (!timer.timeReached()) {
			// Damit LEDs dauerhaft leuchten.
			stopTimer();

			startTimer();

			// wird Zeit dauerhaft an LEDs dargestellt.
			displayTime(toBcd(millisSinceStart() * 10));
		}
		m_startLed->write(0);
		displayTime(100);

	}

	else {
		displayTime(100);

	}
}
