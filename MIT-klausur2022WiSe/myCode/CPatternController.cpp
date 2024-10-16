#include "CPatternController.h"

CPatternController::CPatternController(BusOut *bus, int pins) {
	m_bus = bus;
	m_pins = pins;
}

/**
 * Setzt alle Pins am Bus auf 0.
 */
CPatternController& CPatternController::reset() {
	m_bus->write(0);
	return *this;
}

/**
 * Verschiebt das Bit-Muster am Bus um die angegebene Anzahl
 * Stellen. Dabei werden immer 0-Bits nachgeschoben.
 *
 * @param bits Anzahl der Bits, um die verschoben wird. Ein
 * positiver Wert bedeutet Verschieben nach links, ein negativer
 * Wert Verschieben nach rechts.
 */
CPatternController& CPatternController::operator<<(int bits) {

	if (bits > 0) {

		m_bus->write((*m_bus << bits) & 0xFF);
	}

	else {
		// 0 wird immer nachgeschoben.
		m_bus->write((*m_bus >> (-bits)) & 0xFF);
	}

	return *this;
}

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
CPatternController& CPatternController::rotate(int bits) {

	//stellt sicher, dass die Anzahl von Bits -6...6 ist.
	bits = bits % 6;

	if (bits < 0) {
		bits += m_pins;
	}

	int leftLeds = (m_bus->read() << bits) & 0xFF;
	int rightLeds = (m_bus->read() >> (m_pins - bits)) & 0xFF;

// entsprechend Bitbreite anpassen.
	*m_bus = (leftLeds|rightLeds)&0xFF;
	return *this;
}

/**
 * Setzt das Bit an der angegeben Position auf den angegebenen
 * Wert. Dabei ist position == 0 das niederwertigste Bit.
 *
 * @param position Position des Bits (0 ... Anzahl Bits - 1)
 * @param value zu setzender Wert (0 oder 1)
 */
CPatternController& CPatternController::set(int position, int value) {

	if ((0 <= position) && (position < m_pins)
			&& (((value == 0)) || (value == 1))) {
		int bitMask = *m_bus;

		if (value == 0) {

			// set 0 yg digeseer
			bitMask &= ~(1 << position);
		}

		else if (value == 1) {
			//set 1 yg digeser
			bitMask |= (1 << position);
		}

		m_bus->write(bitMask);
	}

	return *this;
}
