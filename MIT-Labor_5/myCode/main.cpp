#include "mbed.h"
#include "breadboard.h"
#include "CRtc.h"
#include "CSevenSegmentDisplay.h"
#include "CShiftRegisterOutputExtender.h"
#include "GpioUtil.h"
#include "CPolledTImer.h"
#include "CExponentialFilter.h"
#include "CDebouncer.h"
#include "CHexShifter.h"
#include "math.h"

// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

// Definition eines globalen BusIn-Objekts.
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3,
BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);

// Definition eines globalen BusOut-Objekts.
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5,
BB_LED_6, BB_LED_7);

// Definition von DigitalOut-Objekten fuer die Pins des Schieberegisters
// (SER, SCLK, RCLK).
DigitalOut ssdSerialData(SSD_SER);
DigitalOut ssdShiftRegisterClock(SSD_SCLK);
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

// Definition von DigitalOut-Objekten fuer die 4-Anzeigedigits.
DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);

// Ein- bzw. Ausschaltgaenge der Ziffern der 7-Segmentanzeige.
DigitalOut *digits[] = { &ssdDigit3, &ssdDigit2, &ssdDigit1, &ssdDigit0 };

// Deklaration eines globalen AnalogIn Objekts.
AnalogIn poti(BB_POTI, 3.3);

/*
 * Prieft, ob neue Implementierung bei CShiftRegisterOutputExtender- Klasse
 * funktioniert.
 */
void task1() {
	// Definition eines Schieberegister-Objekts.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Die letzte 4 Ziffern meiner Matrikelnummer wird an SSD zugewiesen.
	ssd = 2550;

	//Helligkeit wird zu 80% gesetzt.
	ssd.setBrightness(80);

	while (true) {
	}
}

/**
 * Setzt die SSD Darstellung, LEDs Darstellung (freiwillig) und Ueberprueft
 * den Zustand der Taster, setzt die LEDs Darstellung demenetsprechend und
 * inkrementiert die Zeit auf SSD beim Drucken der 4 rechten Tasten.
 */
