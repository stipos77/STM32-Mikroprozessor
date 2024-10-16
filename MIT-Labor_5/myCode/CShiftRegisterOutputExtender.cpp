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
	m_preparedOutput = 0;

	// Aktuelles "Segment-Muster" mit 0 initialisieren.
	m_currentOutput = 0;
}

void CShiftRegisterOutputExtender::prepareOutput(uint8_t value) {
	// Deklaration von PC7 (Messausgang) zur Ansteuerung.
	DigitalOut pc7(PC_7);

	// Hilfsvariable fuer Informationen der DigitalOut-Pin.
	GPIO_TypeDef *pc_7;
	uint32_t pinMask;

	gpioPortInfo(pc7, pc_7, pinMask);

	//PinPC_7 (Messausgang) wird auf logisch 1 gesetzt.
	pc_7->BSRR = pinMask;

// Hilfsvariable fuer Informationen der SRC-Eingang.
	GPIO_TypeDef *clockPort;
	uint32_t clockMask;

	// Informationen fuer den Takt-Pin in GPIO-Port werden abgeholt.
	gpioPortInfo(*m_shiftRegisterClock, clockPort, clockMask);

	// Hilfsvariable fuer Informationen der SER-Eingang.
	GPIO_TypeDef *dataPort;
	uint32_t dataMask;

	// Informationen fuer den SER-Eingang werden abgeholt.
	gpioPortInfo(*m_serialData, dataPort, dataMask);

	// Daten werden in das Schieberegister verschoben.
	doShift(clockPort, clockMask, dataPort, dataMask, value);

	// Die vorbereitete Ausgabe wird mit dem uebergebenen Wert.
	m_preparedOutput = ~value;

	// PinPC_7 (Messausgang) wird auf logisch 0 gesetzt.
	pc_7->BSRR = pinMask >> 16;
}

void CShiftRegisterOutputExtender::updateOutput() {
	// Der Wert von m_currentOutput wird mit dem Wert von m_preparedOutput
	// aktualisiert.
	m_currentOutput = ~m_preparedOutput;

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

void CShiftRegisterOutputExtender::doShift(GPIO_TypeDef *clockPort,
		uint32_t clockMask, GPIO_TypeDef *dataPort, uint32_t dataMask,
		uint8_t value) {
	// Der Wert wird an allen Ausgaengen QA-QH durch Ansteuern von
	// m_serialData und m_shiftRegisterClock in acht Taktzyklen eingegeben.
	for (int shift = 7; shift >= 0; shift--) {

		// Falls negierte zu bearbeitende Wert in Position shift 1 ist...
		if ((~value >> shift) & 1) {

			// wird in Position shift HIGH ins Serial Data Register eingegeben.
			dataPort->BSRR = dataMask;
		}

		// Sonst...
		else {

			// wird in Position shift LOW ins SER eingegeben.
			dataPort->BSRR = (dataMask << 16);
		}
		// Shift Register Clock (SRC) wird HIGH gesetzt.
		clockPort->BSRR = clockMask;

		// Ein Qualifizierer, der 10 Iterationen  durchlaeuft, um
		// sicherzustellen, dass der Compiler nicht optimiert und entfernt.
		for (volatile int i = 0; i < 10; i++);

		// Shift Register Clock (SRC) wird LOW gesetzt.
		clockPort->BSRR = (clockMask << 16);

		// Die Verzoegerung fuer SRC wird nochmal sichergestellt.
		for (volatile int i = 0; i < 10; i++);
	}
}
