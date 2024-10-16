/*
 * CRotateApp3.h
 *
 *  Created on: 15.02.2023
 *      Author: mnl
 */

#ifndef CROTATEAPP3_H_
#define CROTATEAPP3_H_

#include "CPatternController.h"

extern "C" void TIM1_BRK_TIM9_IRQHandler();
/**
 * Implementiert (unter Verwendung des Timers 9 und seinem Interrupt)
 * ein Lauflicht mit folgendem Verhalten:
 *
 *  * Am Anfang leuchten die beiden LEDs ganz rechts.
 *
 *  * Dann wird das Muster im Sekundentakt nach links rotiert.
 *
 * Implementierungshinweis: Rufen Sie aus der Interrupt-Routine mit
 * dem in der Vorlesung/Labor eingeführten Mechanismus die Methode
 * CRotateApp3::updatePattern auf.
 */
class CRotateApp3 {
private:
	CPatternController *m_pc;

	/**
	 * Wird immer nach Ablauf der Wartezeit aufgerufen,
	 * um das angezeigte Muster zu aktualisieren.
	 */
	void updatePattern();

	/**
	 * Initialisiert den Timer 9 im Interruptbetrieb.
	 */
	void initTimer9();

	// Entsprechend Hinweis (wie im Labor4 SSD)
	static CRotateApp3 * m_instance;

public:
	/**
	 * Initialisiert ein neues Exemplar der App.
	 *
	 * @param pc Zeiger auf den Pattern Controller
	 */
	CRotateApp3(CPatternController *pc);

	/**
	 * Implementiert die für die Klasse beschriebene Funktion
	 * unter Nutzung der privaten Methoden.
	 */
	void run();

	/**
	 * Behandelt den UIF (Update Interrupt Flag) vom Timer 9 und IF (Interrupt
	 * Flag) von CC1. Im public deklariert damit Zugriff auf private Attributen
	 * ermoeglicht wird.
	 */
	friend void TIM1_BRK_TIM9_IRQHandler();
};
#endif /* CROTATEAPP3_H_ */