void task2() {
	// Definition eines Schieberegister-Objekts.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Timer mit 1 Sekunde Wartezeit wird eingestellt.
	CPolledTimer timer(1000);

	// Entprellzeit der Tasten von 1ms.
	CDebouncer keysDebounced(&keys, 1);

	// Anzeige-Modus der LEDs.
	enum DISPLAY_MODES {
		BCD, BALKEN, CYCLE
	};

	// Variable fuer Anzeigemodus wird mit BCD-Darstellung initialisiert.
	DISPLAY_MODES displayMode = BCD;

	// Schiebevariable der LEDs wird mit 0 initialisiert.
	int shift = 0;

	// Timer mit 1/9 Sekunden Periode.
	CPolledTimer barTimer(111);

	// Objekt der Klasse CRtc.
	CRtc realTimeClk;

	// Hier rufen Sie die Funktion auf, um die vergangene Zeit
	// seit dem Start zu erhalten.
	uint32_t secondsSinceStart = realTimeClk.secondsSinceMidnight();

	// Hier koennten Sie die Stunden und Minuten aus den vergangenen
	uint32_t hours = secondsSinceStart / 3600;
	uint32_t minutes = (secondsSinceStart % 3600) / 60;

	while (true) {
		// Taster werden eingelesen.
		int newKeyStates = keysDebounced.read();

		// Ueberprueft der Anzeigemodus.
		switch (displayMode) {

		// BCD Darstellung,
		case BCD:

			// SSD Stellen 0 und 1 (Sekunden) wird zu BCD umgewandelt.
			int leftLeds = (ssd[1] << 4) & 0xF0;
			int rightLeds = (ssd[0]) & 0x0F;

			// leds wird als BCD Darstellung gesetzt.
			leds = (leftLeds | rightLeds);

			break;

			// Balkenanzeige.
		case BALKEN:

			// Falls 1/9 einer Minute erreicht wuerde,,
			if (barTimer.timeReached()) {

				// wird 0xFF um Schiebevariable nach rechts geschoben
				leds = 0xFF >> shift;

				// Schiebevariable wird um 1 inkrementiert.
				shift++;

				// Schiebevariable wird immer im gueltigen Bereich 0...7.
				if (shift == 7) {
					shift = 0;
				}
			}
			break;

			// Sekundentakt
		case CYCLE:

			// LEDs werden ausgeschaltet.
			leds = 0;

			// Falls Timer erreicht wuerde...
			if (timer.timeReached()) {

				// wird Dezimalpunkt in Stelle 1.
				ssd.setDecimalPoint(1);
			}

			// Sonst...
			else {

				// wird Dezimalpunkt in Stelle 2.
				ssd.setDecimalPoint(2);
			}
			break;
		}

		// // Falls Taster 5 gedrueckt wuerde: BCD Darstellung.
		if (newKeyStates & 0x10) {
			displayMode = BCD;
		}

		// // Falls Taster 6 gedrueckt wuerde: Balkenanzeige.
		if (newKeyStates & 0x20) {
			displayMode = BALKEN;
		}

		// // Falls Taster 7 gedrueckt wuerde: Sekundentakt-anzeige.
		if (newKeyStates & 0x40) {
			displayMode = CYCLE;
		}

		// Falls Taster 4 gedrueckt wuerde...
		if (newKeyStates & 0x08) {

			// werden Stundem um 10 inkrementiert.
			hours = hours + 10;

			// Stunden werden immer im gueltigen Bereich 0...24.
			if (hours > 24) {
				hours = hours - 20;
			}
		}

		// Falls Taster 3 gedrueckt wuerde...
		if (newKeyStates & 0x04) {

			// wird Stunden um 10 inkrementiert.
			hours = hours + 1;

			// Stunden werden immer im gueltigen Bereich 0...10.
			if (hours % 10 == 0) {
				hours = hours - 10;
			}

			// Stunden werden immer im gueltigen Bereich 0...24.
			if (hours > 24) {
				hours = hours - 4;
			}
		}

		// Falls Taster 2 gedrueckt wuerde...
		if (newKeyStates & 0x02) {

			// wird Minuten um 10 inkrementiert.
			minutes = minutes + 10;

			// Minuten werden immer im gueltigen Bereich 0...60.
			if (minutes > 60) {
				minutes = minutes - 60;
			}
		}

		// Falls Taster 1 gedrueckt wuerde...
		if (newKeyStates & 0x01) {

			// wird Minuten um 1 inkrementiert.
			minutes = minutes + 1;

			// Minuten werden immer im gueltigen Bereich 0...10.
			if ((minutes % 10 == 0)) {
				minutes = minutes - 10;
			}
		}

		// Sekundenanzeige
		if (keys & 0x80) {
			ssd[3] = -1;
			ssd[2] = -1;
			ssd[1] = minutes / 10;
			ssd[0] = minutes % 10;
		}

		// Die Zeit wird auf SSD dargestellt.
		ssd[3] = hours / 10; // Zehnerstelle der Stunden
		ssd[2] = hours % 10; // Einerstelle der Stunden
		ssd[1] = minutes / 10; // Zehnerstelle der Minuten
		ssd[0] = minutes % 10; // Einerstelle der Minuten
	}
}

