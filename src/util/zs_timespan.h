/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript
{
	class zs_timespan
	{
	public:
		explicit zs_timespan(int days, int hours = 0, int minutes = 0, int seconds = 0);
		int get_days() const;
		int get_hours() const;
		int get_minutes() const;
		int get_seconds() const;
		int get_total_hours() const;
		int get_total_minutes() const;
		int get_total_seconds() const;
		//Operators
		friend bool operator<(const zs_timespan &mts, const zs_timespan &ots);
		friend bool operator>(const zs_timespan &mts, const zs_timespan &ots);
		friend bool operator<=(const zs_timespan &mts, const zs_timespan &ots);
		friend bool operator>=(const zs_timespan &mts, const zs_timespan &ots);
		friend bool operator==(const zs_timespan &mts, const zs_timespan &ots);
		friend bool operator!=(const zs_timespan &mts, const zs_timespan &ots);
	private:
		int days;
		int hours;
		int minutes;
		int seconds;
	};
}


