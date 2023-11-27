/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	TimeSpan * 			TimeSpanModule_new(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return new TimeSpan();
	}

	zs_int 					TimeSpanModule_get_seconds(ZetScript *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getSeconds();
	}

	zs_int 					TimeSpanModule_get_minutes(ZetScript *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getMinutes();
	}

	zs_int 					TimeSpanModule_get_hours(ZetScript *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getHours();
	}

	zs_int 					TimeSpanModule_get_days(ZetScript *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getDays();
	}

	void 					TimeSpanModule_delete(ZetScript *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		delete _this;
	}
}
