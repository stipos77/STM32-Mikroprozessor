/*
 * CExponentialFilter.cpp
 *
 *  Created on: May 29, 2024
 *      Author: Steven Atmodjo
 */

#include "CExponentialFilter.h"

CExponentialFilter::CExponentialFilter(float factor) {
    // Das Faktor wird mit Eingabe zugewiesen.
    m_factor = factor;

    // Der letzte Wert wird mit 0 initialisiert.
    m_last = 0;
}

CExponentialFilter& CExponentialFilter::operator<<(float value) {

    /*
     *  Der geglaettete Wert ergibt sich aus dem alten geglaetteten Wert plus
     *  der Differenz zwischen Eingabe und dem alten Wert multipliziert
     *  mit dem Faktor.
     */
    m_last = m_last+(value - m_last)*m_factor;

    // Ein Referenz auf das aktualisierte Objekt wird zurueckgegeben.
    return *this;
}

CExponentialFilter::operator float() const {

    // Den aktuellen gefilterten Wert wird zurueckgegeben.
    return m_last;
}

