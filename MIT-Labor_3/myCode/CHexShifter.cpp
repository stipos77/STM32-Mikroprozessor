/*
 * CHexShifter.cpp
 *
 *  Created on: May 31, 2024
 *      Author: Steven Atmodjo
 */

#include "CHexShifter.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"

CHexShifter::CHexShifter(CSevenSegmentDisplay* display, BusIn* keys,
		BusOut* leds, BufferedSerial* usart){

	// 7-Segment-Anzeige wird mit im Parameter eingegebenen Zeiger auf
	// CSevenSegmentDisplay-Objekt initialisiert.
	m_display = display;

	// CDebouncer-Objekt wird mit 100 ms Verzoegerung initialisiert.
	m_keys = new CDebouncer(keys, 100);

	// LEDs wird mit uebergegebenen Zeiger auf Bus-Out Objekt initialisiert.
	m_leds = leds;

	// USART wird mit im Parameter eingegebenen Zeiger auf BufferedSerial
	// Objekt initialisiert.
	m_usart = usart;

	// Einfuegerichtung wird mit false (rechts, links = true) initialisiert.
	m_insertLeft = false;
}

CHexShifter::~CHexShifter(){

	// Speicher von dynamischer Attribute werden freigegeben.
	delete[] m_keys;
}

void CHexShifter::poll(){

	// Ueberwacht die Tastatureingaben ueber die USART-Schnittstelle.
	pollUsart();

	// Ueberwacht die Zustaende der Tasten.
	pollKeys();
}

void CHexShifter::pollUsart(){

	// Falls Daten im USART noch vorhanden sind...
	if(m_usart->readable()){

		// Initialisierung eines Arrays zur Speicherung der Tastatureingabe.
		char keyboard [1];

		// wird das Zeichen vom USART eingelesen.
		m_usart->read(keyboard, sizeof(keyboard));

		// werden die Daten mittels processChar()-Methode verarbeitet.
		processChar(keyboard[0]);
	}
}

void CHexShifter::processChar(char c){

	// Initialisierung eines Hilfsvariable zur Zeichenverarbeitung.
	uint8_t value;

	// Zusicherung, damit LED 1 nur bei unbekannter Eingabe leuchtet.
	*m_leds = *m_leds & 0xFD;

	// Falls das eingegebene Zeichen eine Ziffer ist (0..9 ~ 0x30..0x39) ...
	if(('0' <= c) && (c <= '9')){

		// wird mit '0' (0x30) subtrahiert, damit Wert zwischen 0...9 liegt.
		value = c - '0';
	}

	// Falls eingegebene Zeichen zwischen 'A' und 'F' (0x37..0x41) liegt...
	else if(('A' <= c) && (c <= 'F')){

		// wird mit 'A' (0x37) subtrahiert und 10 addiert, damit Wert zwischen
		// A...F liegt.
		value = c - 'A' + 10;
	}

	// Falls eingegebene Zeichen zwischen 'a' und 'f' (0x57..0x61) liegt...
	else if(('a' <= c) && (c <= 'f')){

		// wird mit 'a' (0x57) subtrahiert und 10 addiert, damit Wert zwischen
		// A...F liegt.
		value = c - 'a' + 10;
	}

	// Freiwillig: Falls eingegebene Zeichen '<' (0x3E) ist...
	else if (c=='<'){

		// Einfuegerichtung wird zu links gesetzt.
		setInsertLeft(1);
	}

	// Freiwillig: Falls eingegebene Zeichen '>' (0x3C) ist...
	else if (c=='>'){

		// Schieberichtung wird auf rechts mittels setter-Methode gesetzt.
		setInsertLeft(0);
	}

	// Freiwillig: sonst... (eingegebene Zeichen unerkannt)
	else{

		// das zweitniedrigste Bit des LEDs (LED1) wird gesetzt.
		*m_leds = *m_leds | 0x02;
	}

	// Falls eingegebene Zeichen zwischen 0...15 (0...F) liegt....
	if((0 < value) && (value <= 15)){

		// LED1 wird bei erkantte Ziffer ausgeschaltet.
		*m_leds = *m_leds & 0xFD;

		// Der berechnete Wert wird mittels processDigit() weiter verarbeitet.
		processDigit(value);
	}
}

