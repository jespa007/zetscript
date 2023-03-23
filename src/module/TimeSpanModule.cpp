/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	zs_timespan * 			TimeSpanModule_new(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return new zs_timespan();
	}

	zs_int 					TimeSpanModule_get_seconds(ZetScript *_zs,zs_timespan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_seconds();
	}

	zs_int 					TimeSpanModule_get_minutes(ZetScript *_zs,zs_timespan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_minutes();
	}

	zs_int 					TimeSpanModule_get_hours(ZetScript *_zs,zs_timespan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_hours();
	}

	zs_int 					TimeSpanModule_get_days(ZetScript *_zs,zs_timespan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->get_days();
	}

	void 					TimeSpanModule_delete(ZetScript *_zs,zs_timespan *_this){
		ZS_UNUSUED_PARAM(_zs);
		delete _this;
	}
}
