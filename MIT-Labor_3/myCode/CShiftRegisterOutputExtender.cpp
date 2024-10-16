/*
 * CShiftRegisterOutputExtender.cpp
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */
#include "CShiftRegisterOutputExtender.h"

CShiftRegisterOutputExtender::CShiftRegisterOutputExtender(
		DigitalOut *serialData, DigitalOut *shiftRegisterClock,
		DigitalOut *bufferRegisterClock) {

	// Die Initialisierung der Zeiger fuer Dateneingang des Schieberegisters.
	m_serialData = serialData;

	// Die Initialisierung der Zeiger fuer Takteingang des Schieberegisters.
	m_shiftRegisterClock = shiftRegisterClock;

	// Die Initialisierung der Zeiger fuer Takteingang des Pufferregisters.
	m_bufferRegisterClock = bufferRegisterClock;

	// Das vorbereitete "Segment-Muster" mit 0 initialisieren.
	m_preparedOutput=0;

	// Aktuelles "Segment-Muster" mit 0 initialisieren.
	m_currentOutput=0;
}

void CShiftRegisterOutputExtender::prepareOutput(uint8_t value) {

	// Der uebergegebene Wert wird negiert und in m_preparedOutput zugewiesen.
	m_preparedOutput = ~value;

	for(int shift = 7; shift >=0; shift--){

		// Der Wert wird an allen Ausgaengen QA-QH durch Ansteuern von
		// m_serialData und m_shiftRegisterClock in acht Taktzyklen eingegeben.
		*m_serialData = (m_preparedOutput>>shift) & 1;

		// Die Eingabe wird in das Schieberegister geschoben.
		*m_shiftRegisterClock = 1;
		*m_shiftRegisterClock = 0;
	}
}

void CShiftRegisterOutputExtender::updateOutput() {

	// Der Wert von m_currentOutput wird mit dem Wert von m_preparedOutput
	// aktualisiert.
	m_currentOutput = m_preparedOutput;

	// Die Eingabe wird in das Schieberegister geschoben.
	*m_bufferRegisterClock = 1;
	*m_bufferRegisterClock = 0;
}

void CShiftRegisterOutputExtender::write(uint8_t value) {

	//Die Ausgabe von "value" wird an den Ausgaengen des Schieberegisters
	//vorbereitet.
	prepareOutput(value);

	// Der Wert im Schieberegister wird in das Pufferregister uebernommen, damit
	// wird die Ausgaenge aktualisiert.
	updateOutput();
}

CShiftRegisterOutputExtender& CShiftRegisterOutputExtender::operator =(
		uint8_t value) {

	//Die "write"-Methode wird als ueberladenen Operator zur Verfügung gestellt.
	write(value);

	// Die Adresse dieser Methode wird als Pointer zurueckgegeben
	return *this;
}

uint8_t CShiftRegisterOutputExtender::read() const {

	// Der aktuelle Wert an den Ausgaenge wird zurueckgegeben
	return m_currentOutput;
}

CShiftRegisterOutputExtender::operator int() const {

	// Die "read()"-Methode wird als "cast-Operator" zurueckgegeben
	return read();
}
