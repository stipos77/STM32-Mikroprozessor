/*
 * CMeasurementDevice.h
 *
 *  Created on: Nov 4, 2020
 *      Author: mnl
 */

#ifndef CMEASURINGDEVICE_H_
#define CMEASURINGDEVICE_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"
#include <string>


/* Repraesentiert die realisierte Anwendung "Messgeraet".
 *
 * Das Messgeraet zeigt den Messwert immer auf dem SSD, und entsprechend den
 * Anzeigemodus wird die Darstellung ueber LEDs oder PC-Terminal durchgefuehrt.
 *
 * Die Modi sind entweder ueber die PC-Tastatur oder ueber die Taster
 * einzustellen.
 *
 * Ein-Ausgaben auf der seriellen Schnittstelle:
 *
 * Breadboard-Taster    PC-Taste     Ausgabe auf der seriellen Schnittstelle
 * 7                      'b'           Balken
 * 6                      'c'           BCD
 * 5                      'i'           BinÃ¤r
 * 4                      'p'           PC
 * 3                      'e'           Einzelmessung gestartet
 * 2                      'f'           Dauermessung gestartet
 * 1                      'h'           Dauermessung pausiert oder fortgesetzt
 * 0                      's'           Dauermessung gestoppt
 * on                     'v'           'MIT-Digitalvoltmeter : SW-Rev. 0.1'
 * N/A                   sonst          'Unbekannter Befehl'
 */
class CMeasuringDevice {
private:

	// Definition der Aufzaehlungstype fuer die Anzeige bzw. die Messung gerade
	// eingestellten Modus
	enum DisplayMode{BALKEN, BCD, BINARY, PC};
	enum MeasurementMode{SINGLE, CONTINUOUS, HOLD, STOP};

	// aktueller Anzeigemodus.
	DisplayMode m_displayMode;

	// aktueller Messmodus.
	MeasurementMode m_measurementMode;

	// Deklaration von Zeiger auf das Sieben-Segment-Anzeige (SSD).
	CSevenSegmentDisplay *m_display;

	// Deklaration von Zeiger auf Taster.
	CDebouncer *m_keys;

	// Deklaration von Zeiger auf LEDs.
	BusOut *m_leds;

	// Deklaration von Zeiger auf die gepufferte serielle Schnittstelle.
	BufferedSerial *m_usart;

	// Deklaration von Zeiger auf den gepollten Timer.
	CPolledTimer* m_voltTimer;

	// Deklaration von Zeiger auf ADC fuer Spannungsmessung.
	AnalogIn* m_voltageInput;

	// aktueller gemessene Wert.
	float m_currVoltage;

	// Zuletzt gemessener Wert.
	float m_prevVoltage;

	/**
	 * Leuchtet je nach Messwert die LEDs als Balkenanzeige (keine LED oder die
	 * LED ganz links oder die beiden linken LEDs oder die ersten drei LEDs von
	 * links, die ersten vier LEDs von links usw. bis alle LEDs leuchten).
	 * (Anmerkung: UINT16_MAX = 0xFFFF oder 65535).
	 *
	 * @return auf den LEDs darzustellende Wert (11100000, 11111000, 11111110).
	 */
	uint8_t barDisplayMode();

	/**
	 * Berechnet der vom ADC gemessene Spannungswert als eine BCD-Ziffer. Bei
	 * dieser Darstellung werden jeweils vier Bits fuer die Repraesentation eine
	 * Dezimalstelle verwendet. Die Stelle vor dem Komma und die erste Stelle
	 * nach dem Komma werden zurueckgegeben.
	 *
	 * @return auf den LEDs darzustellende Wert (BCD z.B. 42= 0100 0010).
	 */
	uint8_t bcdDisplayMode();

	/**
	 * Verschiebt den gemessenen Spannungswert (bis zu 0xFFFF) um 8 Bits
	 * zurueck, und gibt den Binaerwert vom ADC zurueck.
	 *
	 * @return auf den LEDs darzustellende Wert (Binaerwert der Spannung).
	 */
	uint8_t binaryDisplayMode();

	/**
	 * Uebertragt der Messwert ueber die serielle Schnittstelle an den PC. Die
	 * Darstellung erfolgt als Fliesskommazahl mit Einheit („V“). Der Messwert
	 * wird immer linksbuendig in der gleichen Zeile angezeigt.
	 *
	 * @return 0 (alle LEDs werden waehrend dieser Darstellung ausgeschaltet).
	 */
	uint8_t pcDisplayMode();

	/**
	 * Ueberprueft der private-Anzeigemodus und ruft der entsprechenden
	 * DisplayMode-Funktion (BALKEN, BCD, BINARY, PC) auf.
	 *
	 * @return auf den LEDs darzustellende Wert.
	 */
	uint8_t setDisplayMode();

