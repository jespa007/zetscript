/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	DateTime * 			DateTimeModule_new(ScriptEngine *_script_engine){
		ZS_UNUSUED_PARAM(_script_engine);
		return new DateTime();
	}

	void DateTimeModule_constructor(
			ScriptEngine *_script_engine
			,DateTime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	){
		ZS_UNUSUED_PARAM(_script_engine);
		*_this=DateTime(
			_year
			,_month
			,_day
			,_hour
			,_minute
			,_second
		);
	}

	/*DateTime * 			DateTimeModule_add(DateTime *dt1,DateTime *dt2){

		DateTime *dt=new DateTime;
		// *dt=*dt1->+*dt2;
		return dt;
	}*/

	ClassScriptObject * 			DateTimeModule_sub(ScriptEngine *_script_engine,DateTime *dt1,DateTime *dt2){
		ZS_UNUSUED_PARAM(_script_engine);
		TimeSpan *ts=new TimeSpan;
		*ts=*dt1-*dt2;
		return _script_engine->newClassScriptObject(ts);
	}

	void 					DateTimeModule_setUtc(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->setUtc();
	}

	void 					DateTimeModule_addSeconds(ScriptEngine *_script_engine,DateTime *_this,zs_int _seconds){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addSeconds(_seconds);
	}

	void 					DateTimeModule_addMinutes(ScriptEngine *_script_engine,DateTime *_this,zs_int _minutes){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addMinutes(_minutes);
	}

	void 					DateTimeModule_addHours(ScriptEngine *_script_engine,DateTime *_this,zs_int _hours){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addHours(_hours);
	}

	void 					DateTimeModule_addDays(ScriptEngine *_script_engine,DateTime *_this,zs_int _days){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addDays(_days);
	}

	void 					DateTimeModule_addMonths(ScriptEngine *_script_engine,DateTime *_this,zs_int _months){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addMonths(_months);
	}

	void 					DateTimeModule_addYears(ScriptEngine *_script_engine,DateTime *_this,zs_int _years){
		ZS_UNUSUED_PARAM(_script_engine);
		_this->addYears(_years);
	}

	zs_int 					DateTimeModule_get_week_day(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getWeekDay();
	}

	String		DateTimeModule_tostring(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return _this->toString();
	}

	zs_int 					DateTimeModule_get_month_day(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getMonthDay();
	}

	zs_int 					DateTimeModule_get_year_day(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getYearDay();
	}

	zs_int 					DateTimeModule_get_second(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getSecond();
	}

	zs_int 					DateTimeModule_get_minute(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getMinute();
	}

	zs_int 					DateTimeModule_get_hour(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getHour();
	}

	zs_int 					DateTimeModule_get_day(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getDay();
	}

	zs_int 					DateTimeModule_get_month(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getMonth();
	}

	zs_int 					DateTimeModule_get_year(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getYear();
	}

	void 					DateTimeModule_delete(ScriptEngine *_script_engine,DateTime *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		delete _this;
	}
}
