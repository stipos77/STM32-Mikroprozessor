/*
 * CPwmGenerator4.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Steven Atmodjo
 */

#ifndef CPWMGENERATOR4_H_
#define CPWMGENERATOR4_H_
#include "mbed.h"
#include "main.h"

/**
 * Modelliert unter Nutzung vom 16 Bit General Purpose Timer 3 einen Generator
 * fuer ein PWM-Signal auf PC6 und m_pwmOut. Die Periode ist auf Max. 20ms, mit
 * einer Ausgangsfrequenz des PSCs von 1MHz und einem einstellbaren Counter-
 * Modulo von Periode * 1000 festgelegt. Das Tastgrad (Ein/ ausverhaeltnis)
 * des PWMs (CCR1) wird einstellbar sein. Die Helligkeit von PC6 und m_pwmOut
 * werden beeinflusst. Es wird auch ein Interrupt-Handler implementiert.
 */
class CPwmGenerator4 {
private:
	// Das einzustellende Tastgrad.
	uint8_t m_percent;

	// Privater Zeiger auf DigitalOut Objekt.
	DigitalOut *m_pwmOut;

	/**
	 * Konfiguriert PC6 als Alternate Function-Ausgang und selektiert
	 * den von Timer 3/Compare Register 1 generierten Wert als das
	 * auszugebende Signal.
	 */
	void configureGpioPin();

	/**
	 * Konfiguriert den Timer 3 inklusive des Compare Register 1 so, dass ein
	 * Signal wie in der Klassenbeschreibung dargestellt erzeugt wird.
	 *
	 * @param period die Periodendauer
	 */
	void configureTimer(uint16_t period);

	/**
	 * Richtet die Interrupt-Behandlung für Timer 3 ein, indem er den Interrupt
	 * -Handler bindet, den Timer-3-Interrupt im NVIC aktiviert und die Update-
	 * und Capture/Compare-Interrupts freigibt.
	 */
	void configureInterrupt();

	/**
	 * Behandelt den UIF (Update Interrupt Flag) vom Timer 3 und IF (Interrupt
	 * Flag) von CC1. Beim Auftreten von UIF bzw. IF wird Flag im SR (Status
	 * Register) zurueckgesetzt, und das uebergegebene LED wird getoggelt.
	 */
	void handleInterrupt();
public:
	/**
	 * Erzeugt ein PWM-Signal mit uebergegebene Periode durch Aufruf der
	 * Konfigurationsmethoden.
	 *
	 * Zur Realisierung der Helligkeitssteuerung der LED 7
	 * muss die Periodendauer des PWM-Signals deutlich verkuerzt werden.
	 * Daher darf die Periodendauer hoechstens 20 ms (20000 Mikrosek) betragen.
	 * Es wird auch ein Interrupt-Handler des Timers aufgerufen.
	 *
	 * @param period die Periodendauer des Signals
	 */
	CPwmGenerator4(uint16_t period);

	/**
	 * Stellt das Tastgrad (0...100) eines PWM-Signals ein. Werte groesser als
	 * 100 werden wie 100 behandelt.
	 *
	 * @param percent Prozent des Tastgrads
	 */
	void setDutyCycle(uint8_t percent);

	/**
	 * Stellt ein DigitalOut Objekt fuer die Ausgabe des PWM-Signals ein.
	 *
	 * @param *pin Zeiger auf den Pin des LEDs
	 */
	void setPwmOut(DigitalOut *pin);
};
#endif /* CPWMGENERATOR4_H_ */
