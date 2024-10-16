/*
 * CRotateApp1.h
 *
 *  Created on: 15.02.2023
 *      Author: mnl
 */

#ifndef CROTATEAPP1_H_

#define CROTATEAPP1_H_

#include "../../mit048/mbed-os/mbed.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"
#include "CPatternController.h"

/**
 * Implementiert (unter Verwendung der Klassen CDebouncer,
 * CPolledTimer und CPatternController) ein Lauflicht
 * mit folgendem Verhalten:
 *
 *  * Am Anfang leuchten die beiden LEDs ganz rechts.
 *
 *  * Dann wird das Muster im Sekundentakt nach links rotiert.
 *
 *  * Wird der Taster ganz rechts gedrückt, wird das Muster
 *    nach rechts (statt nach links) rotiert.
 *
 *  * Wird der Taster ganz links gedrückt, wird das Muster
 *    (wieder, wie am Anfang) nach links rotiert.
 *
 *  * Bei jedem Drücken des vierten Tasters von rechts wird
 *    der Takt um den Faktor 1,1 erhöht bis der Maximalwert
 *    von 10 Hz erreicht ist.
 *
 *  * Bei jedem Drücken des vierten Tasters von links wird
 *    der Takt um den Faktor von 1,1 verringert.
 *
 * Implementierungshinweis: Da die Klasse CPolledTimer keine
 * Methode zum Ändern der Wartezeit bereitstellt, müssen Sie
 * eine Lösung finden, die ohne eine solche Methode auskommt.
 */
class CRotateApp1 {
private:
	/*********************************************************
	 * Es dürfen keine weiteren Attribute deklariert werden. *
	 *********************************************************/

	CPatternController *m_pc;
	BusIn *m_keys;
	/** Gibt die aktuelle eingestellte Laufrichtung an. */
	enum {
		LEFT, RIGHT
	} m_direction = LEFT;
	/** Gibt das Intervall (in Sekunden) an, in dem ein verändertes
	 * Muster angezeigt wird. */
	float m_updateInterval;

	/**
	 * Liefert eine Bit-Maske mit einer 1 für jeden Taster,
	 * der seit dem letzten Aufruf gedrückt wurde.
	 * Implementierungshinweise: (a) Verwenden Sie eine lokale
	 * statische Variable. (b) In der Implementierung darf nur
	 * einmal auf m_keys zugegriffen werden.
	 */
	uint8_t pushedKeys();

	/**
	 * Wird immer nach Ablauf der Wartezeit aufgerufen,
	 * um das angezeigte Muster zu aktualisieren.
	 */
	void updatePattern();

public:
	/**
	 * Initialisiert ein neues Exemplar der App.
	 *
	 * @param pc Zeiger auf den Pattern Controller
	 * @param keys Zeiger auf den Eingangsbus, den den
	 * die Taster angeschlossen sind
	 */
	CRotateApp1(CPatternController *pc, BusIn *keys);

	/**
	 * Implementiert die für die Klasse beschriebene Funktion
	 * unter Nutzung der privaten Methoden.
	 */
	void run();
};

#endif /* CROTATEAPP1_H_ */
