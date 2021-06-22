/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_now(ZetScript *_zs);
	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_nowUtc(ZetScript *_zs);

	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_add(ScriptObjectDateTime *dt1,ScriptObjectDateTime *dt2);
	ScriptObjectDateTime * 	ScriptObjectDateTimeWrap_sub(ScriptObjectDateTime *dt1,ScriptObjectDateTime *dt2);

	void 					ScriptObjectDateTimeWrap_addSeconds(ScriptObjectDateTime *_this,zs_int _seconds);
	void 					ScriptObjectDateTimeWrap_addMinutes(ScriptObjectDateTime *_this,zs_int _minutes);
	void 					ScriptObjectDateTimeWrap_addHours(ScriptObjectDateTime *_this,zs_int _hours);
	void 					ScriptObjectDateTimeWrap_addDays(ScriptObjectDateTime *_this,zs_int _days);
	void 					ScriptObjectDateTimeWrap_addMonths(ScriptObjectDateTime *_this,zs_int _months);
	void 					ScriptObjectDateTimeWrap_addYears(ScriptObjectDateTime *_this,zs_int _years);


	zs_int 					ScriptObjectDateTimeWrap_get_week_day(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_month_day(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_year_day(ScriptObjectDateTime *_this);

	zs_int 					ScriptObjectDateTimeWrap_get_second(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_minute(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_hour(ScriptObjectDateTime *_this);

	zs_int 					ScriptObjectDateTimeWrap_get_day(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_month(ScriptObjectDateTime *_this);
	zs_int 					ScriptObjectDateTimeWrap_get_year(ScriptObjectDateTime *_this);





}
