/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ONE_YEAR_DAYS 		86400*365 //24 hours * 60 mins * 60 secs

#define ONE_DAY_SECS 		86400 //24 hours * 60 mins * 60 secs
#define ONE_HOUR_SECS  		3600 //60 mins * 60 secs
#define ONE_MINUTE_SECS 	60 // 60 secs

//https://github.com/jeremydumais/CPP-DateTime-library/blob/master/src/datetime.h

namespace zetscript{

	ScriptObjectDateTime * ScriptObjectDateTime::newScriptObjectDateTime(ZetScript *_zs){
		ScriptObjectDateTime *dt=new ScriptObjectDateTime();
		dt->init(_zs);
		return dt;

	}

	ScriptObjectDateTime::ScriptObjectDateTime(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME;
		//now_tm=gmtime(NULL);
	    time_t t = time(0);   // get time now
	    struct tm * now = localtime( & t );

		_second=now->tm_sec;
		_minute=now->tm_min;
		_hour=now->tm_hour;

		_day=now->tm_mday;
		_month=now->tm_mon;
		_year=now->tm_year;

		  //struct tm* now_tm;
	}

	bool ScriptObjectDateTime::isValid() const
	{
		if(_month < MIN_MONTH || _month > MAX_MONTH)
		    {
		        std::cerr << "Invalid date " << std::endl;
		        return false;
		    }
		    int daysThisMonth = maxDay(_month, _year);

		    if(_day < MIN_DAY || _day > daysThisMonth)
		    {
		        std::cerr << "Invalid date " << std::endl;
		        return false;

		    }


		    return true;
	}

	bool ScriptObjectDateTime::isLeapYear(int year)
	{
		return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
	}

	bool ScriptObjectDateTime::isLeapYear() const
	{
	    return isLeapYear(_year);
	}

	bool ScriptObjectDateTime::isLeapDay() const
	{
	    return isLeapDay(_day, _month, _year);
	}



	int ScriptObjectDateTime::maxDay(int month, int year)
	{
		if(month == 0 || month == 2 || month == 4 || month == 6 || month == 7 || month == 9 || month == 11){
		        return 31;
		}else if(month == 3 || month == 5 || month == 8 || month == 10){
				return 30;
		}else if (isLeapYear(year)){
				return 29;
		}
		return 28;
	}

	void ScriptObjectDateTime::addSeconds(int nb_seconds)
	{
		struct tm *tm_new_time;
		//errno_t err;
		time_t new_seconds = mktime(timeInfo) + nb_seconds;
		delete timeInfo;

		tm_new_time = localtime(&new_seconds);

		timeInfo = new tm();
		_copy_from(tm_new_time);
	}


	void ScriptObjectDateTime::addDays(int nb_days)	{
		addSeconds(nb_days * ONE_DAY_SECS);
	}


	bool ScriptObjectDateTime::isLeapDay(int day, int month, int year)
	{
		return day == 29 && month == 1 && isLeapYear(year);
	}


	void ScriptObjectDateTime::addYears(int years)
	{
		addDays(ONE_YEAR_DAYS*years);
	}

	void ScriptObjectDateTime::addMonths(int months)
	{
		if (months == 0) return;

		int deltayears = months/12;
		int deltamonths = months % 12;
		int newmonth=0;
		if (months > 0)
		{
			if (_month + deltamonths > 12)
			{
				++deltayears;
				newmonth = (_month + deltamonths) - 12;
			}
			else
			{
				newmonth = _month + deltamonths;
			}
		}
		else //months is negative
		{
			if (_month + deltamonths < 1)
			{
				--deltayears;
				newmonth = _month + deltamonths + 12;
			}
			else
			{
				newmonth = _month + deltamonths;
			}
		}
		if (_day > maxDay(newmonth, _year + deltayears))
		{
			_day = maxDay(_month,_year);
		}
		_year = _year + deltayears;
		_month = newmonth;
	}

	void ScriptObjectDateTime::addDays(int days)
	{//the code below was done by me
		/*if(days < 0)
		{
			for (count = -1; count--)
			{
				addDay(today, false)
			}
		}
		else
			for (count = 1; count++)
			{
				addDay(today, true)
			}*/
	}

}
