/*
 * CMovingAverageFilter.cpp
 *
 *  Created on: May 28, 2024
 *      Author: Steven Atmodjo
 */

#include "CMovingAverageFilter.h"

CMovingAverageFilter::CMovingAverageFilter(uint16_t size) {
	// Ein neues Objekt wird mit Defaultwerten und Eingabe initialisiert.
	m_nextIndex = 0;
	m_sum = 0;
	m_valuesSize=size;

	// Eine dynamische Speicher wird mit eingabeentsprechnde Groesse.
	m_values= new uint16_t [m_valuesSize];

	// Initialisierung des Arrays mit "0"-Werten.
	for(int i=0; i<m_valuesSize; i++){
		m_values[i]=0;
	}
}

CMovingAverageFilter& CMovingAverageFilter::operator<<(uint16_t value) {
	// Den Wert an der aktuellen Position des Index wird von der aktuellen
	// Summe subtrahiert. Den aeltesten Wert wird aus der Summe entfernt.
	m_sum -= m_values[m_nextIndex];

	// Den neuen Wert wird zur aktuellen Summe hinzugefuegt. Eingabe wird in die
	// Summe gespeichert.
	m_sum += value;

	// Den neuen Wert wird an dem aktuellen Index im Array zugewiesen.
	// Aktualisiert das Array mit dem neuen Wert.
	m_values[m_nextIndex] = value;

	// Der Index wird aktualisiert, um auf die nächste Position im Array
	// zuzugreifen.
	m_nextIndex = (m_nextIndex + 1) % m_valuesSize;

	// Ein Referenz auf das aktualisierte Objekt wird zurueckgegeben.
	return *this;
}

CMovingAverageFilter::operator uint16_t() const{
	// Der Mittelwert des Filters (aktuelle Summe/Groesse) wird zurueckgegeben.
	return (m_sum/m_valuesSize);
}
