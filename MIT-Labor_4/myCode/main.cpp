#include "mbed.h"
#include "main.h"
#include "breadboard.h"
#include "CExponentialFilter.h"
#include "CPwmGenerator1.h"
#include "CPwmGenerator2.h"
#include "CPwmGenerator3.h"
#include "CPwmGenerator4.h"
#include "CSevenSegmentDisplay.h"
#include "CShiftRegisterOutputExtender.h"
#include "CPolledTimer.h"

// Definition eines globalen BusIn-Objekts.
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5, BB_BTN_6,
BB_BTN_7);

// Definition eines globalen AnalogIn-Objekts.
AnalogIn poti(BB_POTI, 3.3f);

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

/**
 * Funktion, die der Timer 3 IRQ Handler aufrufen soll. Ermöglicht
 * die Konfiguration unterschiedlicher effektiver IRQ Handler je
 * nach Nutzung des Timers.
 *
 * Die aufzurufende Funktion muss im Rahmen der Konfiguration des
 * Interrupts zugewiesen werden.
 */
function<void()> activeTIM3_IRQHandler;

/**
 * Interrupt-Handler fuer Timer 3. Der Handler ruft die Funktion auf,
 * die der globalen Variablen activeTIM3_IRQHandler zugewiesen wurde.
 */
extern "C" void TIM3_IRQHandler() {
	activeTIM3_IRQHandler();
}

/**
 * Realisiert PWM1 auf PC6 und stellt die Bits 15-10 des Zaehlerstands (CNT)
 * auf den LEDs dar.
 */
void task1() {
	// Definition eines BusOut-Objekts, um Zugriff auf LED0..LED5 zu erhalten.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	// Definition eines Objekts von Klasse CPwmGenerator1 (PWM1).
	CPwmGenerator1 fixedGenerator;

	while (true) {
		// Bits 15-10 von CNT werden auf LEDs 5-0 angezeigt.
		leds = (TIM3->CNT >> 10) & 0x3f;
	}
}

/*
 * Realisiert PWM2 und stellt die Bits vom CNT (10..15) auf den LEDs (0..5) dar.
 * Das PWM Signal wird auf Pin PC6 (LED6) entsprechend des Tastgrads (das
 * mittels ADC angesteuert ist) dargestellt.
 */
void task2() {
	// Definition eines BusOut-Objekts, um Zugriff auf LED0..LED5 zu erhalten.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	// Definition eines Objekts von Klasse CPwmGenerator2 (PWM2).
	CPwmGenerator2 fixedGenerator;

	// Erzeugt einen exponentiellen Filter mit einem Glaettungsfaktor von 0.03.
	CExponentialFilter filter(0.03);

	while (true) {
		// Messwert vom ADC wird als float in den Filter hineingefuegt (0...1).
		filter << poti.read();

		// Tastgrad vom PWM2 wird entsprechend geglaettete Wert gesteuert.
		fixedGenerator.setDutyCycle(filter * 100);

		// Bits 10...15 von CNT werden auf LEDs 0..5 angezeigt.
		leds = (TIM3->CNT >> 10) & 0x3f;
	}
}

/*
 * Realisiert PWM3 und stellt die Bits vom CNT (10..15) auf den LEDs (0..5) dar.
 * Glaettet den Potentiometerwert mit einem exponentiellen Filter und setzt das
 * Tastgrad des PWM-Generators dementsprechend. Stellt das PWM Signal auf Pin
 * PC6 (LED6) dar.
 */
void task3() {
	// Definition eines BusOut-Objekts, um Zugriff an LED0..LED5 zu erhalten.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	// PWM mit Periodendauer von 20 ms (20000 Mikrosekunde) wird definiert.
	CPwmGenerator3 fixedGenerator(20000);

	// Erzeugt einen exponentiellen Filter mit einem Glaettungsfaktor von 0.03
	CExponentialFilter filter(0.03);

	while (true) {
		// Messwert vom ADC wird als Prozent in den Filter hineingefuegt.
		filter << poti.read();

		// Tastgrad vom PWM2 wird entsprechend geglaettete Wert gesteuert.
		fixedGenerator.setDutyCycle(filter * 100);

		// Bits 10...15 von CNT werden auf LEDs 0..5 angezeigt.
		leds = (TIM3->CNT >> 10) & 0x3f;
	}
}

/*
 * Realisiert PWM4 und stellt die Bits vom CNT (10..15) auf den LEDs (0..5) dar.
 * Glaettet den Potentiometerwert mit einem exponentiellen Filter und setzt das
 * Tastgrad des PWM-Generators dementsprechend. Stellt das PWM Signal auf Pin
 * PC6 und PC 7 dar. Das Toggeln von Pin PC7 des PWM Generators erfolgt durch
 * Interrupt-Behandlung in CPwmGenerator4.
 */