	/**
	 * Uerprueft die Eingabe von den 4 linken Tasten (deswegen mit &0xF0
	 * zugesichert), setzt den entsprechenden Anzeigemodus und sendet den
	 * korrespondierenden Text auf dem Terminal.
	 *
	 * @param keys der eingelesene Tastendruck, der folgende Optionen hat:
	 *
	 * 0x80 (Taste 7): Balken
	 * 0x40 (Taste 6): BCD
	 * 0x20 (Taste 5): Binaer
	 * 0x10 (Taste 4): PC
	 */
	void selectDisplayMode(uint8_t keys);

	/**
	 * Liest der aktuelle Wert des ADC einmal aus, aktualisiert der zuletzt
	 * gemessene Wert und die Anzeige, und endet aktive fortlaufende Messungen.
	 */
	void singleMeasureMode();

	/**
	 * Gibt der aktuelle Wert des ADC alle 100 ms neu zurueck.
	 *
	 * @return aktuelle Wert des ADCs (als 0xFFFF).
	 */
	uint16_t continuousMeasureMode();

	/**
	 * Ueberprueft der Messmodus, setzt den Haltemodus, und sendet Text auf
	 * dem Terminal dementsprechend. Falls Messgeraet bereits im Haltemodus
	 * waere, wird Messmodus zum Haltemodus gewechselt. Sonst, wird Messmodus
	 * zur Dauermessung gewechselt.
	 */
	void holdMeasureMode();

	/**
	 * Beendet die periodische Messung und setzt der zuletzt gemessene Wert
	 * (und damit die Anzeige) auf 0.
	 */
	void stopMeasureMode();

	/**
	 * Ueberprueft der private-Messmodus und ruft der entsprechenden
	 * MeasureMode-Funktion (SINGLE, CONTINUOUS, HOLD, STOP) auf.
	 *
	 * @return darzustellende Wert auf SSD und LEDs.
	 */
	uint16_t setMeasurementMode();

	/**
	 * Uerprueft die Eingabe von den 4 rechten Tasten (deswegen mit & 0x0F
	 * zugesichert), setzt den entsprechenden Messmodus und sendet den
	 * korrespondierenden Text auf dem Terminal.
	 *
	 * @param keys der eingelesene Tastendruck, der folgende Optionen hat:
	 *
	 * 0x08 Einzelmessung
	 * 0x04 Dauermessung
	 * 0x02 Fortlaufende Messung Anhalten
	 * 0x01 Fortlaufende Messung Stoppen
	 */
	void selectMeasurementMode(uint8_t keys);

	/**
	 * Sendet der im Parameter zu uebertragenden Text ueber die serielle
	 * Schnittstellen zur USART um Text auf dem Terminal darzustellen.
	 *
	 * @param text der auf dem Terminal darzustellende Text
	 */
	void sendDisplayText(const string &text);

	/**
	 * Ueberprueft die Eingabe aus PC-Taste und setzt den Anzeigemodus
	 * dementsprechend.
	 *
	 * @param keyBoard der eingelesene PC-Taste, der folgende Optionen hat:
	 *
	 * Taste 'b' Balken
	 * Taste 'c' BCD
	 * Taste 'i' BinÃ¤r
	 * Taste 'p' PC
	 * Taste 'v' Startnachricht
	 *
	 * sonst wird Eingabe weiter in processKeyboardMeasurement() ueberprueft.
	 *
	 * Anmerkung: ich gehe davon aus, dass die Implementierungen der private
	 * poll-Methoden nicht laenger als 30 Zeilen sein duerfen, deswegen habe ich
	 * zwei processKeyboard-Methoden geschrieben.
	 */
	void processKeyboardDisplay(char keyBoard);

	/**
	 * Ueberprueft die Eingabe aus PC-Taste und setzt den Messmodus
	 * dementsprechend. Bei unerkanntes Zeichen wird auf dem Terminal
	 * eine Fehlermeldung gesendet.
	 *
	 * @param keyBoard der eingelesene PC-Taste, der folgende Optionen hat:
	 *
	 * Taste 'e' Einzelmessung (SINGLE)
	 * Taste 'f' Dauermessung (CONTINUOUS)
	 * Taste 'h' Fortlaufende Messung Anhalten (HOLD)
	 * Taste 's' Dauermessung stoppen (STOP)
	 *
	 * sonst wird auf dem Terminal Fehlermeldung dargestellt.
	 */
	void processKeyboardMeasurement(char keyBoard);

	/**
	 * Ueberprueft, ob irgendeine PC-Taste gedrueckt wurde und aendert der
	 * Anzeige- bzw. Messmodus dementsprechend. Bei unerkannte Eingabe wird
	 * eine Fehlermeldung auf PC Terminal angezeigt.
	 */
	void pollUsart();

	/**
	 * Ueberprueft, ob irgendeine Taster gedrueckt wurde und aendert der
	 * Anzeige- bzw. Messmodus dementsprechend.
	 */
	void pollKeys();

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
	 * @param voltageInput der Analogeingang
	 */
	CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CMeasuringDevice();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf und aktualisiert fortlaufend die
	 * Messung bzw. Darstellung entsprechend der Modi auf SSD, LEDs und
	 * PC Terminal.
	 */
	void poll();
};


#endif /* CMEASURINGDEVICE_H_ */
