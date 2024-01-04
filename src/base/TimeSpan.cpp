/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	TimeSpan::TimeSpan(){
		days=0;
		hours=0;
		minutes=0;
		seconds=0;
	}

	TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds)
		//days(days), hours(hours), minutes(minutes), seconds(seconds)
	{
		//Check if the values submitted is valid
		if (hours < -23 || hours > 23) {
			throw std::invalid_argument("hours must be between 0 and 23");
		}
		if (minutes < -59 || minutes > 59) {
			throw std::invalid_argument("minutes must be between 0 and 59");
		}
		if (seconds < -59 || seconds > 59) {
			throw std::invalid_argument("seconds must be between 0 and 59");
		}
		this->days = days;
		this->hours = hours;
		this->minutes = minutes;
		this->seconds = seconds;
	}

	int TimeSpan::getDays() const
	{
		return days;
	}

	int TimeSpan::getHours() const
	{
		return hours;
	}

	int TimeSpan::getMinutes() const
	{
		return minutes;
	}

	int TimeSpan::getSeconds() const
	{
		return seconds;
	}

	int TimeSpan::getTotalHours() const
	{
		return  (days * 24) + hours;
	}

	int TimeSpan::getTotalMinutes() const
	{
		return  (days * 60 * 24) + (hours * 60) + minutes;
	}

	int TimeSpan::getTotalSeconds() const
	{
		return  (days * 60 * 60 * 24) + (hours * 60 * 60) + (minutes * 60) + seconds;
	}

	bool operator<(const TimeSpan &mts, const TimeSpan &ots)
	{
		if (mts.days < ots.days) {
			return true;
		}
		if (mts.days > ots.days) {
			return false;
		}
		if (mts.hours < ots.hours) {
			return true;
		}
		if (mts.hours > ots.hours) {
			return false;
		}
		if (mts.minutes < ots.minutes) {
			return true;
		}
		if (mts.minutes > ots.minutes) {
			return false;
		}
		if (mts.seconds < ots.seconds) {
			return true;
		}

        return false;
	}

	bool operator>(const TimeSpan &mts, const TimeSpan &ots)
	{
		return ots < mts;
	}

	bool operator<=(const TimeSpan &mts, const TimeSpan &ots)
	{
		return !(mts > ots);
	}

	bool operator>=(const TimeSpan &mts, const TimeSpan &ots)
	{
		return !(mts < ots);
	}

	bool operator==(const TimeSpan &mts, const TimeSpan &ots)
	{
		return mts.days == ots.days &&
			mts.hours == ots.hours &&
			mts.minutes == ots.minutes &&
			mts.seconds == ots.seconds;
	}

	bool operator!=(const TimeSpan &mts, const TimeSpan &ots)
	{
		return !(mts == ots);
	}
}
