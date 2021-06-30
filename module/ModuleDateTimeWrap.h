/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




namespace zetscript{

	// member functions
	zs_datetime * 		ModuleDateTimeWrap_new(void);

	zs_datetime * 		ModuleDateTimeWrap_add(zs_datetime *dt1,zs_datetime *dt2);
	zs_datetime * 		ModuleDateTimeWrap_sub(zs_datetime *dt1,zs_datetime *dt2);

	void 				ModuleDateTimeWrap_setUtc(zs_datetime *_this);

	void 				ModuleDateTimeWrap_addSeconds(zs_datetime *_this,zs_int _seconds);
	void 				ModuleDateTimeWrap_addMinutes(zs_datetime *_this,zs_int _minutes);
	void 				ModuleDateTimeWrap_addHours(zs_datetime *_this,zs_int _hours);
	void 				ModuleDateTimeWrap_addDays(zs_datetime *_this,zs_int _days);
	void 				ModuleDateTimeWrap_addMonths(zs_datetime *_this,zs_int _months);
	void 				ModuleDateTimeWrap_addYears(zs_datetime *_this,zs_int _years);

	std::string			ModuleDateTimeWrap_toString(zs_datetime *_this);

	zs_int 				ModuleDateTimeWrap_get_week_day(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_month_day(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_year_day(zs_datetime *_this);

	zs_int 				ModuleDateTimeWrap_get_second(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_minute(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_hour(zs_datetime *_this);

	zs_int 				ModuleDateTimeWrap_get_day(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_month(zs_datetime *_this);
	zs_int 				ModuleDateTimeWrap_get_year(zs_datetime *_this);


	void		 		ModuleDateTimeWrap_delete(zs_datetime *_this);

}
