/*
 * CMovingAverageFilter.h
 *
 *  Created on: May 28, 2024
 *      Author: Steven Atmodjo
 */

#ifndef CMOVINGAVERAGEFILTER_H_
#define CMOVINGAVERAGEFILTER_H_
#include <stdint.h>

/**
 * Modelliert ein Filter zur Glaettung des Mittelwerts ueber N Werte. Dieser
 * Filter berechnet den Durchschnitt einer bestimmten Anzahl
 * aufeinanderfolgender Werte, damit werden Rauschen und Schwankungen auf der
 * Messleitung reduziert.
 */
class CMovingAverageFilter {
private:
	// Der Index des naechsten Werts im Array.
	uint16_t m_nextIndex;

	// Die Summe der Werte im Array.
	uint32_t m_sum;

	// Die Zeigervariable, der Platz reserviert, um die Adresse eines Arrays von
	// uint16_t-Variablen zu speichern.
	uint16_t* m_values;

	// Groesse des dynamischen Arrays.
	uint16_t m_valuesSize;
public:

	/**
	 * Erstellt eine Instanz der Klasse CMovingAverageFilter mit der
	 * Festerngroesse "size" des gleitenden Durchschnitts.
	 *
	 * @param size Groesse des Arrays zur Speicherung der Werte.
	 */
	CMovingAverageFilter(uint16_t size=16);

	/**
	 * Ueberladener "<<" - Operator fuer Eingabe von neuen Werten in den Filter
	 * und aktualisiert den gleitenden Durchschnitt.
	 *
	 * @param value hineinzufuegende ganzzahlige Wert.
	 * @return Referenz auf das aktualisierte CMovingAverageFilter-Objekts.
	 */
	CMovingAverageFilter& operator<< (uint16_t value);

	/**
	 * Ueberladener Cast-Operator fuer Rueckgabe der aktuellen
	 * gleitenden Durchschnittswert
	 *
	 * @return aktueller gleitende Durchschnittswert.
	 */
	operator uint16_t() const;
};

#endif /* CMOVINGAVERAGEFILTER_H_ */