void CHexShifter::processDigit(uint8_t value){

	// Falls Einfuegerichtung auf SSD links ist...
	if(m_insertLeft){

		// Das Array in Position des Indexes wird der Inhalt des Arrays nach dem
		// Index zugewiesen.
		for(int i = 0; i<=3; i++){
			(*m_display)[i] = (*m_display)[i+1];
		}

		// Das Array ganz rechts wird mit Eingabewert aus Parameter zugewiesen.
		(*m_display)[3] = value;
	}

	// sonst...(Einfuegerichtung ist rechts)
	else{

		// Das Array in Position des Indexes wird der Inhalt des Arrays vor dem
		// Index zugewiesen.
		for(int i = 3; i >0; i--){
			(*m_display)[i] = (*m_display)[i-1];
		}

		// Das Array ganz links wird mit Eingabewert aus Parameter zugewiesen.
		(*m_display)[0] = value;
	}

	// Die aktualisierten Ziffer wird zum USART gesendet.
	sendDisplayedDigits();
}

void CHexShifter::sendDisplayedDigits() const{

	// Deklaration eines Hilfsarrays zur Speicherung von Zeichen.
	char digits[6];

	// Alle Zahlen im SSD werden zum ASCII-Zeichen umgewandelt und ins
	// Hilfsarray gespeichert.
	for(int i = 0; i<4; i++){

		// Falls der Wert im Index eine Zahl, die zwischen 0...9 liegt.
		if((*m_display)[i] < 10){

			// wird '0'(0x30) addiert, damit Wert zwischen 0x30..0x39 liegt.
			digits[i] = (*m_display)[i] + '0';
		}

		// Falls der Wert im Index eine Zahl, die zwischen 10...15 liegt.
		if((10 <= (*m_display)[i])&& (*m_display)[i] <= 15){

			// wird mit 'a' (0x57) addiert und 10 subtrahiert, damit Wert
			// zwischen 0x57...0x61 liegt.
			digits[i] = (*m_display)[i] + 'a'-10;
		}
	}

	// Carriage Return (0x0D) wird  wird in Stelle 4 zugewiesen.
	digits[4] = '\r';

	// USART-artige Zeilenumbruch/Line Feed (0x0A) in letzter Stelle zugewiesen.
	digits[5] = '\n';

	// Das Hilfsarray wird ueber USART gesendet und auf dem Terminal angezeigt.
	m_usart->write(digits, sizeof(digits));
}

void CHexShifter::pollKeys(){

	// Hilfsvariable, um vorherige Zustand der Tasten zu speichern.
	static uint8_t previousKeyStates = m_keys->read();

	// aktuellen Zustand der Tasten ausser 1 und 2 wird gespeichert.
	uint8_t newKeyStates = (m_keys->read()) & 0xF9;

	// Bitmaske zum Toggeln den LEDs.
	uint8_t keysToggled = newKeyStates & ~previousKeyStates;

	// Der Wert der 4 linken LEDs wird zur Hilfsvariable zugewiesen.
	uint8_t nibbleBits = *m_leds>>4;

	// Hilfsvariable wird mit Tastereingabe aktualisiert.
	previousKeyStates = newKeyStates;

	// Falls LED0 (ganz rechts) an ist...
	if(*m_leds&0x01){

		// ist Einfuegerichtung aus SSD links
		m_insertLeft=true;
	}

	//sonst... (LED0 ausgeschaltet).
	else if(!(*m_leds&0x01)){

		//ist Einfuegerichtung auf SSD rechts...
		m_insertLeft=false;
	}

	// Falls der 4.Taster von rechts (Bit 3) gedrueckt wuerde...
	if (keysToggled & 0x08) {

		// Die 4 linken LEDs werden um 4 geschoben und zum SSD hineingefuegt.
		processDigit(nibbleBits);

		// LED3 wird geleuchtet, solange Taste 3 gedrueckt ist.
		*m_leds = *m_leds ^ 0x08;

		// Die 4 linke LEDs werden ausgeschaltet.
		*m_leds = *m_leds & 0x0F;
	}

	// Alle LEDs ausser 1 und 2 wird leuchten gelassen.
	*m_leds = (*m_leds ^ newKeyStates) & 0xF3;
}

void CHexShifter::setInsertLeft(bool leftOrRight){

	// Falls Einfuegerichtung rechts ist...
	if(leftOrRight){

		// wird LSB (LED0) eingeschaltet.
		*m_leds= *m_leds | 0x01;

		//Einfuegerichtung wird zu links gesetzt.
		m_insertLeft = true;
	}

	// sonst...
	else{

		// wird LSB (Bit 0) ausgeschaltet.
		*m_leds = *m_leds & 0xFE;

		// Einfuegerichtung wird zu rechts gesetzt.
		m_insertLeft = false;
	}
}
