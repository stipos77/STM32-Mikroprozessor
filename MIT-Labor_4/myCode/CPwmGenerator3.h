/*
 * CPwmGenerator3.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Steven Atmodjo
 */

#ifndef CPWMGENERATOR3_H_
#define CPWMGENERATOR3_H_
#include "mbed.h"

/**
 * Modelliert unter Nutzung vom 16 Bit General Purpose Timer 3 einen Generator
 * fuer ein PWM-Signal auf PC6. Die Periode ist mit einer Ausgangsfrequenz des
 * PSCs von ca. 1 MHz und einem einstellbaren Counter-Modulo von "Periode - 1"
 * eingestellt. Die Periode darf hoeschstens 20 ms sein, damit kein Flackern
 * vom Sicht der menschlichen Augen sichtbar wird. Das Tastgrad (Ein/
 * ausverhaeltnis) des PWMs (CCR1) wird einstellbar sein. Die Helligkeit der LED
 * (PC6) wird beeinflusst.
 */
class CPwmGenerator3 {
private:
	// Die Periodendauer vom PWM3 (TIM3), die im Bereich von 1...65536 liegen.
	uint16_t m_period;

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
	 * Erzeugt ein generiertes Signal mit einer uebergegebenen Periode durch
	 * Aufruf der Konfigurationsmethoden
	 *
	 * @param period, die Zeit einer PWM-Periode [1 ..65536 Mikrosekunden].
	 */
	CPwmGenerator3(uint16_t period);

	/**
	 * Ermoeglicht die Einstellung des Tastgrads.
	 *
	 * @param percent, das Tastgrad, der zwischen 0 und 100 liegen sollte.
	 */
	void setDutyCycle(uint8_t percent);
};

#endif /* CPWMGENERATOR3_H_ */
