/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	DateTime * 			DateTimeModule_new(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return new DateTime();
	}

	void DateTimeModule_constructor(
			ZetScript *_zs
			,DateTime * _this
			,zs_int _year
			,zs_int _month
			,zs_int _day
			,zs_int _hour
			,zs_int _minute
			,zs_int _second
	){
		ZS_UNUSUED_PARAM(_zs);
		*_this=DateTime(
			_year
			,_month
			,_day
			,_hour
			,_minute
			,_second
		);
	}

	/*DateTime * 			DateTimeModule_add(DateTime *dt1,DateTime *dt2){

		DateTime *dt=new DateTime;
		// *dt=*dt1->+*dt2;
		return dt;
	}*/

	ClassScriptObject * 			DateTimeModule_sub(ZetScript *_zs,DateTime *dt1,DateTime *dt2){
		ZS_UNUSUED_PARAM(_zs);
		TimeSpan *ts=new TimeSpan;
		*ts=*dt1-*dt2;
		return _zs->newClassScriptObject(ts);
	}

	void 					DateTimeModule_setUtc(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		_this->setUtc();
	}

	void 					DateTimeModule_addSeconds(ZetScript *_zs,DateTime *_this,zs_int _seconds){
		ZS_UNUSUED_PARAM(_zs);
		_this->addSeconds(_seconds);
	}

	void 					DateTimeModule_addMinutes(ZetScript *_zs,DateTime *_this,zs_int _minutes){
		ZS_UNUSUED_PARAM(_zs);
		_this->addMinutes(_minutes);
	}

	void 					DateTimeModule_addHours(ZetScript *_zs,DateTime *_this,zs_int _hours){
		ZS_UNUSUED_PARAM(_zs);
		_this->addHours(_hours);
	}

	void 					DateTimeModule_addDays(ZetScript *_zs,DateTime *_this,zs_int _days){
		ZS_UNUSUED_PARAM(_zs);
		_this->addDays(_days);
	}

	void 					DateTimeModule_addMonths(ZetScript *_zs,DateTime *_this,zs_int _months){
		ZS_UNUSUED_PARAM(_zs);
		_this->addMonths(_months);
	}

	void 					DateTimeModule_addYears(ZetScript *_zs,DateTime *_this,zs_int _years){
		ZS_UNUSUED_PARAM(_zs);
		_this->addYears(_years);
	}

	zs_int 					DateTimeModule_get_week_day(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getWeekDay();
	}

	String		DateTimeModule_tostring(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return _this->toString();
	}

	zs_int 					DateTimeModule_get_month_day(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getMonthDay();
	}

	zs_int 					DateTimeModule_get_year_day(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getYearDay();
	}

	zs_int 					DateTimeModule_get_second(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getSecond();
	}

	zs_int 					DateTimeModule_get_minute(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getMinute();
	}

	zs_int 					DateTimeModule_get_hour(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getHour();
	}

	zs_int 					DateTimeModule_get_day(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getDay();
	}

	zs_int 					DateTimeModule_get_month(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getMonth();
	}

	zs_int 					DateTimeModule_get_year(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getYear();
	}

	void 					DateTimeModule_delete(ZetScript *_zs,DateTime *_this){
		ZS_UNUSUED_PARAM(_zs);
		delete _this;
	}
}
