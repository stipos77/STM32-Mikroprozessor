/*
 * CPwmGenerator2.h
 *
 *  Created on: Jun 25, 2024
 *      Author: Steven Atmodjo
 */

#ifndef CPWMGENERATOR2_H_
#define CPWMGENERATOR2_H_
#include <stdint.h>

/**
 * Modelliert unter Nutzung des 16 Bit General Purpose Timer 3s einen Generator
 * fuer ein PWM-Signal auf PC6. Die Periode ist mit einer Ausgangsfrequenz des
 * Prescalers von ca. 2048 Hz und einem Counter-Modulo von 16*2048 fest
 * auf 16 s eingestellt. Das Tastgrad (Ein/ausverhaeltnis) des PWMs (CCR1) wird
 * einstellbar sein. Die Helligkeit der LED (PC6) wird beeinflusst.
 */
class CPwmGenerator2 {
private:
	/**
	 * Konfiguriert PC6 als Alternate Function-Ausgang und selektiert
	 * den von Timer 3/Compare Register 1 generierten Wert als das
	 * auszugebende Signal.
	 */
	void configureGpioPin();

	/**
	 * Konfiguriert den Timer 3 inklusive des Compare Register 1 so,
	 * dass ein Signal wie in der Klassenbeschreibung dargestellt
	 * erzeugt wird.
	 */
	void configureTimer();
public:
	/**
	 * Erzeugt ein generiertes Signal durch Aufruf der Konfigurationsmethoden.
	 */
	CPwmGenerator2();

	/**
	 * Stellt das Tastgrad eines PWM-Signals ein. Der wert muss zwischen 0 und
	 * 100 liegen. Werte groesser als 100 werden wie 100 behandelt.
	 *
	 * @param percent, Prozent des Tastgrads
	 */
	void setDutyCycle(uint8_t percent);
};

#endif /* CPWMGENERATOR1_H_ */

