/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	zs_datetime * 		DateTimeModule_new(void);
	zs_datetime * 		DateTimeModule_add(zs_datetime *dt1,zs_datetime *dt2);
	zs_datetime * 		DateTimeModule_sub(zs_datetime *dt1,zs_datetime *dt2);

	void 				DateTimeModule_setUtc(zs_datetime *_this);

	void 				DateTimeModule_addSeconds(zs_datetime *_this,zs_int _seconds);
	void 				DateTimeModule_addMinutes(zs_datetime *_this,zs_int _minutes);
	void 				DateTimeModule_addHours(zs_datetime *_this,zs_int _hours);
	void 				DateTimeModule_addDays(zs_datetime *_this,zs_int _days);
	void 				DateTimeModule_addMonths(zs_datetime *_this,zs_int _months);
	void 				DateTimeModule_addYears(zs_datetime *_this,zs_int _years);

	zs_string			DateTimeModule_toString(zs_datetime *_this);

	zs_int 				DateTimeModule_get_week_day(zs_datetime *_this);
	zs_int 				DateTimeModule_get_month_day(zs_datetime *_this);
	zs_int 				DateTimeModule_get_year_day(zs_datetime *_this);

	zs_int 				DateTimeModule_get_second(zs_datetime *_this);
	zs_int 				DateTimeModule_get_minute(zs_datetime *_this);
	zs_int 				DateTimeModule_get_hour(zs_datetime *_this);

	zs_int 				DateTimeModule_get_day(zs_datetime *_this);
	zs_int 				DateTimeModule_get_month(zs_datetime *_this);
	zs_int 				DateTimeModule_get_year(zs_datetime *_this);


	void		 		DateTimeModule_delete(zs_datetime *_this);

}
