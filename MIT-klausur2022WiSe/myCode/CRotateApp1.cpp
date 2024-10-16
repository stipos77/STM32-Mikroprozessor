#include "CRotateApp1.h"

CRotateApp1::CRotateApp1(CPatternController *pc, BusIn *keys) {

	m_pc = pc;
	m_keys = keys;
}

void CRotateApp1::run() {

	// Am Anfang erst 2 LEDs ganz rechts leuchten.
	m_pc->set(0, 1);
	m_pc->set(1, 1);

	static int period = 1000;

	CPolledTimer timer(period);

	while (true) {

		// Nach Periode wird Anzeige aktualisiert.
		if (timer.timeReached()) {
			updatePattern();
		}

		if (pushedKeys() & 0x01) {
			m_direction = RIGHT;
		}

		if (pushedKeys() & 0x80) {
			m_direction = LEFT;
		}

		if (pushedKeys() & 0x10) {

			if(period>100){
				period*=0.9;
				timer = CPolledTimer(period);
			}
		}

		if (pushedKeys() & 0x08) {
			period*=1.1;
			timer = CPolledTimer(period);
		}
	}
}

uint8_t CRotateApp1::pushedKeys() {

	// 50ms Entprellzeit.
	CDebouncer bouncingKeys(m_keys, 1);

	return bouncingKeys.read();
}

void CRotateApp1::updatePattern() {

	if (m_direction == LEFT) {
		m_pc->rotate(1);
	}

	if (m_direction == RIGHT) {
		m_pc->rotate(-1);
	}
}
