#pragma once

namespace zetscript{

	class zs_datetime{
	public:
		//enum weekday { sunday=0, monday, tuesday, wednesday, thursday, friday, saturday };
		enum period { undefined=0, AM, PM };

		zs_datetime();
		zs_datetime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, period day_period = period::undefined);
		zs_datetime(const zs_datetime& other); //Copy constructor
		zs_datetime& operator=(const zs_datetime& other); //Copy assignment
		zs_datetime(zs_datetime&& other) noexcept; //Move constructor
		zs_datetime& operator=(zs_datetime&& other) noexcept; //Move assignement
		virtual ~zs_datetime();
		friend std::ostream& operator<<(std::ostream &os, const zs_datetime &dt);
		friend bool operator<(const zs_datetime &mdt, const zs_datetime &odt);
		friend bool operator>(const zs_datetime &mdt, const zs_datetime &odt);
		friend bool operator<=(const zs_datetime &mdt, const zs_datetime &odt);
		friend bool operator>=(const zs_datetime &mdt, const zs_datetime &odt);
		friend bool operator==(const zs_datetime& mdt, const zs_datetime &odt);
		friend bool operator!=(const zs_datetime& mdt, const zs_datetime &odt);
		friend zs_timespan operator-(const zs_datetime& mdt, const zs_datetime &odt);
		std::string to_string() const;
		std::string to_string(const std::string& format) const;
		std::string to_shortdate_string() const;
		int get_year() const;
		int get_month() const;
		int get_day() const;
		int get_hour() const;
		int get_minute() const;
		int get_second() const;
		int get_week_day() const;
		int get_month_day() const;
		int get_year_day() const;
		void add_years(int nb_years);
		void add_months(int nb_months);
		void add_days(int nb_days);
		void add_hours(int nb_hours);
		void add_minutes(int nb_minutes);
		void add_seconds(int nb_seconds);
		bool is_leapyear();
		static zs_datetime parse(const std::string& format, const std::string& value);
		static bool is_leapyear(int year);
	protected:
		const int ONE_DAY = 86400; //24 hours * 60 mins * 60 secs
		const int ONE_HOUR = 3600; //60 mins * 60 secs
		const int ONE_MINUTE = 60; // 60 secs
		struct tm *timeInfo = nullptr;
		bool auto_created = true;
		bool _is_leapyear(int year) const;
		static int _parse_intvalue(const std::string &pattern, int index, size_t mask_length, const std::string &parse_str);
		void _copy_from(const tm* otm);
	};
}

