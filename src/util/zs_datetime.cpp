#include "zetscript.h"

namespace zetscript{

	//Default constructor get current date and time
	zs_datetime::zs_datetime()
		: timeInfo(new tm())
	{
		time_t rawtime;
		time (&rawtime);
		struct tm *tm_now = localtime(&rawtime);
		_copy_from(tm_now);
	}

	zs_datetime::zs_datetime(int year, int month, int day, int hour, int minute, int second, period day_period)
	{
		//Check out of range limit
		if (month < 1 || month > 12) {
			THROW_RUNTIME_ERROR("month must be between 1 and 12");
		}
		if (day < 1) {
			THROW_RUNTIME_ERROR("day is out of range");
		}
		if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && day > 31) {
			THROW_RUNTIME_ERROR("day is out of range");
		}
		if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
			THROW_RUNTIME_ERROR("day is out of range");
		}
		if (month == 2 && this->_is_leapyear(year) && day > 29) {
			THROW_RUNTIME_ERROR("day is out of range");
		}
		if (month == 2 && !this->_is_leapyear(year) && day > 28) {
			THROW_RUNTIME_ERROR("day is out of range");
		}
		if (day_period == period::undefined)
		{
			if (hour < 0 || hour > 23) {
				THROW_RUNTIME_ERROR("hour must be between 0 and 23");
			}
		}
		else {
			if (day_period != period::AM && day_period != period::PM) {
				THROW_RUNTIME_ERROR("the selected period is out of range");
			}
			if (hour < 1 || hour > 12) {
				THROW_RUNTIME_ERROR("hour must be between 1 and 12");
			}
			//Adjust to 24 hour format
			if (hour == 12 && day_period == period::AM) {
				hour = 0;
			}
			else if (day_period == period::PM && hour < 12) {
				hour = hour + 12;
			}
		}
		if (minute < 0 || minute > 59) {
			THROW_RUNTIME_ERROR("minute must be between 0 and 59");
		}
		if (second < 0 || second > 59) {
			THROW_RUNTIME_ERROR("second must be between 0 and 59");
		}
		timeInfo = new tm();
		timeInfo->tm_year = year - 1900;
		timeInfo->tm_mon = month - 1;
		timeInfo->tm_mday = day;
		timeInfo->tm_hour = hour;
		timeInfo->tm_min = minute;
		timeInfo->tm_sec = second;
		timeInfo->tm_isdst = -1;
		mktime(timeInfo);
	}

	//Copy constructor
	zs_datetime::zs_datetime(const zs_datetime& other)
	{
		timeInfo = new tm();
		_copy_from(other.timeInfo);
	}

	//Copy assignment
	zs_datetime& zs_datetime::operator=(const zs_datetime& other)
	{
		if (this != &other) {
			_copy_from(other.timeInfo);
		}
		return *this;
	}

	zs_datetime::~zs_datetime()
	{
		delete timeInfo;
	}

	//Move constructor
	zs_datetime::zs_datetime(zs_datetime&& other) noexcept
		: timeInfo(other.timeInfo)
	{
		// Release the data pointer from the source object so that the destructor
		// does not free the memory multiple times.
		other.timeInfo = nullptr;
	}

	void zs_datetime::set_utc(){
		time_t rawtime;
		time (&rawtime);
		struct tm *tm_now = gmtime(&rawtime);
		_copy_from(tm_now);
	}

	//Move assignement operator
	zs_datetime& zs_datetime::operator=(zs_datetime&& other) noexcept
	{
		if (this != &other)
		{
			delete timeInfo;
			// Copy the data pointer and its length from the source object.
			timeInfo = other.timeInfo;
			// Release the data pointer from the source object so that
			// the destructor does not free the memory multiple times.
			other.timeInfo = nullptr;
		}
		return *this;
	}

	bool zs_datetime::_is_leapyear(int year) const
	{
		return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
	}

	zs_string zs_datetime::to_string() const
	{
		char retVal[128] = "";
		sprintf(static_cast<char *>(retVal), "%d-%02d-%02d %02d:%02d:%02d",
			get_year(),
			get_month(),
			get_day(),
			get_hour(),
			get_minute(),
			get_second());
		return zs_string(static_cast<char *>(retVal));

	}

	zs_string zs_datetime::to_string(const zs_string& _format) const
	{
		zs_string retVal;
		zs_string format=zs_strutils::to_lower(_format);

		if (strcmp(format.c_str(), "") == 0) {
			return to_string();
		}
		zs_string pattern_temp;
		for (unsigned int index_char = 0; index_char < format.length(); index_char++) {
			bool is_letter = false;
			//Check if the character is a valid pattern char
			if ((format[index_char] >= 'a' && format[index_char] <= 'z') ||
				(format[index_char] >= 'A' && format[index_char] <= 'Z')) {
				is_letter = true;
				if (pattern_temp.length() == 0) {
					pattern_temp += format[index_char];
				}
				//Check if the pattern has not changed
				else if (format[index_char] == pattern_temp[pattern_temp.length() - 1]) {
					pattern_temp += format[index_char];
				}
			}
			//Check if the pattern has not changed
			if (pattern_temp.length() > 0 && (format[index_char] != pattern_temp[pattern_temp.length() - 1] || index_char == format.length() - 1)) {
				//int *ptr_date_section = nullptr;
				char value_converted[20] = "";
				if (pattern_temp == "yyyy") {
					sprintf(static_cast<char *>(value_converted), "%04d", this->get_year());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "yy") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_year() % 100);
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "MM") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_month());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "M") {
					sprintf(static_cast<char *>(value_converted), "%01d", this->get_month());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "dd") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_day());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "d") {
					sprintf(static_cast<char *>(value_converted), "%01d", this->get_day());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "HH") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_hour());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "H") {
					sprintf(static_cast<char *>(value_converted), "%01d", this->get_hour());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "hh") {
					int instance_hour = this->get_hour();
					if (instance_hour == 0) {
						retVal += "12";
					}
					else if (instance_hour > 12) {
						sprintf(static_cast<char *>(value_converted), "%02d", instance_hour - 12);
						retVal += static_cast<char *>(value_converted);
					}
					else {
						sprintf(static_cast<char *>(value_converted), "%02d", instance_hour);
						retVal += static_cast<char *>(value_converted);
					}
				}
				else if (pattern_temp == "h") {
					int instance_hour = this->get_hour();
					if (instance_hour == 0) {
						retVal += "12";
					}
					else if (instance_hour > 12) {
						sprintf(static_cast<char *>(value_converted), "%01d", instance_hour - 12);
						retVal += static_cast<char *>(value_converted);
					}
					else {
						sprintf(static_cast<char *>(value_converted), "%01d", instance_hour);
						retVal += static_cast<char *>(value_converted);
					}
				}
				else if (pattern_temp == "mm") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_minute());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "m") {
					sprintf(static_cast<char *>(value_converted), "%01d", this->get_minute());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "ss") {
					sprintf(static_cast<char *>(value_converted), "%02d", this->get_second());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "s") {
					sprintf(static_cast<char *>(value_converted), "%01d", this->get_second());
					retVal += static_cast<char *>(value_converted);
				}
				else if (pattern_temp == "tt") {
					if (this->get_hour() >= 12) {
						retVal += "PM";
					}
					else {
						retVal += "AM";
					}
				}

				pattern_temp = "";
			}

			if (!is_letter) {
				retVal += format[index_char];
			}
			else if (pattern_temp.length() == 0) {
				pattern_temp += format[index_char];
			}
		}
		return zs_string(retVal);

	}

	zs_string zs_datetime::to_shortdate_string() const
	{
		char retVal[128] = "";
		sprintf(static_cast<char *>(retVal), "%d-%02d-%02d",
			get_year(),
			get_month(),
			get_day());
		return zs_string(static_cast<char *>(retVal));
	}

	int zs_datetime::get_year() const
	{
		return timeInfo->tm_year + 1900;
	}

	int zs_datetime::get_month() const
	{
		return timeInfo->tm_mon + 1;
	}

	int zs_datetime::get_day() const
	{
		return timeInfo->tm_mday;
	}

	int zs_datetime::get_hour() const
	{
		return timeInfo->tm_hour;
	}

	int zs_datetime::get_minute() const
	{
		return timeInfo->tm_min;
	}

	int zs_datetime::get_second() const
	{
		return timeInfo->tm_sec;
	}

	int zs_datetime::get_week_day() const
	{
		return timeInfo->tm_wday;
	}

	int zs_datetime::get_month_day() const
	{
		return timeInfo->tm_mday;
	}

	int zs_datetime::get_year_day() const
	{
		return timeInfo->tm_yday;
	}

	void zs_datetime::add_years(int nb_years)
	{
		timeInfo->tm_year += nb_years;
	}

	void zs_datetime::add_months(int nb_months)
	{
		//Get number of year
		auto nb_year = static_cast<int>(ceil(nb_months / 12));
		int nb_months_final = nb_months % 12;

		if (timeInfo->tm_mon + nb_months_final > 11) { // tm_mon is from 0 to 11
			nb_year++;
			nb_months_final = (timeInfo->tm_mon + nb_months_final) - 12;
			timeInfo->tm_mon = nb_months_final;
		}
		else {
			timeInfo->tm_mon += nb_months_final;
		}

		timeInfo->tm_year += nb_year;
	}

	void zs_datetime::add_days(int nb_days)
	{
		add_seconds(nb_days * ONE_DAY);
	}

	void zs_datetime::add_hours(int nb_hours)
	{
		add_seconds(nb_hours * ONE_HOUR);
	}

	void zs_datetime::add_minutes(int nb_minutes)
	{
		add_seconds(nb_minutes * ONE_MINUTE);
	}

	void zs_datetime::add_seconds(int nb_seconds)
	{
		struct tm *tm_new_time;
		//errno_t err;
		time_t new_seconds = mktime(timeInfo) + nb_seconds;
		delete timeInfo;

		tm_new_time = localtime(&new_seconds);

		timeInfo = new tm();
		_copy_from(tm_new_time);
	}

	bool zs_datetime::is_leapyear()
	{
		return _is_leapyear(get_year());
	}

	bool zs_datetime::is_leapyear(int year)
	{
		zs_datetime dateTemp(year, 1, 1);
		return dateTemp.is_leapyear();
	}

	void zs_datetime::_copy_from(const tm * otm)
	{
		timeInfo->tm_year = otm->tm_year;
		timeInfo->tm_mon = otm->tm_mon;
		timeInfo->tm_mday = otm->tm_mday;
		timeInfo->tm_hour = otm->tm_hour;
		timeInfo->tm_min = otm->tm_min;
		timeInfo->tm_sec = otm->tm_sec;
		timeInfo->tm_isdst = otm->tm_isdst;
		timeInfo->tm_wday = otm->tm_wday;
		timeInfo->tm_yday = otm->tm_yday;
	}

	zs_datetime zs_datetime::parse(const zs_string& format, const zs_string& value)
	{
		int year = 1970, month = 1, day = 1, hour = 0, minute = 0, second = 0;

		if (strcmp(format.c_str(), "") == 0) {
			THROW_RUNTIME_ERROR("format");
		}

		zs_string pattern_temp;
		int pattern_firstindex = 0;
		bool is_letter = false;
		period day_period = period::undefined;
		for (unsigned int index_char = 0; index_char < format.length(); index_char++) {
			//Check if the character is a valid pattern char
			if ((format[index_char] >= 'a' && format[index_char] <= 'z') ||
				(format[index_char] >= 'A' && format[index_char] <= 'Z')) {
				is_letter = true;
				if (pattern_temp.length() == 0) {
					pattern_temp += format[index_char];
					pattern_firstindex = index_char;
				}
				//Check if the pattern has not changed
				else if (format[index_char] == pattern_temp[pattern_temp.length() - 1]) {
					pattern_temp += format[index_char];
				}
			}
			//Check if the pattern has not changed
			if (format[index_char] != pattern_temp[pattern_temp.length() - 1] || index_char == format.length() - 1) {
				if (pattern_firstindex + pattern_temp.length() <= value.length()) { //Ensure that the value if long enough
					int *ptr_date_section = nullptr;
					if (pattern_temp == "yyyy") {
						ptr_date_section = &year;
					}
					if (pattern_temp == "MM") {
						ptr_date_section = &month;
					}
					if (pattern_temp == "dd") {
						ptr_date_section = &day;
					}
					if (pattern_temp == "HH") {
						ptr_date_section = &hour;
					}
					if (pattern_temp == "hh") {
						ptr_date_section = &hour;
						day_period = period::AM; //Set default day period
					}
					if (pattern_temp == "mm") {
						ptr_date_section = &minute;
					}
					if (pattern_temp == "ss") {
						ptr_date_section = &second;
					}
					if (pattern_temp == "tt") { //Day period
						zs_string period_str = value.substr(pattern_firstindex, pattern_temp.length());
						if (strcmp(period_str.c_str(), "AM") == 0) {
							day_period = period::AM;
						}
						else if(strcmp(period_str.c_str(), "PM") == 0) {
							day_period = period::PM;
						}
						else {
							THROW_RUNTIME_ERROR("invalid value for period");
						}
					}
					if (ptr_date_section != nullptr) {
						*ptr_date_section = _parse_intvalue(pattern_temp, pattern_firstindex, pattern_temp.length(), value);
					}
				}
				pattern_temp = "";
			}

			if (is_letter && pattern_temp.length() == 0) {
				pattern_temp += format[index_char];
				pattern_firstindex = index_char;
			}
		}

		return zs_datetime(year, month, day, hour, minute, second, day_period);
	}

	int zs_datetime::_parse_intvalue(const zs_string &pattern, int index, size_t mask_length, const zs_string &parse_str)
	{
		long converted_value;
		int ret_val;
		char *end;
		const char *parse_str_chr;

		zs_string value_parsed = parse_str.substr(index, mask_length);
		parse_str_chr = value_parsed.c_str();
		converted_value = strtol(parse_str_chr, &end, 10);
		if (parse_str_chr == end) {
			THROW_RUNTIME_ERROR("Unable to parse the mask '%s'", pattern.c_str());
		}
		ret_val = static_cast<int>(converted_value);
		return ret_val;
	}

	// Operators
	std::ostream& operator<<(std::ostream &os, const zs_datetime &dt)
	{
		char retVal[128] = "";
		sprintf(static_cast<char *>(retVal), "%d-%02d-%02d %02d:%02d:%02d",
			dt.timeInfo->tm_year + 1900,
			dt.timeInfo->tm_mon + 1,
			dt.timeInfo->tm_mday,
			dt.timeInfo->tm_hour,
			dt.timeInfo->tm_min,
			dt.timeInfo->tm_sec);
		os << static_cast<char *>(retVal);

		return os;
	}

	bool operator<(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return mktime(mdt.timeInfo) < mktime(odt.timeInfo);
	}

	bool operator>(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return mktime(odt.timeInfo) < mktime(mdt.timeInfo);
	}

	bool operator<=(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return !(mktime(mdt.timeInfo) > mktime(odt.timeInfo));
	}

	bool operator>=(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return !(mktime(mdt.timeInfo) < mktime(odt.timeInfo));
	}

	bool operator==(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return mktime(mdt.timeInfo) == mktime(odt.timeInfo);
	}

	bool operator!=(const zs_datetime &mdt, const zs_datetime &odt)
	{
		return !(mktime(mdt.timeInfo) == mktime(odt.timeInfo));
	}

	zs_timespan operator-(const zs_datetime &mdt, const zs_datetime &odt)
	{
		int days = 0, hours = 0, minutes = 0, seconds = 0;

		//Transfer both dates in a number of days
		time_t time_mdt = mktime(mdt.timeInfo);
		time_t time_odt = mktime(odt.timeInfo);
		double difference = difftime(time_mdt, time_odt) / (60 * 60 * 24);
		days = static_cast<int>(difference);

		if (mdt >= odt) {
			hours = mdt.get_hour() - odt.get_hour();
			seconds = mdt.get_second() - odt.get_second();
			minutes = mdt.get_minute() - odt.get_minute();
		}
		else {
			if (mdt.get_second() > odt.get_second()) {
				seconds = mdt.get_second() - odt.get_second() - 60;
				minutes += 1;
			}
			else {
				seconds = mdt.get_second() - odt.get_second();
			}

			if (mdt.get_minute() > odt.get_minute()) {
				minutes += mdt.get_minute() - odt.get_minute() - 60;
				hours += 1;
			}
			else {
				minutes += mdt.get_minute() - odt.get_minute();
			}

			if (mdt.get_hour() > odt.get_hour()) {
				hours += mdt.get_hour() - odt.get_hour() - 24;
			}
			else {
				hours += mdt.get_hour() - odt.get_hour();
			}
		}

		zs_timespan retVal(days, hours, minutes, seconds);
		return retVal;
	}
} // namespace zetscript
