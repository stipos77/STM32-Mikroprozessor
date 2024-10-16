/*
 * CMeasurement.cpp
 *
 *  Created on: Jun 4, 2024
 *      Author: Steven Atmodjo
 */

#include "CMeasuringDevice.h"

CMeasuringDevice::CMeasuringDevice(CSevenSegmentDisplay* display,
	BusIn* keys, BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput){

	// Mitgliedsattribute werden mit den uebergegebenen Parameter initialisiert.
	m_display = display;
	m_leds = leds;
	m_usart = usart;

	// CDebouncer-Objekt wird mit 100 ms Verzoegerung initialisiert.
	m_keys = new CDebouncer(keys, 100);

	// Potentiometer wird mit im Parameter uebergegebenen Zeiger initialisiert.
	m_voltageInput = voltageInput;

	// Timer zur Spannungsmessung mit 100 ms Verzoegerungwird dynamisch erzeugt.
	m_voltTimer = new CPolledTimer(100);

	// Beim Einschalten wird die Anzeige auf Balken gesetzt...
	m_displayMode = BALKEN;

	// und die Messung wird am Anfang gestoppt.
	m_measurementMode = STOP;

	// Variablen fuer Spannungsmessung werden mit 0 initialisiert.
	m_currVoltage=0;
	m_prevVoltage=0;

	// Beim Einschalten wird Startnachricht auf dem Terminal dargestellt.
	sendDisplayText("MIT-Digitalvoltmeter : SW-Rev. 0.1");
}

CMeasuringDevice::~CMeasuringDevice(){

	// Speicher von dynamischen Attributen werden freigegeben.
	delete m_keys;
	delete m_voltTimer;
}

uint8_t CMeasuringDevice::barDisplayMode() {

	// Schiebevariable wird berechnet, um einen Wert von 0 bis 8 zu erhalten.
	uint8_t barDisplay = (int) setMeasurementMode()*8 /UINT16_MAX;

	//0xFF00 wird um Schiebevariable zurueckgeschoben und zurueckgegeben.
	return 0xFF00 >> barDisplay;
}

uint8_t CMeasuringDevice::bcdDisplayMode() {

	// Spannungswert wird zuerst berechnet. (0...3,3V)
	float voltageValue = setMeasurementMode() * 3.3 / UINT16_MAX;

	// Hilfsvariable wird mit dem oberen 4 Bits der Spannungswert initialisiert.
	uint8_t integerPart = static_cast<uint8_t>(voltageValue);

	// Hilfsvariable wird mit dem unteren 4 Bits der Spannungswert initialisiert
	uint8_t decimalPart = static_cast<uint8_t> ((voltageValue-integerPart)*10);

	// Falls die darzustellende Ziffer im gueltigen Bereich liegt... (0...9)
	if(integerPart < 10 && decimalPart < 10){

		// Kombinierte BCD-Anzeige.
		return (((integerPart << 4)&0xF0) | (decimalPart&0x0F));
	  }
	return 0;
}

uint8_t CMeasuringDevice::binaryDisplayMode() {

	// Zuletzt gemessener Spannungswert wird um 8 Bits zurueckgeschoben.
	uint8_t binaryLeds = (int) (setMeasurementMode()&0xFFFF) >> 8;

	// Danach wird der berechnete Wert zurueckgegeben.
	return binaryLeds;
}

uint8_t CMeasuringDevice::pcDisplayMode() {

	// Berechne die Spannung basierend auf dem zuletzt gemessenen Wert.
	m_currVoltage = static_cast<float>(setMeasurementMode()) * 3.3 / UINT16_MAX;

	// Falls Timer fuer Messung sein Intervall erreicht hat (100 ms)....
	if (m_voltTimer->timeReached()) {

		// wird eine String-Variable erstellt.
		string sendString = "Displaymodus: PC Messwert= ";

		// den Spannungswert wird zum String-Variable hinzugefuegt.
		sendString += to_string(m_currVoltage) + " V\r";

		// Den String wird ueber serielle Schnittstelle an den PC angezeigt.
		sendDisplayText(sendString);
	}

	// Alle LEDs werden waehrend PC-Darstellung ausgeschaltet.
	return 0;
}

void CMeasuringDevice::selectDisplayMode(uint8_t keys) {

	switch (keys & 0xF0) {

	case 0x80:
		sendDisplayText("Displaymodus: Balken");
		m_displayMode = BALKEN;
		break;

	case 0x40:
		sendDisplayText("Displaymodus: BCD");
		m_displayMode = BCD;
		break;

	case 0x20:
		sendDisplayText("Displaymodus: Binär");
		m_displayMode = BINARY;
		break;

	case 0x10:
		m_displayMode = PC;
		break;
	}
}

uint8_t CMeasuringDevice::setDisplayMode(){

	switch (m_displayMode) {

		case BALKEN:
			return barDisplayMode();

		case BCD :
			return bcdDisplayMode();

		case BINARY:
			return binaryDisplayMode();

		case PC:
			return pcDisplayMode();

		default :
			return 0;
	}
}

void CMeasuringDevice::singleMeasureMode(){

	// aktueller Messwert wird vom ADC ausgelesen.
	m_prevVoltage = m_voltageInput->read_u16();

	// Messmodus wird zu HOLD (Meessung anhalten) gewechselt.
	m_measurementMode = HOLD;
}

uint16_t CMeasuringDevice::continuousMeasureMode(){

	// Falls Timer fuer Spannungsmessung (100ms) schon erreicht wuerde....
	if(m_voltTimer->timeReached()){

		// wird der zuletzt gemessene Wert mit aktueller Messung aktualisiert.
		m_prevVoltage = m_voltageInput->read_u16();
	}

	// Spannungswert wird zurueckgegeben.
	return m_prevVoltage;
}

