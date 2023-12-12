/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	TimeSpan * 			TimeSpanModule_new(ScriptEngine *_se){
		ZS_UNUSUED_PARAM(_se);
		return new TimeSpan();
	}

	zs_int 					TimeSpanModule_get_seconds(ScriptEngine *_se,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_se);
		return  _this->getSeconds();
	}

	zs_int 					TimeSpanModule_get_minutes(ScriptEngine *_se,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_se);
		return  _this->getMinutes();
	}

	zs_int 					TimeSpanModule_get_hours(ScriptEngine *_se,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_se);
		return  _this->getHours();
	}

	zs_int 					TimeSpanModule_get_days(ScriptEngine *_se,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_se);
		return  _this->getDays();
	}

	void 					TimeSpanModule_delete(ScriptEngine *_se,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_se);
		delete _this;
	}
}
