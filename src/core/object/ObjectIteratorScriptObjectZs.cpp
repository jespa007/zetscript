/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ObjectIteratorScriptObjectZs_next(ScriptEngine *_script_engine,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_script_engine);
		oi->next();
	}

	void	ObjectIteratorScriptObjectZs_get(ScriptEngine *_script_engine,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_script_engine);
		oi->get();
	}


	bool	 ObjectIteratorScriptObjectZs_end(ScriptEngine *_script_engine,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_script_engine);
		return oi->end();
	}
}
