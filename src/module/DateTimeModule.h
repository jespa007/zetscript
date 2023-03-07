/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	zs_datetime * 		DateTimeModule_new(ZetScript *_zs);
	void DateTimeModule_constructor(
			ZetScript *_zs
			,zs_datetime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	);
	/*zs_datetime * 		DateTimeModule_add(zs_datetime *dt1,zs_datetime *dt2);
	zs_datetime * 		DateTimeModule_sub(zs_datetime *dt1,zs_datetime *dt2);*/

	void 				DateTimeModule_setUtc(ZetScript *_zs,zs_datetime *_this);

	void 				DateTimeModule_addSeconds(ZetScript *_zs,zs_datetime *_this,zs_int _seconds);
	void 				DateTimeModule_addMinutes(ZetScript *_zs,zs_datetime *_this,zs_int _minutes);
	void 				DateTimeModule_addHours(ZetScript *_zs,zs_datetime *_this,zs_int _hours);
	void 				DateTimeModule_addDays(ZetScript *_zs,zs_datetime *_this,zs_int _days);
	void 				DateTimeModule_addMonths(ZetScript *_zs,zs_datetime *_this,zs_int _months);
	void 				DateTimeModule_addYears(ZetScript *_zs,zs_datetime *_this,zs_int _years);

	zs_string			DateTimeModule_tostring(ZetScript *_zs,zs_datetime *_this);

	zs_int 				DateTimeModule_get_week_day(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_month_day(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_year_day(ZetScript *_zs,zs_datetime *_this);

	zs_int 				DateTimeModule_get_second(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_minute(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_hour(ZetScript *_zs,zs_datetime *_this);

	zs_int 				DateTimeModule_get_day(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_month(ZetScript *_zs,zs_datetime *_this);
	zs_int 				DateTimeModule_get_year(ZetScript *_zs,zs_datetime *_this);


	void		 		DateTimeModule_delete(ZetScript *_zs,zs_datetime *_this);

}
