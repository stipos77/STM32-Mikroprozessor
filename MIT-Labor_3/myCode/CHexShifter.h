/*
 * CHexShifter.h
 *
 *  Created on: Nov 1, 2020
 *      Author: mnl
 */

#ifndef CHEXSHIFTER_H_
#define CHEXSHIFTER_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"

/**
 * ReprÃ¤sentiert die realisierte Anwendung.
 *
 * Die Anwendung stellt auf der 7-Segment-Anzeige (SSD) eine 4-stellige
 * Hexadezimalzahl dar (zu Beginn 0000). Wenn Ã¼ber die serielle
 * Schnittstelle ein Zeichen 0..9 oder a..f (oder A..F) empfangen
 * wird, werden alle Stellen auf der 7-Segment-Anzeige um eine Position
 * nach links geschoben und der empfangene Wert wird auf der Stelle ganz
 * rechts dargestellt. Die damit neu entstandene Zahl wird als eine Zeile
 * (d.h. abgeschlossen mit CR/LF) Ã¼ber die serielle Schnittstelle an das
 * Terminal gesendet.
 *
 * Ãœber diese Grundfunktion hinaus unterstÃ¼tzt die Anwendung ein
 * Umschalten der EinfÃ¼gerichtung. Wird der Taster ganz rechts
 * gedrÃ¼ckt, erfolgt das EinfÃ¼gen von links. D.h. beim Empfang eines
 * neuen Zeichens werden die dargestellten Zeichen nach rechts
 * geschoben und das neue Zeichen wird ganz links eingefÃ¼gt. Das
 * ursprÃ¼ngliche Verhalten (rechts einfÃ¼gen) kann durch erneutes
 * DrÃ¼cken der Taste ganz rechts wieder hergestellt werden.
 *
 * WÃ¤hrend die Fuktionsweise auf "EinfÃ¼gen von links" umgestellt ist,
 * leuchtet die LED ganz rechts.
 */
class CHexShifter {
private:

	// Zeiger auf das Siebensegment-Display.
	CSevenSegmentDisplay* m_display;

	// Zeiger auf Taster.
	CDebouncer* m_keys;

	// Zeiger auf LEDs.
	BusOut* m_leds;

	// Zeiger auf die gepufferte serielle Schnittstelle.
	BufferedSerial* m_usart;

	/* Wenn true, kommen neue Zeichen auf SSD an die Stelle ganz links. */
	bool m_insertLeft;

	/**
	 * PrÃ¼ft, ob Ã¼ber die serielle Schnittstelle ein neues Zeichen
	 * empfangen wurde. Wenn das der Fall ist, wird das Zeichen
	 * zur Verarbeitung an processChar weitergegeben.
	 */
	void pollUsart();

	/**
	 * PrÃ¼ft, ob das Ã¼bergebene Zeichen zulÃ¤ssig ist. Wenn ja,
	 * konvertiert das Zeichen in eine Zahl zwischen 0 und 15
	 * und ruft processDigit mit dem Ergebnis auf.
	 *
	 * @param c das zu bearbeitende Zeichen
	 */
	void processChar(char c);

	/**
	 * Schiebt in der Grundfunktion alle Stellen auf der
	 * 7-Segment-Anzeige nach links und stellt den Ã¼bergebenen
	 * Wert auf der Stelle ganz rechts dar.
	 *
	 * Bei umschaltbarere EinfÃ¼gerichtung wird entsprechend
	 * der ausgewÃ¤hlten Richtung geschoben und eingefÃ¼gt.
	 *
	 * Ruft abschlieÃŸend sendDisplayedDigits auf.
	 *
	 * @param value der einzufÃ¼gende Wert
	 */
	void processDigit(uint8_t value);

	/**
	 * Sendet die aktuell in der 7-Segment-Anzeige dargestellten
	 * Zeichen als Zeile Ã¼ber die serielle Schnittstelle.
	 */
	void sendDisplayedDigits() const;

	/**
	 * PrÃ¼ft, ob der Taster ganz rechts gedrÃ¼ckt wurde und Ã¤ndert
	 * die EinfÃ¼gerichtung entsprechend.
	 */
	void pollKeys();

	/**
	 * Prueft der im Parameter uebergegebenen bool Wert und setzt die LED
	 * ganz rechts und Einfuegerichtung demenentsprechend.
	 */
	void setInsertLeft(bool leftOrRight);

public:
	/**
	 * Erzeugt ein neues Exemplar der Anwendung, das die angegebenen
	 * Hardware-Komponenten benutzt.
	 *
	 * Die Taster werden Ã¼ber ein Exemplar von CDebouncer entprellt.
	 * Da die Klasse CDebouncer keinen Default-Konstruktor hat, kann
	 * das Objekt nicht als Attribut definiert werden, sondern muss im
	 * Konstruktor auf dem Heap erzeugt und im Destruktor wieder
	 * freigegeben werden.
	 *
	 * @param display die 7-Segment-Anzeige
	 * @param keys die EingÃ¤nge, an die die Taster angeschlossen sind
	 * @param leds die AusgÃ¤nge, an die die LEDs angeschlossen sind
	 * @param usart die serielle Schnittstelle
	 */
	CHexShifter(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CHexShifter();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf.
	 */
	void poll();
};

#endif /* CHEXSHIFTER_H_ */
