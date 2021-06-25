/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


namespace zetscript{

	ScriptObjectDateTime * ScriptObjectDateTimeWrap_now(ZetScript *_zs){
		ScriptObjectDateTime *dt=ScriptObjectDateTime::newScriptObjectDateTime(_zs);
		return dt;
	}


	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_nowUtc(ZetScript *_zs){
		ScriptObjectDateTime *dt=ScriptObjectDateTime::newScriptObjectDateTime(_zs);
		dt->datetime.set_utc();
		return dt;
	}

	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_add(ScriptObjectDateTime *dt1,ScriptObjectDateTime *dt2){
		ScriptObjectDateTime *dt=ScriptObjectDateTime::newScriptObjectDateTime(dt1->getZetScript());
		return dt;
	}

	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_sub(ScriptObjectDateTime *dt1,ScriptObjectDateTime *dt2){
		ScriptObjectDateTime *dt=ScriptObjectDateTime::newScriptObjectDateTime(dt1->getZetScript());
		return dt;
	}

	void 					ScriptObjectDateTimeWrap_addSeconds(ScriptObjectDateTime *_this,zs_int _seconds){
		_this->datetime.add_seconds(_seconds);
	}

	void 					ScriptObjectDateTimeWrap_addMinutes(ScriptObjectDateTime *_this,zs_int _minutes){
		_this->datetime.add_minutes(_minutes);
	}

	void 					ScriptObjectDateTimeWrap_addHours(ScriptObjectDateTime *_this,zs_int _hours){
		_this->datetime.add_hours(_hours);
	}

	void 					ScriptObjectDateTimeWrap_addDays(ScriptObjectDateTime *_this,zs_int _days){
		_this->datetime.add_days(_days);
	}

	void 					ScriptObjectDateTimeWrap_addMonths(ScriptObjectDateTime *_this,zs_int _months){
		_this->datetime.add_months(_months);
	}

	void 					ScriptObjectDateTimeWrap_addYears(ScriptObjectDateTime *_this,zs_int _years){
		_this->datetime.add_years(_years);
	}

	zs_int 					ScriptObjectDateTimeWrap_get_week_day(ScriptObjectDateTime *_this){
		return  _this->datetime.get_week_day();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_month_day(ScriptObjectDateTime *_this){
		return  _this->datetime.get_month_day();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_year_day(ScriptObjectDateTime *_this){
		return  _this->datetime.get_year_day();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_second(ScriptObjectDateTime *_this){
		return  _this->datetime.get_second();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_minute(ScriptObjectDateTime *_this){
		return  _this->datetime.get_minute();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_hour(ScriptObjectDateTime *_this){
		return  _this->datetime.get_hour();
	}


	zs_int 					ScriptObjectDateTimeWrap_get_day(ScriptObjectDateTime *_this){
		return  _this->datetime.get_day();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_month(ScriptObjectDateTime *_this){
		return  _this->datetime.get_month();
	}

	zs_int 					ScriptObjectDateTimeWrap_get_year(ScriptObjectDateTime *_this){
		return  _this->datetime.get_year();
	}
}
