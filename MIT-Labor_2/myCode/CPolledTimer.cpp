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
	// Die aktuelle Zeit wird als Millisekunden zurückgegeben
	return std::chrono::duration_cast<std::chrono::milliseconds>(
			HighResClock::now().time_since_epoch()).count();
}

bool CPolledTimer::timeReached() {
	// Ist die aktuelle Zeit groeßer oder gleich der Zielzeit
	if(millisSinceStart()>=m_targetTime){
		//Falls ja, Zielzeit um Wartezeit inkrementiert
		m_targetTime+=m_waitTime;
		// Es wird true zuruckgegeben, wenn Zielzeit erreicht wurde
		return true;
	}
	// Falls nicht, war Zielzeit nicht erreicht. False wird zurueckgegeben
	return false;
}

void CPolledTimer::updateWaitTime(int32_t waitTime){
	// Die eingegebene Wartezeit wird in m_waitTime zugewiesen
	m_waitTime=waitTime;
	// Die Zielzeit wird durch die Wartezeit und die aktuelle Zeit bestimmt
	m_targetTime=millisSinceStart()+m_targetTime;
}
