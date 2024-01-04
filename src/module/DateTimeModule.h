/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ClassScriptObject;

	// member functions
	DateTime * 		DateTimeModule_new(ScriptEngine *_script_engine);
	void DateTimeModule_constructor(
			zetscript::ScriptEngine *_script_engine
			,DateTime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	);
	/*DateTime * 		DateTimeModule_add(DateTime *dt1,DateTime *dt2);*/

	ClassScriptObject * 		DateTimeModule_sub(ScriptEngine *_script_engine,DateTime *dt1,DateTime *dt2);

	void 				DateTimeModule_setUtc(ScriptEngine *_script_engine,DateTime *_this);

	void 				DateTimeModule_addSeconds(ScriptEngine *_script_engine,DateTime *_this,zs_int _seconds);
	void 				DateTimeModule_addMinutes(ScriptEngine *_script_engine,DateTime *_this,zs_int _minutes);
	void 				DateTimeModule_addHours(ScriptEngine *_script_engine,DateTime *_this,zs_int _hours);
	void 				DateTimeModule_addDays(ScriptEngine *_script_engine,DateTime *_this,zs_int _days);
	void 				DateTimeModule_addMonths(ScriptEngine *_script_engine,DateTime *_this,zs_int _months);
	void 				DateTimeModule_addYears(ScriptEngine *_script_engine,DateTime *_this,zs_int _years);

	String			DateTimeModule_tostring(ScriptEngine *_script_engine,DateTime *_this);

	zs_int 				DateTimeModule_get_week_day(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_month_day(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_year_day(ScriptEngine *_script_engine,DateTime *_this);

	zs_int 				DateTimeModule_get_second(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_minute(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_hour(ScriptEngine *_script_engine,DateTime *_this);

	zs_int 				DateTimeModule_get_day(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_month(ScriptEngine *_script_engine,DateTime *_this);
	zs_int 				DateTimeModule_get_year(ScriptEngine *_script_engine,DateTime *_this);


	void		 		DateTimeModule_delete(ScriptEngine *_script_engine,DateTime *_this);

}
