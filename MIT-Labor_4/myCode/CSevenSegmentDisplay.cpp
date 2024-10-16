/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"

//Segment-Mustern von hexadezimale Zahlen (0...F) fuer die 7-Segment-Anzeige
uint8_t CSevenSegmentDisplay::patterns[] = {0xD7, 0x11, 0x8F, 0x9B, 0x59,
		0xDA, 0xDE, 0X91, 0XDF,0XDB, 0XDD, 0X5E, 0XC6, 0X1F, 0XCE, 0XCC} ;

// Deklaration einse statischen Zeiger-mitgriedsvariable.
CSevenSegmentDisplay* CSevenSegmentDisplay::m_instance;

CSevenSegmentDisplay::CSevenSegmentDisplay(CShiftRegisterOutputExtender *
										ssdSegments, DigitalOut* digits[4]) {
	// Die uebergegebene Werte werden in Attributen des Parameters gespeichert.
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Die Initialisierung des Arrays.
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// Als Startzustand wird die erste Anzeigestelle aktiviert und alle andere
	// werden ausgeschaltet.
	*m_digits[0] = 1;
	for (int digit = 1; digit < 4; digit++) {
		*m_digits[digit] = 0;
	}

	// Die Initialisierung von m_activeDigit und Zuweisung des Wertes 0.
	m_activeDigit = 0;

	// Die Initialisierung von m_decimalDigit und Zuweisung des Wertes 0.
	m_decimalDigit=0;

	// alle Ausgaenge des Schieberegisters wird auf 1 gesetzt (7-Segment wird
	// umgeschaltet).
	*m_ssdSegments = 0xFF;

	// Zeiger auf Instanz dieser Klasse.
	m_instance = this;

	// Timer 10 wird initialisiert.
	initTimer();
}

void CSevenSegmentDisplay::activateNextDigit() {
	 // Das Segmentmuster wird am Anzeigepunkt "(n + 1) % 4" ins Schieberegister
	 // eingegeben.
	m_ssdSegments->prepareOutput((patterns[m_values[(m_activeDigit + 1) % 4]])

	// Falls Dezimalpunkt am Anzeigepunkt "(n + 1) % 4" ist, wird 0x20 (5. Bit)
	// eingeschaltet.
	|((m_activeDigit + 1) % 4 == m_decimalDigit ? 0x20 : 0x00));

	// Die Anzeigestelle "n" wird ausgeschaltet.
	*m_digits[m_activeDigit] = 0;

	// "n" wird auf den Wert "(n + 1) % 4“.
	m_activeDigit = (m_activeDigit + 1) % 4;

	// Das Segment Muster wird im Pufferregister aktualisiert.
	m_ssdSegments->updateOutput();

	// Die Anzeigestelle "n" wird eingeschaltet".
	*m_digits[m_activeDigit] = 1;
}

uint8_t& CSevenSegmentDisplay::operator[] (int position) {
	// Die Initialisierung von einem Dummy-Referenz fuer Lauf der Methode
	static uint8_t dummy = 0;

	// Uberprueft, ob die angegebene Position gueltig ist (0-3).
	if (position < 0 || position > 3) {
		return dummy;
	}

	// Ein Referenz auf die Ziffer an der angegebenen Stelle wird zurueckgegeben
	return m_values[position];
}

void CSevenSegmentDisplay::setDecimalPoint(int digit){
	// Die Position der Ziffer wird angezeigt.
	m_decimalDigit=3-digit;
}

void CSevenSegmentDisplay::setValue(int value){
	// Falls Eingabe ausserhalb dem gueltigen Bereich (0...999) liegt...
	if((value < 0) | (value > 9999)){

		//  werden an alle Anzeigestellen "0xE" angezeigt.
		for(int i = 0; i < 4; i++){
			m_values[i] = 0xE;
		}
	}

	// Sonst...
	else {

		// an jeder Stelle wird die anzuzeigenden Werten ermittelt
		for(int i = 0; i < 4; i++){

			// an jeder Stelle wird dem Restwert nach Division von 10 zugewiesen
			m_values[i] = value % 10;

			// Wert wird durch 10 dividiert.
			value /= 10;
		}
	}
}

void CSevenSegmentDisplay::setValue(float value){
	// Der Eingabewert wird an einer int-Variable mittels typecast zugewiesen.
	int newValue = (int) value;

	// Hilfsvariable zur Bestimmung der Position des Dezimalpunkts.
	int intDigit = 3;

	// Wert wird mit 1000 multipliziert.
	value *= 1000;

	// Falls Eingabe ausserhalb dem gueltigen Bereich liegt (0.0 ... 9999.0)....
	if((value < 0.0) || (value > 9999.0)){

		//  werden an alle Anzeigestellen "0xE" angezeigt.
		for(int i = 0; i < 4; i++){
			m_values[i] = 0xE;
		}
	}

	// Solange der Eingabewert (nach Typecasting) noch durch 10 teilbar ist.
	while (newValue /= 10){

		// wird intDigit substrahiert (um Dezimalpunkt einmal nach links bewegen)
		intDigit--;

		// Eingabewert wird durch 10 dividiert.
		value/=10;
	}

	// Der Dezimalpunkt wird auf SSD mittels setDecimalPoint()-Methode gesetzt.
	setDecimalPoint(intDigit );//== 0 ? -1 : intDigit);

	// Die setValue()-Methode wird mittels typecasting von int aufgerufen, zur
	// Zuweisung von neuen Ganzzahlen und Aktualisierung von der Anzeige.
	setValue((int)value);
}

