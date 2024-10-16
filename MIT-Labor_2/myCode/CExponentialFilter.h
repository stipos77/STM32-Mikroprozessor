/*
 * CExponentialFilter.h
 *
 *  Created on: May 29, 2024
 *      Author: Steven Atmodjo
 */

#ifndef CEXPONENTIALFILTER_H_
#define CEXPONENTIALFILTER_H_

/**
 * Modelliert ein Exponentialfilter zur exponentiellen Glaettung von Werten.
 * Eine exponentielle Gewichtsfunktion wird an dieser Filter angewendet, damit
 * werden Rauschen und Schwankungen auf der Messleitung reduziert.
 */
class CExponentialFilter {
private:
	// Deklaration eines Glaettungsfaktors
	float m_factor;

	// Deklaration Index letzter geglaettete Wert
	float m_last;
public:
	/**
	 * Erzeugt ein Exponentialfilter mit Parametereingabe als Faktor.
	 *
	 * @param factor Der Glättungsfaktor, der bei der Berechnung des
	 * Exponentialfilters verwendet wird.
	 */
	CExponentialFilter(float factor);

	/**
	 * Ueberladener "<<" - Operator fuer Eingabe von neuen Werten in den Filter
	 * und aktualisiert den geglaetteten Wert.
	 *
	 * @param value hineinzufuegende Wert.
	 * @return Referenz auf das aktualisierte CExponentialFilter-Objekts.
	 */
	CExponentialFilter& operator<<(float value);

	/**
	 * Ueberladener Cast-Operator fuer typecasting von Variablen als float.
	 * Stellt die aktuelle gefilterten Wert dar.
	 *
	 * @return aktuelle gefilterte Wert.
	 */
	operator float() const;
};

#endif /* CEXPONENTIALFILTER_H_ */
