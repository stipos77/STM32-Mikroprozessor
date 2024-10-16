#ifndef CREACTIONAPP_H_
#define CREACTIONAPP_H_

#include "mbed.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"

extern "C" void TIM3_IRQHandler();

/**
 * Realisiert eine Anwendung zur Messung der (menschlichen)
 * Reaktionszeit. Die Anwendung wird durch Aufruf der Methode
 * run1() oder run2() gestartet. Die detaillierte Beschreibung
 * des Verhaltens finden Sie im Kommentar der jeweiligen
 * Methode.
 */
class CReactionApp {
private:
	/**********************************************************************
	 * Es dürfen keine weiteren Attribute oder Methoden definiert werden. *
	 **********************************************************************/

	BusIn* m_keys;
	DigitalOut* m_startLed;
	BusOut* m_resultLeds;

	/* Exemplar der Anwendung. Wird nur für run2() benötigt. */
	static CReactionApp* m_instance;
	/* Seit Start der Zeitmessung vergangene Zeit, darf nur
	 * in run2() und dem IRQ-Handler verwendet werden. */
	uint64_t m_elapsedTime;

	/**
	 * Liefert die Zeit, die seit dem Starten des Programms
	 * vergangen ist, in Millisekunden.
	 */
	int64_t millisSinceStart() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				HighResClock::now().time_since_epoch()).count();
	}

	/**
	 * Wartet eine zufällige Zeit zwischen 1 und 6 Sekunden und
	 * schaltet dann die Start-LED (m_startLed) an. (9 Punkte)
	 */
	void waitAndTurnOn();

	/**
	 * Wartet bis die Taste ganz links gedrückt wird (s. m_keys)
	 * oder 3 Sekunden vergangen sind. (Andere Tasten haben keine
	 * Auswirkung.)
	 *
	 * Liefert true, wenn die Taste gedrückt wurde bzw. false,
	 * wenn die Funktion wegen des Timeout beendet wurde.
	 *
	 * (6 Punkte)
	 */
	bool waitForKeyWithTimeout();

	/**
	 * Konvertiert den übergebenen Binärwert in seine BCD-Darstellung.
	 * Die Methode funktioniert für den kompletten Wertebereich von 0
	 * bis 4294967295. (10 Punkte)
	 */
	uint64_t toBcd(uint32_t value);

	/**
	 * Zeigt die übergeben Zeitdauer auf den LEDs an
	 * (s. m_resultLeds). Angezeigt werden hundertstel
	 * Sekunden in BCD-Darstellung. Damit ist der größte
	 * darstellbare Wert 99/100. Ist der übergebene Wert
	 * größer, werden alle LEDs angeschaltet. (4 Punkte)
	 *
	 * @param timeInMs die anzuzeigende Zeit in Millisekunden (ms)
	 */
	void displayTime(uint64_t timeInMs);

	/**
	 * Konfiguriert den Timer 3 so, dass er alle Millisekunde einen
	 * Interrupt erzeugen kann (wenn er läuft). Startet den Timer
	 * (noch) nicht. (6 Punkte)
	 *
	 * (Sie brauchen hierfür keinen Taschenrechner!)
	 */
	void initTimer();

	/**
	 * Schaltet die Interrupt-Erzeugung bei Überlauf des Timer 3
	 * ein und startet den Timer 3. (2 Punkte)
	 */
	void startTimer();

	/**
	 * Stoppt den Timer 3, schaltet die Interrupt-Erzeugung bei
	 * Überlauf aus und setzt ihn auf 0. (4 Punkte)
	 */
	void stopTimer();

	/**
	 * Der IRQ-Handler des Timer 3. Zählt die vergangene Zeit
	 * (m_elapsedTime) hoch und aktualisiert die Anzeige.
	 * (3 Punkte)
	 */
	friend void TIM3_IRQHandler();

public:
	/**
	 * Initialisiert ein neues Exemplar der App.
	 *
	 * @param keys Zeiger auf den Eingangsbus, an den
	 * die Taster angeschlossen sind
	 * @param startLed Zeiger auf einen Ausgang, an den die LED
	 * angeschlossen ist, die den Beginn der Zeitmessung signalisiert
	 * @param resultLeds Zeiger auf einen Ausgangsbus an den 8
	 * LEDs angschlossen sind
	 * (4 Punkte)
	 */
	CReactionApp(BusIn* keys, DigitalOut* startLed, BusOut* resultLeds);

	/**
	 * Die Anwendung verhält sich wie folgt:
	 *
	 * (1) Nach einer zufälligen Wartezeit zwischen 1 und 6 Sekunden
	 * wird der Ausgang "startLed" (vergl. Konstruktorparameter)
	 * auf 1 gesetzt. Damit wird die "Start-LED" angeschaltet.
	 * Es beginnt eine Zeitmessung.
	 *
	 * (2) Die Anwendung wartet darauf, dass der Benutzer die Taste
	 * ganz links im übergebenen Tastenfeld (vergl. Konstruktorparameter)
	 * drückt oder eine Wartezeit von 3 Sekunden überschritten wird.
	 *
	 * (3) Jetzt erfolgt die Ausgabe auf den 8 LEDs zur Anzeige des
	 * Ergebnisses (vergl. Konstruktorparameter). Wurde die Wartezeit
	 * in Schritt (2) überschritten oder ist das Ergebnis größer als
	 * 0,99 s, werden alle LEDs angeschaltet. Andernfalls wird die
	 * Zeit in hundertstel Sekunden in BCD-Darstellung angezeigt.
	 *
	 * (4) Nachdem das Ergbnis für 5 Sekunden angezeigt wurde,
	 * werden der Ausgang "startLed" und die Ergebnisanzeige wieder
	 * auf 0 gesetzt und der Ablauf beginnt wieder mit Schritt (1).
	 *
	 * (9 Punkte)
	 */
	void run1();

	/**
	 * Die Anwendung verhält sich grundsätzlich wie bei Aufruf der
	 * Methode run1(). Allerdings wird beim Starten der Zeitmessung
	 * nicht nur die Start-LED eingeschaltet, sondern es beginnt
	 * sofort zusätzlich die Anzeige der vergangenen Zeit auf den
	 * Ergebnis-LEDs (hundertstel Sekunden in BCD-Darstellung seit
	 * Anschalten der Start-LED). (3 Punkte)
	 */
	void run2();
};

#endif /* CROTATEAPP1_H_ */
