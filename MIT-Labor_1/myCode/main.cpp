/*
 * Abgegeben von: Steven Atmodjo
 * Mat. Nr. 772550
 *
 */

#include "mbed.h"

#include "breadboard.h"
#include "CPolledTimer.h"
#include "CDebouncer.h"
#include <cmath>

// Initialization of Object "BusOut" to gain access of peripheral LED components
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5,
		BB_LED_6, BB_LED_7);

// Initialization of Object "keys" to gain access of peripheral keys components
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5,
		BB_BTN_6, BB_BTN_7);

// task1: lights up all LEDs (0xFF) and turns them off (0x00) every 100 ms
void task1() {
	while (true) {
		//turning on LEDs
		leds = 0xFF;
		//time delay of 100ms  (LEDs remain turned on)
		thread_sleep_for(100);
		//turning off LEDs
		leds = 0x00;
		//time delay of 100ms (LEDs remain turned off)
		thread_sleep_for(100);
	}
}

/**
 *  task 2: this function turns on a corresponding LED as long as a button is
 *  pressed.
 */
void task2() {
	while (true) {
		// binary worth of leds is the same as binary worth of the buttons
		leds = keys.read();
	}
}

//task 3: this function toggles the leds according to the press of the keys
void task3() {
	//assigning current key-state to a variable
	uint8_t previousKeyStates = keys.read();
	while (true) {
		//assigning new key-state to a variable
		uint8_t newKeyStates = keys.read();
		// an LED is toggled only when its corresponding key has been pressed
		leds = leds ^ (newKeyStates & (~previousKeyStates));
		//assigning current newKeyStates to previousKeyStates for further
		//use of the program
		previousKeyStates = newKeyStates;
	}
}

/**
 * task 4: this function toggles the leds exactly like task 3, but with a
 * debouncer which delays the reading of keys
 */
void task4() {
	//creating an object newDebouncer of  type CDebouncer
	CDebouncer newDebouncer(&keys, 250);
	//assigning current key-state (read using debouncer) to a variable
	uint8_t previousKeyStates = newDebouncer.read();
	while (true) {
		//reading key inputs using the int() operator of class CDebouncer
		uint8_t newKeyStates = newDebouncer;
		leds = leds ^ (newKeyStates & ~previousKeyStates);
		previousKeyStates = newKeyStates;
	}
}

/**
 * task5: this function shifts the LED light to the left every 1000 ms using
 * thread_sleep_for to replicate running lights (LED0, then LED1, and so on)
 */
void task5() {
	//initializing the leds (first light is at furthest right side)
	leds = 0x1;
	while (true) {
		//time delay of 1000ms
		thread_sleep_for(1000);
		//shifts LED to the left by 1
		leds = leds << 1;
		//has the LED been shifted to the furthest right side?
		if (leds == 0) {
			//LED begins from furthest left side again
			leds = 0x1;
		}
	}
}

//task6: this function shifts the LED light to the right every 1000 ms using a
//self made timer
void task6() {
	//amount of time delay (1000 ms)
	int64_t waitTime = 1000;
	//targetTime is equal to waitTime + current time
	int64_t targetTime = waitTime + std::chrono::duration_cast
			< std::chrono::milliseconds
			> (HighResClock::now().time_since_epoch()).count();
	//initializing leds (first light is at furthest right side)
	leds = 0x01;
	while (true) {
		//If current time is later than targetTime
		if (std::chrono::duration_cast < std::chrono::milliseconds
				> (HighResClock::now().time_since_epoch()).count()
				>= targetTime) {
			//LED is shifted once to the left
			leds = leds << 1;
			//targetTime is updated
			targetTime += waitTime;
			//if LED has been shifted to furthest right side
			if (leds == 0) {
				//LED is set to furthest left side again
				leds = 0x01;
			}
		}
	}
}

/**
 * task7: this function shows different frequencies through the blinking of leds
 * by creating objects (timers) of class type CPolledTimer
 */
void task7() {
	//represents 2 Hz (1 s/ 500 ms = 2 Hz)
	CPolledTimer twoHz(500);
	//represents 3 Hz (1 s/ 333 ms = 3 Hz)
	CPolledTimer threeHz(333);
	//represents 4 Hz (1 s/ 250 ms = 4 Hz)
	CPolledTimer fourHz(250);
	while (true) {
		//If two Hz has been reached
		if (twoHz.timeReached()) {
			//furthest left LED blinks
			leds = leds ^ (1 << 0);
		}
		//If three Hz has been reached
		if (threeHz.timeReached()) {
			//LED shifted by 1 to the right blinks
			leds = leds ^ (1 << 1);
		}
		//If four Hz has been reached
		if (fourHz.timeReached()) {
			//LED shifted by 2 to the right blinks
			leds = leds ^ (1 << 2);
		}
	}
}


/**
 * task8: this function performs the same task as task6 using a timer made by
 * creating an object (timer) of class type CPolledTimer
 */
void task8() {

	//creating an object "chasingLightTimer" with a time delay of 1000ms
	CPolledTimer chasingLightTimer(1000);
	//initializing leds (first light is at furthest right side)
	leds = 0x01;
	while (true) {
		//If 1000ms has passed (timer has been reached)
		if (chasingLightTimer.timeReached()) {
			//LED is shifted by 1 to the right
			leds = leds << 1;
			//If LED has been shifted to the furthest right side
			if (leds == 0) {
				//LED is set to furthest left side
				leds = 0x01;
			}
		}
	}
}