void task4() {
	// Definition eines BusOut-Objekts, um Zugriff an LED0..LED5 zu erhalten.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	// Anzusteurnde LED (LED7) wird zu einer Variable zugewiesen.
	DigitalOut led7 = BB_LED_7;

	// PWM mit Periodendauer von 20 ms (20000 Mikrosekunde) wird definiert.
	CPwmGenerator4 fixedGenerator(20000);

	// Erzeugt einen exponentiellen Filter mit einem Glaettungsfaktor von 0.03.
	CExponentialFilter filter(0.03);

	// Helligkeit von LED7 (PC7) wird mittels Tastgrad angestuert.
	fixedGenerator.setPwmOut(&led7);

	while (true) {
		// Messwert vom ADC wird als Prozent in den Filter hineingefuegt.
		filter << poti.read();

		// Tastgrad vom PWM2 wird entsprechend Wert vom ADC gesteuert.
		fixedGenerator.setDutyCycle(filter * 100);

		// Bits 10...15 von CNT werden auf LEDs 0..5 angezeigt.
		leds = (TIM3->CNT >> 10) & 0x3f;
	}
}

/**
 *  Zeigt die 4 letzten Ziffern meiner Matrikelnummer auf dem SSD an und steuert
 *  die Helligkeit von LED0. Wegen des Zeitmultiplexverfahrens (der mittels
 *  TIM10 durchgefuehrt wird) leuchtet das SSD flackerfrei. Da der Super Loop
 *  leer ist (CPU "aktiv nichts tut", wird waehrend der SSD- Darstellung Energie
 *  mittels Assembler-Befehl "Wait For Interrupt" [__WFI()] gespart
 * (Stromverbrauch verringert).
 */
void task5() {
	// Definition eines BusOut-Objekts (zur Ansteuerung der Helligkeit).
	BusOut led0(BB_LED_0);

	// Definition eines Schieberegister-Objekts.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Die letzte 4 Ziffern meiner Matrikelnummer wird ins SSD hineingefuegt.
	ssd = 2550;

	// Helligkeit von 33% wird eingestellt.
	ssd.setBrightness(33);

	while (true) {
		// Ausfuehrung von Interrupt Handlern verhindern und LED0 ausschalten.
		__disable_irq();
		led0 = 0;

		// Auf einen Interrupt warten. Wenn er auftritt wird die CPU wieder
		// aktiviert, der Interrupt Handler wird aber noch nicht ausgefuehrt.
		// Der Interrupt "steht aus" ("is pending").
		__WFI();

		// LED0 einschalten und die Ausfuehrung von Interrupt Handlern wieder
		// zulassen. Der Handler wird damit sofort nach __enable_irq()
		// aufgerufen.
		led0 = 1;
		__enable_irq();
	}
}

/**
 *  Zeigt die 4 letzten Ziffern einer Matrikelnummer  auf dem SSD an. Wegen des
 *  Zeitmultiplexverfahrens (wird mittels TIM10 durchgefuehrt) leuchtet das SSD
 *  flackerfrei. Da der Super Loop leer ist (CPU "aktiv nichts tut"), wird
 *  waehrend der SSD- Darstellung Energie mittels Assembler-Befehl "Wait For
 *  Interrupt" [__WFI()] gespart.
 */
void task6() {
	// Definition eines Schieberegister-Objekts.
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);

	// Definition eines CSevenSegmentDisplay-Objekts.
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	// Timer mit 50 ms Wartezeit wird initialisiert.
	CPolledTimer timer(50);

	// Die letzte 4 Ziffern meiner Matrikelnummer wird ins SSD hineingefuegt.
	ssd = 2250;

	// Helligkeit von 33% wird eingestellt.
	ssd.setBrightness(33);

	while (true) {

		// Pruft die Helligkeit und fuehrt entsprechender Aktion aus.
		switch (ssd.getBrightness()) {

		// Falls Helligkeit beim 100 Prozent liegt...
		case 100:

			// Solange Taster 7 (ganz links ) gedrueckt wuerde...
			while (keys & 0x80) {

				// Falls timer (50ms) erreicht wuerde...
				if (timer.timeReached()) {

					// wird der Helligkeit um 1 subtrahiert.
					ssd.setBrightness(ssd.getBrightness() - 1);
				}
			}
			break;

			//Sonst...
		default:

			// Solange Taster 7 (ganz links )gedrueckt wuerde...
			while (keys & 0x80) {

				// Falls timer (50ms) erreicht wurde...
				if (timer.timeReached()) {

					// wird der Helligkeit um 1 subtrahiert.
					ssd.setBrightness(ssd.getBrightness() + 1);
				}

			}
			break;
		}
	}
}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgefuehrt.
 */
int main() {
	// Taster werden mit mode Pull-Down konfiguriert.
	keys.mode(PullDown);

// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
// implementiert.
//	task1();
//  task2();
//	task3();
//	task4();
//	task5();
	task6();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgefuehrt. Fuer den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
	while (true) {
	}
}

