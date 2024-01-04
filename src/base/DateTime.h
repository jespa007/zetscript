/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class DateTime{
	public:
		//enum weekday { sunday=0, monday, tuesday, wednesday, thursday, friday, saturday };
		enum period { undefined=0, AM, PM };

		DateTime();
		DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, period day_period = period::undefined);
		DateTime(const DateTime& other); //Copy constructor
		DateTime& operator=(const DateTime& other); //Copy assignment
		DateTime(DateTime&& other) noexcept; //Move constructor
		DateTime& operator=(DateTime&& other) noexcept; //Move assignement
		virtual ~DateTime();

		void setUtc();
		friend std::ostream& operator<<(std::ostream &os, const DateTime &dt);
		friend bool operator<(const DateTime &mdt, const DateTime &odt);
		friend bool operator>(const DateTime &mdt, const DateTime &odt);
		friend bool operator<=(const DateTime &mdt, const DateTime &odt);
		friend bool operator>=(const DateTime &mdt, const DateTime &odt);
		friend bool operator==(const DateTime& mdt, const DateTime &odt);
		friend bool operator!=(const DateTime& mdt, const DateTime &odt);
		friend TimeSpan operator-(const DateTime& mdt, const DateTime &odt);
		String toString() const;
		String toString(const String& format) const;
		String to_shortdate_string() const;
		int getYear() const;
		int getMonth() const;
		int getDay() const;
		int getHour() const;
		int getMinute() const;
		int getSecond() const;
		int getWeekDay() const;
		int getMonthDay() const;
		int getYearDay() const;
		void addYears(int nb_years);
		void addMonths(int nb_months);
		void addDays(int nb_days);
		void addHours(int nb_hours);
		void addMinutes(int nb_minutes);
		void addSeconds(int nb_seconds);
		bool isLeapYear();
		static DateTime parse(const String& format, const String& value);
		static bool isLeapYear(int year);
	protected:
		const int ONE_DAY = 86400; //24 hours * 60 mins * 60 secs
		const int ONE_HOUR = 3600; //60 mins * 60 secs
		const int ONE_MINUTE = 60; // 60 secs
		struct tm *time_info = nullptr;
		bool auto_created = true;
		bool _isLeapYear(int year) const;
		static int _parseIntValue(const String &pattern, int index, size_t mask_length, const String &parse_str);
		void _copyFrom(const tm* otm);
	};
}

