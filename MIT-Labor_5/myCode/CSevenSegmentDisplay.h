/*
 * CSevenSegmentDisplay.h
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#ifndef CSEVENSEGMENTDISPLAY_H_
#define CSEVENSEGMENTDISPLAY_H_
#include "CShiftRegisterOutputExtender.h"

// Deklaration der C-Funktion fuer Interrupt-Behandlung (von TIM10).
extern "C" void TIM1_UP_TIM10_IRQHandler();

/**
 * Modelliert eine 4-stellige 7-Segment-Anzeige. Um eine "aktive" und
 * flackerfreie Darstellung zu erhalten, die Methode "activateNextDigit" wird
 * alle 5 ms mittels Interrupt Handler aufgerufen.
 */
class CSevenSegmentDisplay {
private:
	// Deklaration eines vordefinierten Arrays fuer Bitmustern des SSDs.
	static uint8_t patterns[];

	// Zeiger auf Objekt von CShiftRegisterOutputExtender.
	CShiftRegisterOutputExtender* m_ssdSegments;

	// Zeiger auf Objekt von DigitalOut.
	DigitalOut* m_digits[4];

	// Index des Anzeigepunkts.
	uint8_t m_activeDigit;

	// Array von anzuzeigenden Werten.
	int m_values[4];

	// Position des Dezimalpunkts.
	uint8_t m_decimalDigit;

	// Helligkeit des SSDs.
	uint8_t m_brightness;

	// Zeiger auf Instanz dieser Klasse.
	static CSevenSegmentDisplay* m_instance;

	/**
	 * Hilfsmethode für das Zeitmultiplexverfahren. Aktiviert die nächste
	 * Anzeigestelle.
	 */
	void activateNextDigit();

	/**
	 * Konfiguriert den Timer 10 inklusive des Compare Register 1 so, dass ein
	 * Signal wie in der Klassenbeschreibung dargestellt erzeugt wird.
	 * Richtet auch die Interrupt-Behandlung.
	 */
	void initTimer();
public:
	/**
	 * Erzeugt ein neues Objekt, das eine 4-stellige 7-Segment-Anzeige
	 * ansteuert, deren Segmente (und Dezimalpunkt) an das über
	 * ssdSegments ansteuerbare Schieberegister angeschlossen sind und
	 * deren Anzeigestellen über die übergebenen digitalen Ausgänge
	 * aktiviert werden koennen.
	 *
	 * @param ssdSegments die Modellierung des Schieberegisters zur
	 *  Ansteuerung der Segemente
	 * @param digits die Ausgänge zum Aktivieren der Anzeigestellen
	 */
	CSevenSegmentDisplay(CShiftRegisterOutputExtender* ssdSegments,
			DigitalOut* digits[4]);

	/**
	 * Ermoeglicht den Zugriff auf die an der ausgewählten Anzeigestelle
	 * anzuzeigende Ziffer. Da eine Referenz auf den gespeicherten Wert
	 * zurückgeliefert wird, kann der Operator sowohl als Ziel einer
	 * Zuweisung als auch zum Abfragen des aktuell gespeicherten Wertes
	 * verwendet werden.
	 *
	 * @param position Index der Anzeigestelle (0 für Stelle ganz rechts)
	 * @return Referenz auf den an der ausgewählten Stelle anzuzeigenden Wert
	 */
	int& operator[] (int position);


	/**
	 * Stellt ein Dezimalpunkt auf der 7-Segment-Anzeige dar. Falls Eingabe
	 * im gueltigen Bereich liegt (0...3) wird der Dezimalpunkt auf der
	 * entsprechenden Position geleuchtet.
	 *
	 * @param digit Position des Dezimalpunkts auf 7-Segment-Anzeige.
	 */
	void setDecimalPoint(int digit);

	/**
	 * Stellt einen ganzzahligen Wert dar,der auf der SSD angezeigt werden soll.
	 *
	 * @param value Der anzuzeigende ganzzahlige Wert.
	 */
	void setValue(int value);

	/**
	 * Stellt einen Fliesskommawert dar, der auf SSD angezeigt werden soll.
	 *
	 * @param value Der anzuzeigende Fliesskommawert.
	 */
	void setValue(float value);

	/**
	 * Ueberladener Operator fuer ganzzahlige Werte, der die
	 * setValue(int value)-Methode aufruft zur Darstellung der Wert auf SSD.
	 *
	 * @param value Der anzuzeigende ganzzahlige Wert
	 * @return Referenz auf das aktualisierte CSevenSegmentDisplay-Objekts
	 */
	CSevenSegmentDisplay& operator=(int value);

	/**
	 * Ueberladener Operator fuer Fliesskommmawerte, der die
	 * setValue(float value)-Methode aufruft zur Darstellung der Wert auf SSD.
	 *
	 * @param value Der anzuzeigende Fliesskommawert
	 * @return Referenz auf das aktualisierte CSevenSegmentDisplay-Objekts
	 */
	CSevenSegmentDisplay& operator=(float value);

	/**
	 * Stellt die Helligkeit des SSDs zwischen 5% und 100% ein.
	 *
	 * @param percent Prozentsatz des Testgrads.
	 */
	void setBrightness(int percent);

	/**
	 * Gibt die Heilligkeit als Integer Zahl zurueck.
	 *
	 * @return die Helligkeit als ganzzahl.
	 */
	int getBrightness();

	/**
	 * Behandelt den UIF (Update Interrupt Flag) vom Timer 10 und IF (Interrupt
	 * Flag) von CC1. Im public deklariert damit Zugriff auf private Attributen
	 * ermoeglicht wird.
	 */
	friend void TIM1_UP_TIM10_IRQHandler();

	/**
	 * Schaltet Anzeige in Anzeigestelle "n" aus.
	 */
	void turnDisplayOff();
};

#endif /* CSEVENSEGMENTDISPLAY_H_ */
