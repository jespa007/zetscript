/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




namespace zetscript{

	// member functions
	zs_datetime * 		DateTimeModuleWrap_new(void);

	zs_datetime * 		DateTimeModuleWrap_add(zs_datetime *dt1,zs_datetime *dt2);
	zs_datetime * 		DateTimeModuleWrap_sub(zs_datetime *dt1,zs_datetime *dt2);

	void 				DateTimeModuleWrap_setUtc(zs_datetime *_this);

	void 				DateTimeModuleWrap_addSeconds(zs_datetime *_this,zs_int _seconds);
	void 				DateTimeModuleWrap_addMinutes(zs_datetime *_this,zs_int _minutes);
	void 				DateTimeModuleWrap_addHours(zs_datetime *_this,zs_int _hours);
	void 				DateTimeModuleWrap_addDays(zs_datetime *_this,zs_int _days);
	void 				DateTimeModuleWrap_addMonths(zs_datetime *_this,zs_int _months);
	void 				DateTimeModuleWrap_addYears(zs_datetime *_this,zs_int _years);

	std::string			DateTimeModuleWrap_toString(zs_datetime *_this);

	zs_int 				DateTimeModuleWrap_get_week_day(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_month_day(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_year_day(zs_datetime *_this);

	zs_int 				DateTimeModuleWrap_get_second(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_minute(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_hour(zs_datetime *_this);

	zs_int 				DateTimeModuleWrap_get_day(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_month(zs_datetime *_this);
	zs_int 				DateTimeModuleWrap_get_year(zs_datetime *_this);


	void		 		DateTimeModuleWrap_delete(zs_datetime *_this);

}