void CMeasuringDevice::holdMeasureMode(){

	// Falls Messgeraet bereits im Haltemodus waere...
	if (m_measurementMode != HOLD) {

		// wird Text auf dem Terminal gesendet.
		sendDisplayText("Dauermessung pausiert");

		// Messmodus wird zum Haltemodus gewechselt.
		m_measurementMode = HOLD;
	}

	// sonst..
	else {

		// wird Text auf dem Terminal gesendet.
		sendDisplayText("Dauermessung fortgesetzt");

		// Messmodus wird zur Dauermessung gewechselt.
		m_measurementMode = CONTINUOUS;
	}
}

void CMeasuringDevice::stopMeasureMode(){

	// Die Spannungswerte werden auf Null gesetzt.
	m_prevVoltage = 0;
	m_currVoltage = 0;

	// in den HALT-Modus wechseln.
	m_measurementMode = HOLD;
}

void CMeasuringDevice::selectMeasurementMode(uint8_t keys) {

	switch (keys & 0x0F) {

	case 0x08:
		sendDisplayText("Einzelmessung gestartet.");
		m_measurementMode = SINGLE;
		break;

	case 0x04:
		sendDisplayText("Dauermessung gestartet.");
		m_measurementMode = CONTINUOUS;
		break;

	case 0x02:

		// beim erneuten Tasterdruck wird Dauermessung forgesetzt.
		holdMeasureMode();
		break;

	case 0x01:

		// Messung wird gestoppt, nur wenn aktueller Modus Dauermessung ist.
		if(m_measurementMode==CONTINUOUS){
			sendDisplayText("Dauermessung gestoppt.");
			m_measurementMode = STOP;
		}
		break;
	}
}

uint16_t CMeasuringDevice::setMeasurementMode(){

	switch (m_measurementMode){

	case SINGLE:

		// Einzelmessung wird durchgeführt.
		singleMeasureMode();

		// aktualisierte Spannungswert wird zurueckgegeben.
		return m_prevVoltage;

	case CONTINUOUS:

		// aktueller Spannungswert wird zurueckgeliefert.
		return continuousMeasureMode();

	case HOLD:

		// zuletzt gemessener Spannungswert wird zurueckgegeben.
		return m_prevVoltage;

	case STOP:
		stopMeasureMode();
		return 0;

	default:
		return 0;
	}
}

void CMeasuringDevice::sendDisplayText(const string &text){

	// Den uebergegebenen Text wird ueber USART geschrieben.
	m_usart->write(text.c_str(), text.length());

	// Array mit Carriage Return und Line Feed werden initialisiert.
	char end[2] = { '\r' , '\n' };

	// Das Array wird ueber USART gesendet.
	m_usart->write(end, 2);
}

void CMeasuringDevice::processKeyboardDisplay(char keyboard) {

	switch (keyboard) {

	case 'b':
		selectDisplayMode(0x80);
		break;

	case 'c':
		selectDisplayMode(0x40);
		break;

	case 'i':
		selectDisplayMode(0x20);
		break;

	case 'p':
		selectDisplayMode(0x10);
		break;

	case 'v':
		sendDisplayText("MIT-Digitalvoltmeter: SW-Rev. 0.1");
		break;

	default:
		processKeyboardMeasurement(keyboard);
		break;
	}
}

void CMeasuringDevice::processKeyboardMeasurement(char keyboard) {

	switch (keyboard) {

	case 'e':
		selectMeasurementMode(0x08);
		break;

	case 'f':
		selectMeasurementMode(0x04);
		break;

	case 'h':
		selectMeasurementMode(0x02);
		break;

	case 's':
		selectMeasurementMode(0x01);
		break;

	default:
		sendDisplayText("Unbekannter Befehl");
		break;
	}
}

void CMeasuringDevice::pollUsart() {

	// Falls Daten im USART noch vorhanden sind...
	if (m_usart->readable()) {

		// wird ein Array zur Speicherung der Tastatureingabe initialisiert.
		char keyboard[1];

		// wird das Zeichen vom USART eingelesen.
		m_usart->read(keyboard, sizeof(keyboard));

		// werden die Eingabe weiter verarbeitet, um  Anzeige- bzw. Messmodus
		// zu aktualisieren.
		processKeyboardDisplay(keyboard[0]);
	}
}

void CMeasuringDevice::pollKeys(){

	// Hilfsvariable, um vorherigen Zustand der Taster zu speichern.
	static uint8_t previousKeyStates = m_keys->read();

	// aktuellen Zustand der Taster wird gespeichert.
	uint8_t newKeyStates = m_keys->read();

	// Falls irgendeine Taster gedrueckt wuerde...
	if(newKeyStates != previousKeyStates){

		// Hilfsvariable wird mit Tastereingabe aktualisiert.
		previousKeyStates = newKeyStates;

		// Anzeigemodus wird entsprechend Druck der 4 linken Tasten aktualisiert
		selectDisplayMode(newKeyStates & 0xF0);

		// Messmodus wird entsprechend Druck der 4 rechten Tasten aktualisiert.
		selectMeasurementMode(newKeyStates & 0x0F);
	}
}

void CMeasuringDevice::poll(){

	// Ueberwacht die Tastatureingaben ueber die USART-Schnittstelle.
	pollUsart();

	// Ueberwacht der Zustand der Tasten.
	pollKeys();

	// Zuletzt gemessener Spannungswert wird fuer SSD Darstellung berechnet.
	float displayVariable = (setMeasurementMode()&0xFFFF)* 3.3 / UINT16_MAX;

	// SSD wird mit zuletzt gemessener Spannungswert zugewiesen.
	*m_display = displayVariable;

	// LEDs wird entsprechend Messmodus geleuchtet.
	*m_leds = setDisplayMode();
}
