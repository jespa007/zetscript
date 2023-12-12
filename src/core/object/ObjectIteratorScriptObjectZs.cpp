/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ObjectIteratorScriptObjectZs_next(ScriptEngine *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->next();
	}

	void	ObjectIteratorScriptObjectZs_get(ScriptEngine *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->get();
	}


	bool	 ObjectIteratorScriptObjectZs_end(ScriptEngine *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		return oi->end();
	}
}
