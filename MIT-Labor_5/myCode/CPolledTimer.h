/*
 * CPolledTimer.h
 *
 *  Created on: 01.10.2020
 *      Author: mnl
 */

#ifndef CPOLLEDTIMER_H_
#define CPOLLEDTIMER_H_

#include <stdint.h>

/**
 * Ein Zeittaktgeber, bei dem ueber eine Abfrage ermittelt werden kann,
 * ob der (periodische) Zielzeitpunkt erreicht wurde.
 */
class CPolledTimer {
private:
	// Eine Hilfsmethode, die die seit Start des Programms vergangene
	// Zeit in Millisekunden liefert.
	int64_t millisSinceStart();

	// Wartezeit des Timers.
	int32_t m_waitTime;

	// Zielzeit des Timers.
	int64_t m_targetTime;
public:
	/**
	 * Erzeugt ein neues Objekt mit einer Zielzeit, die sich aus
	 * der aktuellen Zeit plus der angegebenen Wartezeit ergibt.
	 *
	 * @param waitTime Wartezeit in Millisekunden
	 */
	CPolledTimer(int32_t waitTime = 1000);

	/**
	 * Prueft, ob die Zielzeit erreicht wurde. Falls nein, wird
	 * false zurueckgegeben. Falls ja, wird eine neue Zielzeit berechnet,
	 * indem der Wert von waitTime zur aktuellen Zielzeit addiert wird.
	 * Anschließend wird true zurueckgegeben.
	 *
	 * @return true, wenn die Zielzeit beim Aufruf erreicht war
	 */
	bool timeReached();

	 /**
	  * @brief Aktualisiert die Wartezeit und berechnet die neue Zielzeit.
	  *
	  * @param waitTime Wartezeit (Millisekunden)
	  */
	void updateWaitTime(int32_t waitTime);
};
#endif /* CPOLLEDTIMER_H_ */
