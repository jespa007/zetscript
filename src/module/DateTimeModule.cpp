/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	//----
	zs_datetime * 			DateTimeModuleWrap_new(){
		return new zs_datetime();
	}

	zs_datetime * 			DateTimeModuleWrap_add(zs_datetime *dt1,zs_datetime *dt2){
		zs_datetime *dt=new zs_datetime;
		//*dt=*dt1->+*dt2;
		return dt;
	}

	zs_datetime * 			DateTimeModuleWrap_sub(zs_datetime *dt1,zs_datetime *dt2){
		zs_datetime *dt=new zs_datetime;
		//*dt=*dt1-*dt2;
		return dt;
	}

	void 					DateTimeModuleWrap_setUtc(zs_datetime *_this){
		_this->set_utc();
	}

	void 					DateTimeModuleWrap_addSeconds(zs_datetime *_this,zs_int _seconds){
		_this->add_seconds(_seconds);
	}

	void 					DateTimeModuleWrap_addMinutes(zs_datetime *_this,zs_int _minutes){
		_this->add_minutes(_minutes);
	}

	void 					DateTimeModuleWrap_addHours(zs_datetime *_this,zs_int _hours){
		_this->add_hours(_hours);
	}

	void 					DateTimeModuleWrap_addDays(zs_datetime *_this,zs_int _days){
		_this->add_days(_days);
	}

	void 					DateTimeModuleWrap_addMonths(zs_datetime *_this,zs_int _months){
		_this->add_months(_months);
	}

	void 					DateTimeModuleWrap_addYears(zs_datetime *_this,zs_int _years){
		_this->add_years(_years);
	}

	zs_int 					DateTimeModuleWrap_get_week_day(zs_datetime *_this){
		return  _this->get_week_day();
	}

	std::string		DateTimeModuleWrap_toString(zs_datetime *_this){
		return _this->to_string();
	}

	zs_int 					DateTimeModuleWrap_get_month_day(zs_datetime *_this){
		return  _this->get_month_day();
	}

	zs_int 					DateTimeModuleWrap_get_year_day(zs_datetime *_this){
		return  _this->get_year_day();
	}

	zs_int 					DateTimeModuleWrap_get_second(zs_datetime *_this){
		return  _this->get_second();
	}

	zs_int 					DateTimeModuleWrap_get_minute(zs_datetime *_this){
		return  _this->get_minute();
	}

	zs_int 					DateTimeModuleWrap_get_hour(zs_datetime *_this){
		return  _this->get_hour();
	}

	zs_int 					DateTimeModuleWrap_get_day(zs_datetime *_this){
		return  _this->get_day();
	}

	zs_int 					DateTimeModuleWrap_get_month(zs_datetime *_this){
		return  _this->get_month();
	}

	zs_int 					DateTimeModuleWrap_get_year(zs_datetime *_this){
		return  _this->get_year();
	}

	void 					DateTimeModuleWrap_delete(zs_datetime *_this){
		delete _this;
	}
}
