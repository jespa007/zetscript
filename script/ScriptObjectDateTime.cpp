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

	//----------------------------

	ScriptObjectDateTime::ScriptObjectDateTime(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME;
	}


	void ScriptObjectDateTime::addSeconds(int _seconds)
	{
		datetime.add_seconds(_seconds);
	}

	void ScriptObjectDateTime::addMinutes(int _minutes)
	{
		datetime.add_minutes(_minutes);
	}

	void ScriptObjectDateTime::addHours(int _hours)
	{
		datetime.add_hours(_hours);
	}


	void ScriptObjectDateTime::addDays(int _days)	{
		datetime.add_days(_days);
	}

	void ScriptObjectDateTime::addMonths(int _months)
	{
		datetime.add_months(_months);
	}

	void ScriptObjectDateTime::addYears(int _years)
	{
		datetime.add_months(_years);
	}


	int ScriptObjectDateTime::getWeekDay() const
	{
		return datetime.get_week_day();
	}

	int ScriptObjectDateTime::getMonthDay() const
	{
		return datetime.get_month_day();
	}

	int ScriptObjectDateTime::getYearDay() const{
		return datetime.get_year_day();
	}


	int ScriptObjectDateTime::getSecond() const
	{
		return datetime.get_second();
	}

	int ScriptObjectDateTime::getMinute() const{
		return datetime.get_minute();
	}

	int ScriptObjectDateTime::getHour() const{
		return datetime.get_hour();
	}

	int ScriptObjectDateTime::getDay() const
	{
		return datetime.get_day();
	}

	int ScriptObjectDateTime::getMonth() const{
		return datetime.get_month();
	}

	int ScriptObjectDateTime::getYear() const{
		return datetime.get_year();
	}

	std::string ScriptObjectDateTime::toString(const std::string & _format){
		return datetime.to_string(_format);
	}

	std::string ScriptObjectDateTime::toString(){
		return datetime.to_string();
	}

}
