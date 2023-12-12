/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	TimeSpan * 		TimeSpanModule_new(ScriptEngine *_zs);

	zs_int 				TimeSpanModule_get_seconds(ScriptEngine *_zs,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_minutes(ScriptEngine *_zs,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_hours(ScriptEngine *_zs,TimeSpan *_this);

	zs_int 				TimeSpanModule_get_days(ScriptEngine *_zs,TimeSpan *_this);

	void		 		TimeSpanModule_delete(ScriptEngine *_zs,TimeSpan *_this);

}
