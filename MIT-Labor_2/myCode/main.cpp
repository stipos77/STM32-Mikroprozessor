/*
 * Abgegeben von: Steven Atmodjo
 * Mat. Nr. 772550
 *
 */
#include "breadboard.h"
#include "mbed.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CMovingAverageFilter.h"
#include "CExponentialFilter.h"

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
 * Zeigt Tasteneingaben auf der 7-Segment-Anzeige (SSD) an.
 *
 * Wenn einer der Taster gedrueckt wird, wird der entsprechende Segment
 * aus, solange der Taster gedrueckt ist.
 */
void task1() {
	// Alle Anzeigestelle werden eingeschaltet
	ssdDigit0 = 1;
	ssdDigit1 = 1;
	ssdDigit2 = 1;
	ssdDigit3 = 1;

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Die bitwise-negierte Eingabewert wird an ssdSegments zugewiesen.
		ssdSegments = ~keys.read();
	}
}

/**
 * Zeigt Tasteneingaben als hexadezimale Ziffern/Buchstaben auf einer
 * entsprechenden Anzeigestelle der 7-Segment-Anzeige (SSD) und der Binaerwert
 * auf LEDs an.
 *
 * Beim Druecken der Tasten wird die entsprechende Hexadezimalziffer oder
 * Buchstabe auf der 7-Segment-Anzeige (SSD) angezeigt.
 */
void task2() {

	// Nur die erste Anzeigestelle wird eingeschaltet.
	ssdDigit0 = 1;
	ssdDigit1 = 0;
	ssdDigit2 = 0;
	ssdDigit3 = 0;

	// Die Hexadezimale Segment-Mustern (0...F) werden als Array initialisiert.
  	uint8_t patterns[] = {0xD7, 0x11, 0x8F, 0x9B, 0x59, 0xDA, 0xDE, 0X91,
  			0XDF, 0XDB, 0XDD, 0X5E, 0XC6, 0X1F, 0XCE, 0XCC};

  	// Der aktuelle Zustand der Tasten wird gespeichert.
	uint8_t previousKeyStates = keys.read();

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Der aktuelle Zustand der Tasten wird gespeichert.
		uint8_t newKeyStates = keys.read();

		// Prueft, ob einer der Tasten gedrueckt wuerde.
		if (newKeyStates != previousKeyStates) {

			// Der Binaerwert wird auf den vier recht LEDs dargestellt.
			leds.write((leds ^ newKeyStates)&0x0F);

			// newKeyStates wird an previousKeyStates zugewiesen.
			previousKeyStates = newKeyStates;
		}

		// Das abgerufene Muster ssdSegments wird zugewiesen, und wird auf dem
		// 7-Segment-Anzeige (SSD) angezeigt.
		ssdSegments = patterns[(keys & 0x0F)];

	}
}

/**
 *  Zeigt die 4 letzten Ziffern einer Matrikelnummer mit einem
 *  Dezimalpunkt auf dem SSD an.
 *
 *  Wegen des Zeitmultiplexverfahrens von der activateNextDigit()-Methode,
 *  laueft leuchtet das SSD flackerfrei.
 */
void task3(){
	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments,digits);

	// Die Ziffern "2550" werden anhand vordefiniertes Array "patterns"
	// ins Array des Objekts gespeichert.
	ssd[3] = 2;
	ssd[2] = 5;
	ssd[1] = 5;
	ssd[0] = 0;

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Falls Timer schon erreicht wuerde (5ms)...
		if (digitTimer.timeReached()) {

			// wird die naechste Ziffer auf der SSD angezeigt.
			ssd.activateNextDigit();
		}
	}
}

/**
 * Zeigt eine Folge von Fließkommawerten wird auf dem SSD an.
 *
 * Die Position des Dezimalpunktes wird automatisch angepasst, um den richtigen
 * Wert anzuzeigen.
 */
void task4() {
	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments,digits);

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Die Initialisierung eines Timers (1000 ms) fuer die Aktualisierung der
	// anzuzeigenden Ziffern auf der Anzeige des SSDs.
	CPolledTimer showNext(1000);

	// Die Initialisierung der Anzeigevariable mit dem anzuzeigenden Wert.
	float value = 0.0001234;

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Der anzuzeigende Wert wird mittels Ueberladener Operator zum
		// ssd (CSevenSegmentDisplay-Objekt) zugewiesen.
		ssd = value;

		// Falls Timer fuers Zeitmultiplexverfahren (5ms) schon erreicht wuerde.
		if (digitTimer.timeReached()) {

			// wird die naechsten Ziffer auf der SSD angezeigt.
			ssd.activateNextDigit();
		}

		// Falls Timer (1000 ms) schon erreicht wuerde...
		if (showNext.timeReached()) {

			// Der naechste anzuzeigende Wert wird berechnet.
			value *= 10;

			//Falls der anzuzeigende Wert ausserhalb dem gueltigen Bereich
			//liegt...
			if (value > 9999) {

				// Die Anzeigevariable wird mit dem Anfangswert zugewiesen.
				value = 0.0001234;
			}
		}
	}
}

