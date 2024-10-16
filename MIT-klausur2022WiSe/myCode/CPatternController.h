/*
 * CPatternController.h
 *
 *  Created on: 15.02.2023
 *      Author: mnl
 */

#ifndef CPATTERNCONTROLLER_H_

#define CPATTERNCONTROLLER_H_

#include "../../mit048/mbed-os/mbed.h"

/**
 * Die Klasse manipuliert Bit-Muster in einem BusOut-Objekt.
 */
class CPatternController {
private:
	BusOut* m_bus;
	int m_pins;

public:
	/**
	 * Erzeugt einen neuen Pattern-Controller.
	 *
	 * @param bus Zeiger auf das BusOut-Objekt
	 * @param pins Anzahl der vom Bus angesteuerten Pins.
	 */
	CPatternController(BusOut* bus, int pins);

	/**
	 * Setzt alle Pins am Bus auf 0.
	 */
	CPatternController& reset();

	/**
	 * Verschiebt das Bit-Muster am Bus um die angegebene Anzahl
	 * Stellen. Dabei werden immer 0-Bits nachgeschoben.
	 *
	 * @param bits Anzahl der Bits, um die verschoben wird. Ein
	 * positiver Wert bedeutet Verschieben nach links, ein negativer
	 * Wert Verschieben nach rechts.
	 */
	CPatternController& operator<< (int bits);

	/**
	 * Rotiert das Bit-Muster am Bus um die angegebene Anzahl
	 * Stellen. Beim Rotieren um 1 wird das Bit-Muster um eine
	 * Position nach links verschoben, wobei niederwertigste
	 * Bit den Wert des höchstwertigen Bits vor dem Rotieren
	 * bekommt. Beim Rotieren nach rechts bekommt entsprechend
	 * das höchstwertige Bit den Wert des niederwertigsten vor
	 * dem Rotieren.
	 *
	 * Ein Rotieren um mehrere Stellen verhält sich wie das
	 * entsprechend oft durchgeführte Rotieren um eine Stelle.
	 *
	 * @param bits Anzahl der Bits, um die rotiert wird. Ein
	 * positiver Wert bedeutet Rotieren nach links, ein negativer
	 * Wert Rotieren nach rechts.
	 */
	CPatternController& rotate (int bits);

	/**
	 * Setzt das Bit an der angegeben Position auf den angegebenen
	 * Wert. Dabei ist position == 0 das niederwertigste Bit.
	 *
	 * @param position Position des Bits (0 ... Anzahl Bits - 1)
	 * @param value zu setzender Wert (0 oder 1)
	 */
	CPatternController& set (int position, int value);
};

#endif /* CPATTERNCONTROLLER_H_ */
