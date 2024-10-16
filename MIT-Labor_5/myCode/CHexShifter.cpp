/*
 * CHexShifter.cpp
 *
 *  Created on: May 31, 2024
 *      Author: Steven Atmodjo
 */

#include "CHexShifter.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"

enum TIME_UNIT {
	NONE, MINUTE_UNIT, MINUTE_TENS, HOUR_UNIT, HOUR_TENS
};

static TIME_UNIT unit = NONE;

CHexShifter::CHexShifter(CSevenSegmentDisplay *display, BusIn *keys,
		BusOut *leds, BufferedSerial *usart) {

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

CHexShifter::~CHexShifter() {

	// Speicher von dynamischer Attribute werden freigegeben.
	delete[] m_keys;
}

void CHexShifter::poll() {

	// Ueberwacht die Tastatureingaben ueber die USART-Schnittstelle.
	pollUsart();
}

void CHexShifter::pollUsart() {

	// Falls Daten im USART noch vorhanden sind...
	if (m_usart->readable()) {

		// Initialisierung eines Arrays zur Speicherung der Tastatureingabe.
		char keyboard[1];

		// wird das Zeichen vom USART eingelesen.
		m_usart->read(keyboard, sizeof(keyboard));

		// werden die Daten mittels processChar()-Methode verarbeitet.
		processChar(keyboard[0]);
	}
}

void CHexShifter::processChar(char c) {

	// Initialisierung eines Hilfsvariable zur Zeichenverarbeitung.
	int8_t value;

	// Falls das eingegebene Zeichen eine Ziffer ist (0..9 ~ 0x30..0x39) ...
	if (('0' <= c) && (c <= '9')) {

		// wird mit '0' (0x30) subtrahiert, damit Wert zwischen 0...9 liegt.
		value = c - '0';
	}

	// Falls eingegebene Zeichen zwischen 0...9 liegt....
	if ((0 < value) && (value <= 9)) {

		// Der berechnete Wert wird mittels processDigit() weiter verarbeitet.
		processDigit(value);
	}

	// Falls eingegebene Zeichen negativ ist...
	else if (value < 0) {

		// wird SSD an Indexstelle ausgeschaltet.
		m_display->setValue(-1);
	}

	// Freiwillig: Falls eingegebene Zeichen '<' (0x3E) ist...
	else if (c == '<') {

		if (unit == NONE) {

			unit = NONE;
		}

		else {

			unit = static_cast<TIME_UNIT>((unit - 1));
		}
	}

	// Freiwillig: Falls eingegebene Zeichen '>' (0x3C) ist...
	else if (c == '>') {

		if (unit == HOUR_TENS) {
			unit = HOUR_TENS;
		}

		else {

			unit = static_cast<TIME_UNIT>((unit - 1));
		}
	}
}

void CHexShifter::processDigit(int8_t value) {

	if (unit != NONE) {
		m_display[static_cast<int>(unit) + 1] = value;
	}

	// Die aktualisierten Ziffer wird zum USART gesendet.
	sendDisplayedDigits();
}

void CHexShifter::sendDisplayedDigits() const {

	// Deklaration eines Hilfsarrays zur Speicherung von Zeichen.
	char digits[6];

	// Alle Zahlen im SSD werden zum ASCII-Zeichen umgewandelt und ins
	// Hilfsarray gespeichert.
	for (int i = 0; i < 4; i++) {

		// Falls der Wert im Index eine Zahl, die zwischen 0...9 liegt.
		if ((*m_display)[i] < 10) {

			// wird '0'(0x30) addiert, damit Wert zwischen 0x30..0x39 liegt.
			digits[i] = (*m_display)[i] + '0';
		}

		// Falls der Wert im Index eine Zahl, die zwischen 10...15 liegt.
		if ((10 <= (*m_display)[i]) && (*m_display)[i] <= 15) {

			// wird mit 'a' (0x57) addiert und 10 subtrahiert, damit Wert
			// zwischen 0x57...0x61 liegt.
			digits[i] = (*m_display)[i] + 'a' - 10;
		}
	}

	// Carriage Return (0x0D) wird  wird in Stelle 4 zugewiesen.
	digits[4] = '\r';

	// USART-artige Zeilenumbruch/Line Feed (0x0A) in letzter Stelle zugewiesen.
	digits[5] = '\n';

	// Das Hilfsarray wird ueber USART gesendet und auf dem Terminal angezeigt.
	m_usart->write(digits, sizeof(digits));
}
