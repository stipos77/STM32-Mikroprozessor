/*
 * CPolledTimer.cpp
 *
 *  Created on: 01.10.2020
 *      Author: mnl
 */

#include "mbed.h"
#include "CPolledTimer.h"
CPolledTimer::CPolledTimer(int32_t waitTime) {
	// eingegebene Wartezeit wird in m_waitTime zugewiesen.
	m_waitTime=waitTime;

	// Zielzeit ist gleich Wartezeit + Aktuelle Zeit
	m_targetTime=m_waitTime+millisSinceStart();
}

int64_t CPolledTimer::millisSinceStart() {
	// Aktuelle Zeit wird als Millisekunden zurueckgegeben
	return std::chrono::duration_cast<std::chrono::milliseconds>(
			HighResClock::now().time_since_epoch()).count();
}

bool CPolledTimer::timeReached() {
	// Falls aktuelle Zeit groesser gleich der Zielzeit waere...
	if(millisSinceStart()>=m_targetTime){

		// wird Zielzeit um Wartezeit inkrementiert.
		m_targetTime+=m_waitTime;

		// und wird True zurueckgegeben.
		return true;
	}

	// Sonst, wird false zurueckgegeben.
	return false;
}

void CPolledTimer::updateWaitTime(int32_t waitTime){
	// Die eingegebene Wartezeit wird in m_waitTime zugewiesen.
	m_waitTime=waitTime;

	// Die Zielzeit wird durch die Wartezeit und die aktuelle Zeit bestimmt.
	m_targetTime=millisSinceStart()+m_targetTime;
}
