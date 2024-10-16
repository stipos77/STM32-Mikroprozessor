/*
 * CRotateApp2.h
 *
 *  Created on: 15.02.2023
 *      Author: mnl
 */

#ifndef CROTATEAPP2_H_
#define CROTATEAPP2_H_

#include "CPatternController.h"

/**
 * Implementiert (unter Verwendung des Timers 9) und Polling
 * des Update Flags ein Lauflicht mit folgendem Verhalten:
 *
 *  * Am Anfang leuchten die beiden LEDs ganz rechts.
 *
 *  * Dann wird das Muster im Sekundentakt nach links rotiert.
 */
class CRotateApp2 {
private:
	/*********************************************************
	 * Es dürfen keine weiteren Attribute deklariert werden. *
	 *********************************************************/

	CPatternController* m_pc;

	/**
	 * Wird immer nach Ablauf der Wartezeit aufgerufen,
	 * um das angezeigte Muster zu aktualisieren.
	 */
	void updatePattern();

	/**
	 * Initialisiert den Timer 9.
	 */
	void initTimer9();

public:
	/**
	 * Initialisiert ein neues Exemplar der App.
	 *
	 * @param pc Zeiger auf den Pattern Controller
	 */
	CRotateApp2(CPatternController* pc);

	/**
	 * Implementiert die für die Klasse beschriebene Funktion
	 * unter Nutzung der privaten Methoden.
	 */
	void run();
};

#endif /* CROTATEAPP2_H_ */