/**
 * Zeigt der Spannung von einem Potentiometer auf einem SSD mit
 * Über-/Unterschreitungsanzeige.
 *
 * Wenn die Spannung außerhalb des Bereichs (25%-75%) liegt, wird LED 0
 * (ganz rechts) beleuchtet.
 */
void task5(){
	// Definition eines Schieberegister-Objekts zur Ansteuerung des SSDs.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock,&ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments,digits);

	// Der Dezimalpunkt wird ganz nach rechts gesetzt.
	ssd.setDecimalPoint(3);

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Die Initialisierung eines Timers (20 ms) fuer Spannungsmessung.
	CPolledTimer measurementTimer(20);

	// Definition eines globalen AnalogIn-Objekts.
	AnalogIn poti(BB_POTI,3.3f);

	// Das Analog Watch Dog (AWD) von Control Register 1 in Analog to Digital
	// Converter 1 (ADC1) wird aktiviert (Enable).
	ADC1->CR1 |= ADC_CR1_AWDEN;

	// Der obere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (3/4* Vref).
	ADC1->HTR = ADC_HTR_HT * 0.75;

	// Der untere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (1/4* Vref).
	ADC1->LTR = ADC_LTR_LT * 0.25;

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Falls Timer fuers Zeitmultiplexverfahren (5ms) schon erreicht wuerde.
		if (digitTimer.timeReached()) {

			// wird die naechsten Ziffer auf dem SSD angezeigt.
			ssd.activateNextDigit();
		}

		// Falls Timer fuer Spannungsmessung (20 ms) schon erreicht wuerde....
		if (measurementTimer.timeReached()) {

			// Spannung des Potentiometers wird abgelesen und mittels
			// ueberladenen Operator "=" auf dem SSD zugewiesen.
			ssd = poti.read_voltage();

			// Falls AWD-Flag von ADC1 schon gesetzt ist....
			if (ADC1->SR & ADC_SR_AWD) {

				// wird LED0 (ganz rechts) eingeschaltet.
				leds = 0x01;

				// wird das AWD-Flag im Statusregister (SR) in ADC1 geloescht.
				ADC1->SR &= 0;
			}

			// sonst...
			else {

				// wird das AWD-Flag nicht gesetzt, LED0 wird ausgeschaltet.
				leds = 0x00;
			}
		}
	}
}

/**
 * Zeigt gefilterter Spannungswerte von einem Potentiometer auf einem SSD an.
 *
 * Die gefilterten Spannungswerte des Potentiometers werden auf dem SSD
 * angezeigt. Einen gleitenden Mittelwertfilter wird angewendet, um Schwankungen
 * bzw. Rauschen zu reduzieren. Wenn die Spannung ausserhalb der Grenzen
 * (1/4...3/4) liegt, wird der LED 0 (ganz rechts) eingeschaltet.
 */
void task6() {
	// Definition eines Schieberegister-Objekts zur Ansteuerung des SSDs.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Der Dezimalpunkt wird ganz nach rechts gesetzt.
	ssd.setDecimalPoint(3);

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Die Initialisierung eines Timers (20 ms) fuer Spannungsmessung.
	CPolledTimer measurementTimer(20);

	// Die Initialisierung eines Filters mit Groesse von 50
	CMovingAverageFilter filter(50);

	// Definition eines globalen AnalogIn-Objekts.
	AnalogIn poti(BB_POTI, 3.3f);

	// Das Analog Watch Dog (AWD) von Control Register 1 in Analog to Digital
	// Converter 1 (ADC1) wird aktiviert (Enable).
	ADC1->CR1 |= ADC_CR1_AWDEN;

	// Der obere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (3/4* Vref).
	ADC1->HTR = ADC_HTR_HT * 0.75;

	// Der untere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (1/4* Vref).
	ADC1->LTR = ADC_LTR_LT * 0.25;

	// Die Anzeige wird in der while-Schleife aktualisiert.
	while (true) {
		// Falls Timer fuers Zeitmultiplexverfahren (5ms) schon erreicht wuerde.
		if (digitTimer.timeReached()) {

			// wird die naechsten Ziffer auf dem SSD angezeigt.
			ssd.activateNextDigit();
		}

		// Falls Timer fuer Spannungsmessung (20 ms) schon erreicht wuerde.
		if (measurementTimer.timeReached()) {

			// Messwerte in den Filter werden geschrieben
			filter << poti.read_u16();

			// Den Mittelwert wird auf dem Display angezeigt
			ssd = (float) filter*3.3f / 0xffff;

			// Falls AWD-Flag von ADC1 schon gesetzt ist....
			if (ADC1->SR & ADC_SR_AWD) {

				// wird LED0 (ganz rechts) eingeschaltet.
				leds = 0x1;

				// wird AWD-Flag im Statusregister (SR) in ADC1 geloescht.
				ADC1->SR &= 0;
			}

			// sonst...
			else {

				// wird das AWD-Flag nicht gesetzt, LED0 wird ausgeschaltet.
				leds = 0x0;
			}
		}
	}
}

