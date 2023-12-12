/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ObjectIteratorScriptObjectZs_next(ScriptEngine *_se,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_se);
		oi->next();
	}

	void	ObjectIteratorScriptObjectZs_get(ScriptEngine *_se,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_se);
		oi->get();
	}


	bool	 ObjectIteratorScriptObjectZs_end(ScriptEngine *_se,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_se);
		return oi->end();
	}
}
