/*
 * CRtc.h
 *
 *  Created on: 15.01.2021
 *      Author: mnl
 */

#ifndef CRTC_H_
#define CRTC_H_

/**
 * Provides easy access to the Real Time Clock.
 */
class CRtc {
public:
	CRtc();

private:
	/**
	 * Enable power management and allow access to backup domain.
	 * Called once in constructor. (POLLLLLLLLLLLLLLLL)
	 */
	void enableRtcAccess();

	/**
	 * Select LSE as clock source. Called once in constructor.  (POLLLLLLLLLLLLLLLL)
	 */
	void selectClockSource();

	/**
	 * Enable modification of RTC registers, stop counting.
	 */
	void enterInitializationMode();

	/**
	 * Disable modification of RTC registers, resume counting.
	 */
	void leaveInitializationMode();

public:
	/**
	 * Returns the time as number of seconds since midnight.
	 */
	uint32_t secondsSinceMidnight();

	/**
	 * Sets the time.
	 *
	 * @param secondsSinceMidnight the time specified as seconds since 00:00
	 */
	void updateTime(uint32_t secondsSinceMidnight);
};

#endif /* CRTC_H_ */
