/*
 * AccessUtil.h
 *
 *  Created on: 14.01.2021
 *      Author: mnl
 */

#ifndef GPIOUTIL_H_
#define GPIOUTIL_H_

#include <mbed.h>

/**
 * Diese Funktion umgeht das Information Hiding von DigitalOut und liefert
 * in den Rückgabe-Parametern einen Zeiger auf das verwendete Port (Zeiger
 * auf CMSIS GPIO_TypeDef) und die Bitmaske für den verwendeten Pin des
 * Ports.
 *
 * @param digitalOut der Ausgabeanschluss wie von mbed os modelliert
 * @param gpioTypeDef ein Zeiger auf die CMSIS-Modellierung der Port-Register
 * @param mask eine Bitmaske mit genau einer 1 für den verwendeten Ausgang
 */
inline void gpioPortInfo(DigitalOut& digitalOut,
		GPIO_TypeDef*& gpioTypeDef, uint32_t& mask) {

	class Accessor : public DigitalOut {
	public:
		Accessor(DigitalOut& digitalOut): DigitalOut{digitalOut} {
		}
		gpio_t getGpio() {
			return gpio;
		}
	};

	Accessor accessor(digitalOut);
	gpioTypeDef = accessor.getGpio().gpio;
	mask = accessor.getGpio().mask;
}

#endif /* GPIOUTIL_H_ */