/**
 * task 9: This function creates configureable running lights.
 * [LED0...LED3] represents the speed/direction in binary
 * [LED4...LED7] represents the running lights with adjustable Frequencies which
 * is determined by the control variable.
 * The LEDs move to the left when the control variable negative is, and will
 * move to the opposite direction when it is positive,
 * [Key 1...Key 3] functions as direct setting for the control variable
 * [Key 4] is a "+" Key, which increments the control variable
 * [Key 5] is a "-" Key, which decreases the control variable
 * [Key 6] is a "reverse" -Key , which changes the direction of the light path
 */
void task9() {
	//variable to control the light, which can take the value of -7..7 ~ 0..15
	int8_t lightVariable = 0;
	//absolute value which represents the speed of light
	uint8_t lightSpeed = 0;
	//variable used to shift bits along the BusOut Object
	uint8_t shift = 0;
	//represents the direction of the chasing light
	bool toTheLeft = false;
	//timer with time delay of 1000 (for shifting the lights)
	CPolledTimer chasingLightTimer(1000);
	//timer with time delay of 80ms (for reading keys)
	CDebouncer myDebouncer(&keys, 80);
	uint8_t newKeyStates = 0;
	uint8_t previousKeyStates = myDebouncer.read();
	while (true) {
		//Is lightSpeed 0?
		if (lightSpeed == 0) {
			//none of the lights run, only the light Variable is displayed
			leds.write(lightVariable & 0x0F);
		}
		//If not...
		else {
			/**
			 * The 4 furthest left leds will run (according to "shift". The 4
			 * furthest right LEDs represent the current lightVariable (furthest
			 * right is LSB). They are both controlled with 0x0F and 0xF0
			 */
			leds.write(((1 << (shift +4))& 0xF0) |  (lightVariable & 0x0F));
			/**
			 * The code below checks the direction of the light path and updates
			 * updates the shift variable everytime the timer from CPolledTimer
			 * is reached
			 */
			//If the timer has been reached...
			if (chasingLightTimer.timeReached()) {
				//If the direction of light is to the left..
				if (toTheLeft) {
					//If the light path has been shifted to the furthest left
					if (shift >= 3) {
						//"shift" is set to 0 (lights start again from 0x10)
						shift = 0;
					}
					//If not ...
					else {
						//"shift" variable is added by 1
						shift++;
					}
				}
				//If the direction of light is to the right...
				else {
					//If the light path has been shifted to the furthest right
					if (shift == 0) {
						//"shift" is set to 3 (lights start again from 0x80
						shift = 3;
					}
					else {
						//"shift" variable is substracted by 1
						shift--;
					}
				}
			}
		}
		/**
		 * The code below controls the buttons. Every time a key is pressed,
		 * the code checks which task corresponds to the key pressed and
		 * executes it. At the end, lightSpeed and lightVariable are updated
		 */
		newKeyStates = myDebouncer.read();
		//If any of the keys have been pressed
		if (newKeyStates != previousKeyStates) {
			previousKeyStates = newKeyStates;
			// If one of the direct-config-buttons is pressed
			if (newKeyStates & 0x0F) {
				//lightVariable is updated (toggled)
				lightVariable ^= newKeyStates;
				lightVariable &= 0x0F;
			}
			//If the "+" button has been pressed
			if (newKeyStates & 0x10) {
				//If lightVariable hasn't reached the Max Worth (7)
				if (lightVariable<7||lightVariable>=9) {
					//lightVariable is increased
					lightVariable++;
					lightVariable &= 0x0F;
				}
			}
			//If the "-" button has been pressed
			if (newKeyStates & 0x20) {
				//If lightVariable hasn't reached the Min Worth (-7 ~ 15)
				if ((lightVariable<=7)||
						(lightVariable>9 && lightVariable<=15)) {
					//lightVariable is decreased
					lightVariable--;
					lightVariable &= 0x0F;
				}
			}
			//checks if the "reverse" - button has been pressed
			if (newKeyStates & 0x40) {
				//If lightVariable is negative (has a "MSB" of 1)
				if (lightVariable & 0x08) {
					//lightVariable is negated and added with 1 to be converted
					//to positive. (2's Complement)
					lightVariable = (~lightVariable + 1) & 0x0F;
				}
				//If lightVariable is positive (has an "Most Significant Bit"
				//[MSB] of 0)
				else if (lightVariable & 0x07) {
					//lightVariable is first subtracted by 1 and then negated to
					//be bitwise converted to negative
					lightVariable = ~(lightVariable - 1) & 0x0F;
				}
			}
			/**
			 * the following code calculates the absolute speed from the control
			 * variable, determines the direction and updates the
			 * chasingLightTimer with the updateWaitTime method
			 */
			//If lightVariable is negative
			if (lightVariable & 0x08) {
				//light path moves to the left
				toTheLeft = true;
				//If lightVariable takes the value of 0b1000
				if (lightVariable == 0x08) {
					//lightSpeed is set to 0
					lightSpeed = 0;
				}
				else {
					// negating the lightVariable and adding 1, also assigns the
					// result to lightSpeed
					lightSpeed = (~lightVariable & 0x0F) + 1;
				}
			}
			//If lightVariable isn't positive....
			else {
				//assigning lightVariable to lightSpeed
				lightSpeed = lightVariable & 0x0F;
				//light path is moving to the right
				toTheLeft = false;
			}
			//updates timer if a button has been pressed
			chasingLightTimer.updateWaitTime(abs(lightSpeed) * 250);
		}
	}
}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	keys.mode(PullDown); //Objekt keys Initialisieren
	//task1();
	//task2();
	//task3();
//	task4();
//	task5();
//	task6();
//	task7();
//	task8();
	task9();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2(),
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
	while (true) {
	}
}
