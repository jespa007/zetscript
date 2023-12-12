/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	TimeSpan * 			TimeSpanModule_new(ScriptEngine *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return new TimeSpan();
	}

	zs_int 					TimeSpanModule_get_seconds(ScriptEngine *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getSeconds();
	}

	zs_int 					TimeSpanModule_get_minutes(ScriptEngine *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getMinutes();
	}

	zs_int 					TimeSpanModule_get_hours(ScriptEngine *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getHours();
	}

	zs_int 					TimeSpanModule_get_days(ScriptEngine *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		return  _this->getDays();
	}

	void 					TimeSpanModule_delete(ScriptEngine *_zs,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_zs);
		delete _this;
	}
}
