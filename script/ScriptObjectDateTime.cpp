/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


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
		 if(!(year % 4))
		    {
		        if(!(year % 100))
		        {
		            if(!(year % 400))
		            {
		                return true;
		            }
		            else
		            {
		                return false;
		            }
		        }
		        else
		        {
		            return true;
		        }
		    }
		    else
		    {
		        return false;
		    }
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
		//the code below was done by me
		if (isLeapYear(year) && month == 2)
		{
			maxDay = 29;
			return maxDay;
		}
		switch (month)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:

				maxDay = 31;
				break;

		case 4:
		case 6:
		case 9:
		case 11:

				maxDay = 30;
				break;

		}
	//this has been modified.
	//Use the DAYS_PER_MONTH array
	return 31;
	}


	void ScriptObjectDateTime::addDay(bool forward)
	{
		if forward == true;
	{
		if today.day = 1;
		{
			++today._month;
		}
		if today._month > 12;
		{
			today._month = 1;
			++today._year;
		}
	}
	else
	{
		if (today._day > 1)
		{
			--today._day;
			addDay = _today;
		}
		else
		{
			--today._month;

		if (today._month == 0;)
		{
			today._month = 12;
			-- toda._year
		}
			today._day = maxDay (today._month, today._year)
		}

//this has been already modified and will hopefully work.
	}
	}


	bool ScriptObjectDateTime::isLeapDay(int day, int month, int year)
	{if (date.day == 29 && date.month is 2 && leapyear(date.year))
	{
		leapday = true;
	}
	else
	{
		leapday = false;
	}
//this has been modified.
return false;
	}


	void ScriptObjectDateTime::addYears(int years)
	{//the code below was done by me
		if (years == 0)
		{
			return;
		}
		if (today == leapday && (today + years) != leaday)
		{
			today._day = 28;
		}
		today.year = today.year + years;
	}

	void ScriptObjectDateTime::addMonths(int months)
	{//the code below was done by me
		if (months == 0)
		{
			return;
		{
		deltayears = months/12;
		deltamonths = months % 12;
		if (months > 0)
		{
			if (today._month + deltamonths > 12)
			{
			++deltayears
			newmonth = (today._month + deltamonths) - 12;
			}
			else
			{
				newmonth = today.month + deltamonths;
			}
		}
		else //months is negative
		{
			if (today._month + deltamonths < 1)
			{
				--deltayears;
				newmonth = today._month + deltamonths + 12;
			}
			else
			{
				newmonth = today._month + deltamonths;
			}
		}
		if (today._day > maxDay(newmonth, today._year + deltayears)
		{
			today._day = maxDay;
		}
		today._year = today._year + deltayears
		today._month = newmonth
	}

	void ScriptObjectDateTime::addDays(int days)
	{//the code below was done by me
		if(days < 0)
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
			}
	}

}