// (Freiwillig)
void task3() {
	// Definition eines Schieberegister-Objekts.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Sekundentakt Timer.
	CPolledTimer timer(1000);

	// Entprellzeit von 500ms.
	CDebouncer keysDebounced(&keys, 500);

	CExponentialFilter filter(0.03);

	CRtc realTimeClk;

	CRtc timeOut;

	// Zwei- Tastereinstellung (freiwillig).
	enum SETTING_MODES {
		NONE, HOUR_UNIT, HOUR_TENS, MINUTE_UNIT, MINUTE_TENS
	};

	SETTING_MODES settingModes = NONE;

	// Hier rufen Sie die Funktion auf, um die vergangene Zeit
	// seit dem Start zu erhalten.
	uint32_t secondsSinceStart = realTimeClk.secondsSinceMidnight();

	// Hier koennten Sie die Stunden und Minuten aus den vergangenen
	uint32_t hours = secondsSinceStart / 3600;
	uint32_t minutes = (secondsSinceStart % 3600) / 60;

	while (true) {

		if ((ssd[3] * ssd[2] <= 23 * 60) && (8 <= (hours * 60 + minutes))) {
			ssd.setBrightness(filter * 100);
		}

		else {
			ssd.setBrightness(filter * 70);
		}

		if (keys & 0x01) {

			while (keys & 0x01) {
				ssd = -1;
				timer = CPolledTimer(1000);

				if (keys & 0x02) {
					settingModes = static_cast<SETTING_MODES>((settingModes + 1)
							% 5);
				}

				// Pro Sekunde inkrementiert.
				if (settingModes != NONE && !timer.timeReached()) {
					switch (settingModes) {
					case NONE:
						break;

					case HOUR_UNIT:
						hours = (hours + 1) % 24;
						break;

					case HOUR_TENS:
						hours = (hours + 10) % 24;
						break;

					case MINUTE_UNIT:
						minutes = (minutes + 1) % 60;
						break;

					case MINUTE_TENS:
						minutes = (minutes + 10) % 60;
						break;
					}
				}
			}
		}
		// Die Zeit wird auf SSD dargestellt.
		ssd[3] = hours / 10; // Zehnerstelle der Stunden
		ssd[2] = hours % 10 % 24; // Einerstelle der Stunden
		ssd[1] = minutes / 10; // Zehnerstelle der Minuten
		ssd[0] = minutes % 10; // Einerstelle der Minuten

	}
}
//	while (true) {
//
//		filter << poti.read();
//
//		static uint32_t minutesSinceStart = hours*60+minutes;
//		// Falls Uhrzeit zw. 8 und 23 Uhr ist...
//		if ((minutesSinceStart <= 23 * 60)
//				&& (8 <= (hours * 60 + minutes))) {
//
//			// wird die Helligkeit normal sein.
//			ssd.setBrightness(filter * 100);
//		}
//
//		// Sonst...
//		else {
//
//			// Helligkeit wird um 30 % vermindert.
//			ssd.setBrightness(filter * 70);
//		}
//
//		int newKeyStates = keysDebounced.read();
//
//		// Check if first button is pressed
//		if (keys & 0x01) {
//
//			timeOut.updateTime(0);
//
//			// Check if button press duration is less than 5 seconds and new state is detected
//			if ((timeOut.secondsSinceMidnight() < 5) && (newKeyStates & 0x01)) {
//
//				// Ziffer an Anzeigestelle wird ausgeschaltet (blinken).
//				ssd = -1;
//
//				// Cycle through setting modes
//				if (newKeyStates & 0x03) {
//					settingModes = static_cast<SETTING_MODES>((settingModes + 1)
//							% 5);
//				}
//
//				// Handle the different setting modes
//				if (settingModes != NONE && !timer.timeReached()) {
//					switch (settingModes) {
//					case NONE:
//						break;
//
//					case HOUR_UNIT:
//							hours = (hours + 1) % 24;
//						break;
//
//					case HOUR_TENS:
//							hours = (hours + 10) % 24;
//						break;
//
//					case MINUTE_UNIT:
//							minutes = (minutes + 1) % 60;
//						break;
//
//					case MINUTE_TENS:
//							minutes = (minutes + 10) % 60;
//						break;
//					}
//				}
//			}
//		}
//
//		// Die Zeit wird auf SSD dargestellt.
//		ssd[3] = hours / 10; // Zehnerstelle der Stunden
//		ssd[2] = hours % 10; // Einerstelle der Stunden
//		ssd[1] = minutes / 10; // Zehnerstelle der Minuten
//		ssd[0] = minutes % 10; // Einerstelle der Minuten
//	}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {
	// Falls notwendig, ergänzen Sie hier Aufrufe, um Konfigurationen
	// an globalen Objekten vorzunehmen, die nicht durch die Angabe von
	// Argumenten beim Aufruf des Konstruktors vorgenommen werden können.
	keys.mode(PullDown);

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
//	task1();
	task2();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
	while (true) {
	}
}

