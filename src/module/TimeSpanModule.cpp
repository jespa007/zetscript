/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----
	TimeSpan * 			TimeSpanModule_new(ScriptEngine *_script_engine){
		ZS_UNUSUED_PARAM(_script_engine);
		return new TimeSpan();
	}

	zs_int 					TimeSpanModule_get_seconds(ScriptEngine *_script_engine,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getSeconds();
	}

	zs_int 					TimeSpanModule_get_minutes(ScriptEngine *_script_engine,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getMinutes();
	}

	zs_int 					TimeSpanModule_get_hours(ScriptEngine *_script_engine,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getHours();
	}

	zs_int 					TimeSpanModule_get_days(ScriptEngine *_script_engine,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		return  _this->getDays();
	}

	void 					TimeSpanModule_delete(ScriptEngine *_script_engine,TimeSpan *_this){
		ZS_UNUSUED_PARAM(_script_engine);
		delete _this;
	}
}