CSevenSegmentDisplay& CSevenSegmentDisplay:: operator=(int value){
	// Die setValue()-Methode wird mittels typecasting von int aufgerufen, zur
	// Zuweisung von neuen Ganzzahlen und Aktualisierung von der Anzeige.
	setValue((int)value);

	// Ein Referenz auf das aktualisierte Objekt wird zurueckgegeben.
	return *this;
}

CSevenSegmentDisplay& CSevenSegmentDisplay:: operator=(float value){
	// Die setValue()-Methode wird mittels typecasting von float aufgerufen, zur
	// Zuweisung von Gleitkommazahl und Aktualisierung von der Anzeige.
	setValue((float)value);

	// Ein Referenz auf das aktualisierte Objekt wird zurueckgegeben.
	return *this;
}
void CSevenSegmentDisplay::initTimer() {
	// Timer 10 wird zuerst mit Bus-Takt (vom APB2 Bus) versorgt.
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN);

	// PSC wird auf berechneten Wert fuer ftimer = 1 MHz gesetzt.
	WRITE_REG(TIM10->PSC , 84 - 1);

	// ARR wird mit Periode des TIM10s (alle 5 ms) gesetzt.
	WRITE_REG(TIM10->ARR , 5000 - 1);

	// Enable Bit von CCER wird gesetzt, um die Ausgabe einzuschalten.
	SET_BIT(TIM10->CCER, TIM_CCER_CC1E);

	// Update Event fuer TIM 10 wird generiert.
	SET_BIT(TIM10->EGR, TIM_EGR_UG);

	// UIF wird geloescht.
	CLEAR_BIT(TIM10->SR, TIM_SR_UIF);

	// Timer wird gestartet.
	SET_BIT(TIM10->CR1, TIM_CR1_CEN);

	// Interrupt wird im Nested Vectored Interrupt Controller eingeschaltet.
	__NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

	// Timer 10 wird fuer Updates eingeschaltet, um Interrupts zu aktivieren.
	// (Update Interrupt Flag wird in DMA/Interrupt Enable Register gesetzt).
	SET_BIT(TIM10->DIER, TIM_DIER_UIE);

	// IE von CC1 wird im DIER (DMA/Interrupt Enable Register) gesetzt.
	// Ermoeglicht die Erzeugung von Interrupts bei CC1-Ereignisse auf TIM10.
	SET_BIT(TIM10->DIER, TIM_DIER_CC1IE);
}

void TIM1_UP_TIM10_IRQHandler(){
	// Falls UIF im SR gesetzt ist (jede 5ms)...
	if (READ_BIT(TIM10->SR, TIM_SR_UIF)) {

		// wird UIF wieder zurueckgesetzt.
		CLEAR_BIT(TIM10->SR, TIM_SR_UIF);

		// Naechste Zeichen wird aktiviert.
		CSevenSegmentDisplay::m_instance->activateNextDigit();

		// Falls Wert vom CR1 ungleich Null ist...
		if (READ_REG(TIM10->CCR1) != 0) {

			//Hilfsvariable fuer Helligkeit.
			int brightness = CSevenSegmentDisplay::m_instance->getBrightness();

			// Helligkeit des SSDs wird aktualisiert.
			CSevenSegmentDisplay::m_instance->setBrightness(brightness);
		}
	}

	// Falls Interrupt Flag in CC1 gesetzt ist (Compare Interrupt)....
	if (READ_BIT(TIM10->SR, TIM_SR_CC1IF)) {

		// wird der Update Flag geloescht.
		CLEAR_BIT(TIM10->SR, TIM_SR_CC1IF);

		// Anzeigestelle auf SSD wird ausgeschaltet.
		CSevenSegmentDisplay::m_instance->turnDisplayOff();
	}
}

void CSevenSegmentDisplay::setBrightness(int percent){
	// Falls Prozentsantz mehr als 100 waere...
	if(percent>100){

		// wird Prozentsatz als 100 gesetzt.
		percent = 100;
	}

	// Falls Prozentsantz kleiner als 5 waere...
	else if(percent < 5){

		// wird Prozentsatz als 5 gesetzt.
		percent =5;
	}

	// Das Tastgrad wird entsprechend Bitbreite des CCR1s von TIM10 angepasst.
	uint16_t val = ((READ_REG(TIM10->ARR) + 1) * percent/100);

	// Das Tastgrad wird ins CCR1 von Timer 3 zugewiesen.
	WRITE_REG(TIM10->CCR1, val-1);
}

int CSevenSegmentDisplay::getBrightness(){
	// Wert des CCR1s wird berechnet und als int zurueckgegeben.
	return (int)((READ_REG(TIM10->CCR1) + 1)* 100/(READ_REG(TIM10->ARR) + 1));
}

void CSevenSegmentDisplay::turnDisplayOff(){
	// Wert an aktueller Position wird auf 0 gesetzt, um Anzeige auszuschalten.
	*m_digits[m_activeDigit]=0;
}
