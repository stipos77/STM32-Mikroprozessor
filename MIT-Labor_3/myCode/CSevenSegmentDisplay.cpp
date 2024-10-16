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

CSevenSegmentDisplay::CSevenSegmentDisplay(
		CShiftRegisterOutputExtender *ssdSegments, DigitalOut* digits[4]) {
	// Die uebergegebene Werte werden in Attributen des Parameters gespeichert
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Die Initialisierung des Arrays
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// Als Startzustand wird die erste Anzeigestelle aktiviert und alle andere
	// werden ausgeschaltet.
	*m_digits[0] = 1;
	for (int digit = 1; digit < 4; digit++) {
		*m_digits[digit] = 0;
	}

	// Die Initialisierung von m_activeDigit und Zuweisung des Wertes 0
	m_activeDigit = 0;

	// Die Initialisierung von m_decimalDigit und Zuweisung des Wertes 0
	m_decimalDigit=0;

	// alle Ausgaenge des Schieberegisters wird auf 1 gesetzt (7-Segment wird
	// umgeschaltet).
	*m_ssdSegments = 0xFF;
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

	// Die Initialisierung einer Hilfsvariable zur Bestimmung der Position des
	// Dezimalpunkts.
	int intDigit = 3;

	// Wert wird mit 1000 multipliziert
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

	//setting decimal point. If intDigit is 0, decimalPoint is set to -1,
	//if not intDigit is set as position of decimal point

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

