/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	TimeSpan * 		TimeSpanModule_new(ScriptEngine *_script_engine);

	zs_int 				TimeSpanModule_get_seconds(ScriptEngine *_script_engine,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_minutes(ScriptEngine *_script_engine,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_hours(ScriptEngine *_script_engine,TimeSpan *_this);

	zs_int 				TimeSpanModule_get_days(ScriptEngine *_script_engine,TimeSpan *_this);

	void		 		TimeSpanModule_delete(ScriptEngine *_script_engine,TimeSpan *_this);

}
