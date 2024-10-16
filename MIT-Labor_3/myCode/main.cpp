/*
 * Steven Atmodjo
 * Matrikel Nummer 772550
 */

#include "breadboard.h"
#include "mbed.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CDebouncer.h"
#include "CHexShifter.h"
#include "CMeasuringDevice.h"
#include<iostream>

// Definition eines globalen BusIn-Objekts.
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5, BB_BTN_6,
		BB_BTN_7);

// Definition eines globalen BusOut-Objekts.
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5, BB_LED_6
		,BB_LED_7);

// Definition von DigitalOut-Objekten fuer die Pins des Schieberegisters
// (SER, SCLK, RCLK).
DigitalOut ssdSerialData(SSD_SER);
DigitalOut ssdShiftRegisterClock(SSD_SCLK);
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

// Definition von DigitalOut-Objekten fuer die 4-Anzeigedigits.
DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);

// Ein- bzw. Ausschaltgaenge der Ziffern der 7-Segmentanzeige.
DigitalOut *digits[] = {&ssdDigit3, &ssdDigit2, &ssdDigit1, &ssdDigit0};

// Definition eines Schieberegister-Objekts zur Ansteuerung der
// 7-Segment-Anzeige.
CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
		&ssdBufferRegisterClock);

/**
 * Sendet das Zeichen '*' ueber USART.
 */
void task1() {

	// USART wird mit den Pins PA_2 und PA_3 und einer Baudrate von 115200
	// initialisiert.
	BufferedSerial usart(PA_2, PA_3, 115200);
    while (true) {

    	// char- Array mit dem Inhalt '*' wird initialisiert.
    	char data[] = { '*' };

    	// Array wird ueber USART gesendet.
    	usart.write(data, 1);
    }
}

/**
 * Realisiert die Anwendung von der Klasse CHexShifter, die die Anzeige
 * des SSDs
 */
void task2() {

	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// USART wird mit den Pins PA_2 und PA_3 und Baudrate von 115200
	// initialisiert.
	BufferedSerial usart(PA_2, PA_3, 115200);

	// Den USART wird auf nicht-blockierenden Modus (non-blocking) gesetzt.
	usart.set_blocking(false);

	// Definition eines CHexShifter-Objekts fuer die Nutzung von USART.
	CHexShifter application(&ssd, &keys, &leds, &usart);

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {

		// Falls Timer schon erreicht wuerde (5ms)...
		if (digitTimer.timeReached()) {

			// wird die naechste Ziffer auf der SSD angezeigt.
			ssd.activateNextDigit();
		}

		// die poll-Methode von CHexShifter wird ausgefuehrt.
		application.poll();
	}
}

/**
 * Verwendet die 7-Segment-Anzeige (SSD), einen USART, Tasten , LEDs und
 * Potentiometer.
 *
 * Je nach Anzeigemodus bzw. Messmodus wird die Spannungswert auf den
 * SSD, LEDs und PC-Terminal darstellbar.
 */
void task3(){

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// USART wird mit den Pins PA_2 und PA_3 und einer Baudrate von 115200
	// initialisiert.
	BufferedSerial usart(PA_2, PA_3, 115200);

	// USART wird auf nicht-blockierenden Modus (non-blocking) gesetzt.
	usart.set_blocking(false);

	// Definition eines globalen AnalogIn-Objekts mit Referenzspannung von 3,3V
	// fuer Spannungsmessung.
	AnalogIn poti(BB_POTI, 3.3f);

	// Definition eines CMeasuringDevice-Objekts
	CMeasuringDevice application(&ssd, &keys, &leds, &usart, &poti);

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {

		// Falls Timer schon erreicht wuerde (5ms)...
		if (digitTimer.timeReached()) {

			// wird die naechste Ziffer auf der SSD angezeigt.
			ssd.activateNextDigit();
		}

		// die poll-Methode der CMeasurementDevice wird ausgefuehrt.
		application.poll();
	}
}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgefÃ¼hrt.
 */
int main() {

	// Pulldown-Widerstaende wird fuer die Tasten eingeschaltet.
	keys.mode(PullDown);

	// Aufruf der Funktion, die die Loesung der gerade bearbeiteten Aufgabe
	// implementiert.
//	task1();
//  task2();
	task3();


	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgefuehrt. Fuer den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterlaeuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
    while (true) {
    }
}

