/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ClassScriptObject;

	// member functions
	DateTime * 		DateTimeModule_new(ZetScript *_zs);
	void DateTimeModule_constructor(
			ZetScript *_zs
			,DateTime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	);
	/*DateTime * 		DateTimeModule_add(DateTime *dt1,DateTime *dt2);*/

	ClassScriptObject * 		DateTimeModule_sub(ZetScript *_zs,DateTime *dt1,DateTime *dt2);

	void 				DateTimeModule_setUtc(ZetScript *_zs,DateTime *_this);

	void 				DateTimeModule_addSeconds(ZetScript *_zs,DateTime *_this,zs_int _seconds);
	void 				DateTimeModule_addMinutes(ZetScript *_zs,DateTime *_this,zs_int _minutes);
	void 				DateTimeModule_addHours(ZetScript *_zs,DateTime *_this,zs_int _hours);
	void 				DateTimeModule_addDays(ZetScript *_zs,DateTime *_this,zs_int _days);
	void 				DateTimeModule_addMonths(ZetScript *_zs,DateTime *_this,zs_int _months);
	void 				DateTimeModule_addYears(ZetScript *_zs,DateTime *_this,zs_int _years);

	String			DateTimeModule_tostring(ZetScript *_zs,DateTime *_this);

	zs_int 				DateTimeModule_get_week_day(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_month_day(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_year_day(ZetScript *_zs,DateTime *_this);

	zs_int 				DateTimeModule_get_second(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_minute(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_hour(ZetScript *_zs,DateTime *_this);

	zs_int 				DateTimeModule_get_day(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_month(ZetScript *_zs,DateTime *_this);
	zs_int 				DateTimeModule_get_year(ZetScript *_zs,DateTime *_this);


	void		 		DateTimeModule_delete(ZetScript *_zs,DateTime *_this);

}
