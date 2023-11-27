/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript
{
	class TimeSpan
	{
	public:
		TimeSpan();
		explicit TimeSpan(int days, int hours = 0, int minutes = 0, int seconds = 0);
		int getDays() const;
		int getHours() const;
		int getMinutes() const;
		int getSeconds() const;
		int getTotalHours() const;
		int getTotalMinutes() const;
		int getTotalSeconds() const;
		//Operators
		friend bool operator<(const TimeSpan &mts, const TimeSpan &ots);
		friend bool operator>(const TimeSpan &mts, const TimeSpan &ots);
		friend bool operator<=(const TimeSpan &mts, const TimeSpan &ots);
		friend bool operator>=(const TimeSpan &mts, const TimeSpan &ots);
		friend bool operator==(const TimeSpan &mts, const TimeSpan &ots);
		friend bool operator!=(const TimeSpan &mts, const TimeSpan &ots);
	private:
		int days;
		int hours;
		int minutes;
		int seconds;
	};
}


