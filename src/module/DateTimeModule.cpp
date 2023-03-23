/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	zs_datetime * 			DateTimeModule_new(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return new zs_datetime();
	}

	void DateTimeModule_constructor(
			ZetScript *_zs
			,zs_datetime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	){
		ZS_UNUSUED_PARAM(_zs);
		*_this=zs_datetime(
			_year
			,_month
			,_day
			,_hour
			,_minute
			,_second
		);
	}

	/*zs_datetime * 			DateTimeModule_add(zs_datetime *dt1,zs_datetime *dt2){

		zs_datetime *dt=new zs_datetime;
		// *dt=*dt1->+*dt2;
		return dt;
	}*/

	zs_timespan * 			DateTimeModule_sub(ZetScript *_zs,zs_datetime *dt1,zs_datetime *dt2){
		ZS_UNUSUED_PARAM(_zs);
		zs_timespan *ts=new zs_timespan;
		*ts=*dt1-*dt2;
		return ts;
	}

	void 					DateTimeModule_setUtc(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		_this->set_utc();
	}

	void 					DateTimeModule_addSeconds(ZetScript *_zs,zs_datetime *_this,zs_int _seconds){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_seconds(_seconds);
	}

	void 					DateTimeModule_addMinutes(ZetScript *_zs,zs_datetime *_this,zs_int _minutes){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_minutes(_minutes);
	}

	void 					DateTimeModule_addHours(ZetScript *_zs,zs_datetime *_this,zs_int _hours){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_hours(_hours);
	}

	void 					DateTimeModule_addDays(ZetScript *_zs,zs_datetime *_this,zs_int _days){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_days(_days);
	}

	void 					DateTimeModule_addMonths(ZetScript *_zs,zs_datetime *_this,zs_int _months){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_months(_months);
	}

	void 					DateTimeModule_addYears(ZetScript *_zs,zs_datetime *_this,zs_int _years){
		ZS_UNUSUED_PARAM(_zs);
		_this->add_years(_years);
	}

	zs_int 					DateTimeModule_get_week_day(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_week_day();
	}

	zs_string		DateTimeModule_tostring(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return _this->to_string();
	}

	zs_int 					DateTimeModule_get_month_day(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_month_day();
	}

	zs_int 					DateTimeModule_get_year_day(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_year_day();
	}

	zs_int 					DateTimeModule_get_second(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_second();
	}

	zs_int 					DateTimeModule_get_minute(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_minute();
	}

	zs_int 					DateTimeModule_get_hour(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_hour();
	}

	zs_int 					DateTimeModule_get_day(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_day();
	}

	zs_int 					DateTimeModule_get_month(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_month();
	}

	zs_int 					DateTimeModule_get_year(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_year();
	}

	void 					DateTimeModule_delete(ZetScript *_zs,zs_datetime *_this){
		ZS_UNUSUED_PARAM(_zs);
		delete _this;
	}
}