/**
 * Zeigt exponentieller gefilterten Spannungswerten von einem Potentiometer auf
 * einem SSD an.
 *
 * Die gefilterten Spannungswerte des Potentiometers werden auf dem SSD
 * angezeigt. Ein Exponentialfilter mit einem Glaettungsfaktor wird angewendet,
 * um Schwankungen zu reduzieren. Wenn die Spannung ausserhalb der Grenzen
 * (1/4...3/4) liegt wird LED 0 (ganz rechts) eingeschaltet.
 */
void task7(){
	// Definition eines Schieberegister-Objekts zur Ansteuerung des SSDs.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts fuer die Anzeige des SSDs.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Der Dezimalpunkt wird ganz nach rechts gesetzt.
	ssd.setDecimalPoint(3);

	// Die Initialisierung eines Timers (5 ms) fuers Zeitmultiplexverfahren.
	CPolledTimer digitTimer(5);

	// Die Initialisierung eines Timers (20 ms) fuer Spannungsmessung.
	CPolledTimer measurementTimer(20);

	// Die Initialisierung des Faktors eines exponentiellen Filters von
	// 0.025.
	CExponentialFilter filter(0.025f);

	// Definition eines globalen AnalogIn-Objekts.
	AnalogIn poti(BB_POTI, 3.3f);

	// Das Analog Watch Dog (AWD) von Control Register 1 in Analog to Digital
	// Converter 1 (ADC1) wird aktiviert (Enable).
	ADC1->CR1 |= ADC_CR1_AWDEN;

	// Der obere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (3/4* Vref).
	ADC1->HTR = ADC_HTR_HT * 0.75;

	// Der untere Grenzwert (0xFFF) wird fuer ADC-Grenzwertueberwachung von ADC1
	// gesetzt (1/4* Vref).
	ADC1->LTR = ADC_LTR_LT * 0.25;

	while (true) {
		// Falls Timer fuers Zeitmultiplexverfahren (5ms) schon erreicht wuerde.
		if (digitTimer.timeReached()) {

			// wird die naechsten Ziffer auf dem SSD angezeigt.
			ssd.activateNextDigit();
		}

		// Falls Timer fuer Spannungsmessung (20 ms) schon erreicht wuerde.
		if (measurementTimer.timeReached()) {

			// Messwerte werden den Filter werden hineingefuegt.
			filter << poti.read_voltage();

			// Den Mittelwert wird auf dem Display angezeigt
			ssd = (float) filter;

			// Falls AWD-Flag von ADC1 schon gesetzt ist....
			if (ADC1->SR & ADC_SR_AWD) {

				// wird LED0 (ganz rechts) eingeschaltet.
				leds = 0x1;

				// wird AWD-Flag im Statusregister (SR) in ADC1 geloescht.
				ADC1->SR &= 0;
			}

			// sonst...
			else {

				// wird das AWD-Flag nicht gesetzt, LED0 wird ausgeschaltet.
				leds = 0x0;
			}
		}
	}
}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {
	// Falls notwendig, ergänzen Sie hier Aufrufe, um Konfigurationen
	// an globalen Objekten vorzunehmen, die nicht durch die Angabe von
	// Argumenten beim Aufruf des Konstruktors vorgenommen werden können.

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.

	keys.mode(PullDown);
//	task1();
//	task2();
//	task3();
//	task4();
//	task5();
//	task6();
	task7();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
	while (true) {
	}
}

