/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	//----
	zs_datetime * 			ModuleDateTimeWrap_new(){
		return new zs_datetime();
	}

	zs_datetime * 			ModuleDateTimeWrap_add(zs_datetime *dt1,zs_datetime *dt2){
		zs_datetime *dt=new zs_datetime;
		//*dt=*dt1->+*dt2;
		return dt;
	}

	zs_datetime * 			ModuleDateTimeWrap_sub(zs_datetime *dt1,zs_datetime *dt2){
		zs_datetime *dt=new zs_datetime;
		//*dt=*dt1-*dt2;
		return dt;
	}

	void 					ModuleDateTimeWrap_setUtc(zs_datetime *_this){
		_this->set_utc();
	}

	void 					ModuleDateTimeWrap_addSeconds(zs_datetime *_this,zs_int _seconds){
		_this->add_seconds(_seconds);
	}

	void 					ModuleDateTimeWrap_addMinutes(zs_datetime *_this,zs_int _minutes){
		_this->add_minutes(_minutes);
	}

	void 					ModuleDateTimeWrap_addHours(zs_datetime *_this,zs_int _hours){
		_this->add_hours(_hours);
	}

	void 					ModuleDateTimeWrap_addDays(zs_datetime *_this,zs_int _days){
		_this->add_days(_days);
	}

	void 					ModuleDateTimeWrap_addMonths(zs_datetime *_this,zs_int _months){
		_this->add_months(_months);
	}

	void 					ModuleDateTimeWrap_addYears(zs_datetime *_this,zs_int _years){
		_this->add_years(_years);
	}

	zs_int 					ModuleDateTimeWrap_get_week_day(zs_datetime *_this){
		return  _this->get_week_day();
	}

	std::string		ModuleDateTimeWrap_toString(zs_datetime *_this){
		return _this->to_string();
	}

	zs_int 					ModuleDateTimeWrap_get_month_day(zs_datetime *_this){
		return  _this->get_month_day();
	}

	zs_int 					ModuleDateTimeWrap_get_year_day(zs_datetime *_this){
		return  _this->get_year_day();
	}

	zs_int 					ModuleDateTimeWrap_get_second(zs_datetime *_this){
		return  _this->get_second();
	}

	zs_int 					ModuleDateTimeWrap_get_minute(zs_datetime *_this){
		return  _this->get_minute();
	}

	zs_int 					ModuleDateTimeWrap_get_hour(zs_datetime *_this){
		return  _this->get_hour();
	}

	zs_int 					ModuleDateTimeWrap_get_day(zs_datetime *_this){
		return  _this->get_day();
	}

	zs_int 					ModuleDateTimeWrap_get_month(zs_datetime *_this){
		return  _this->get_month();
	}

	zs_int 					ModuleDateTimeWrap_get_year(zs_datetime *_this){
		return  _this->get_year();
	}

	void 					ModuleDateTimeWrap_delete(zs_datetime *_this){
		delete _this;
	}
}