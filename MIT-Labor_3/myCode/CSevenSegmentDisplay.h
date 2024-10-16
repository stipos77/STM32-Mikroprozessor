/*
 * CSevenSegmentDisplay.h
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#ifndef CSEVENSEGMENTDISPLAY_H_
#define CSEVENSEGMENTDISPLAY_H_
#include "CShiftRegisterOutputExtender.h"

/**
 * Modelliert eine 4-stellige 7-Segment-Anzeige. Um eine "aktive" und
 * flackerfreie Darstellung zu erhalten, muss sichergestellt werden,
 * dass die Methode "activateNextDigit" mindestens alle 5 ms aufgerufen wird.
 */
class CSevenSegmentDisplay {
private:
	// Deklaration eines vordefinierten Arrays fuer Bitmustern des SSDs.
	static uint8_t patterns[];
	CShiftRegisterOutputExtender* m_ssdSegments;
	DigitalOut* m_digits[4];
	uint8_t m_activeDigit;
	uint8_t m_values[4];
	uint8_t m_decimalDigit;

public:
	/**
	 * Erzeugt ein neues Objekt, das eine 4-stellige 7-Segment-Anzeige
	 * ansteuert, deren Segmente (und Dezimalpunkt) an das über
	 * ssdSegments ansteuerbare Schieberegister angeschlossen sind und
	 * deren Anzeigestellen über die übergebenen digitalen Ausgänge
	 * aktiviert werden können.
	 *
	 * @param ssdSegments die Modellierung des Schieberegisters zur
	 *  Ansteuerung der Segemente
	 * @param digits die Ausgänge zum Aktivieren der Anzeigestellen
	 */
	CSevenSegmentDisplay(CShiftRegisterOutputExtender* ssdSegments,
			DigitalOut* digits[4]);

	/**
	 * Hilfsmethode für das Zeitmultiplexverfahren. Aktiviert die nächste
	 * Anzeigestelle.
	 */
	void activateNextDigit();

	/**
	 * Ermöglicht den Zugriff auf die an der ausgewählten Anzeigestelle
	 * anzuzeigende Ziffer. Da eine Referenz auf den gespeicherten Wert
	 * zurückgeliefert wird, kann der Operator sowohl als Ziel einer
	 * Zuweisung als auch zum Abfragen des aktuell gespeicherten Wertes
	 * verwendet werden.
	 *
	 * @param position Index der Anzeigestelle (0 für Stelle ganz rechts)
	 * @return Referenz auf den an der ausgewählten Stelle anzuzeigenden Wert
	 */
	uint8_t& operator[] (int position);


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
};

#endif /* CSEVENSEGMENTDISPLAY_H_ */
