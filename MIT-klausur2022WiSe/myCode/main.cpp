#include "../../mit048/mbed-os/mbed.h"
#include "breadboard.h"
#include "CPatternController.h"
#include "CRotateApp1.h"
#include "CRotateApp2.h"
#include "CRotateApp3.h"

/**
 * Ein Eingangsbus, an den die Taster des Breadboards
 * angschlossen sind.
 */
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3,
		BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);

/**
 * Ein Ausgangsbus an den 6 LEDs des Breadboards angeschlossen sind.
 */
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3,
		BB_LED_4, BB_LED_5);

CPatternController pc(&leds, 6);

void task1() {
	CRotateApp1 app(&pc, &keys);
	app.run();
}
void task2() {
	CRotateApp2 app(&pc);
	app.run();
}
void task3() {
	CRotateApp3 app(&pc);
	app.run();
}

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
//	task2();
	task3();

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

