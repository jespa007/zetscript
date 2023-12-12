/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ArrayIteratorObjectZs_get(ScriptEngine *_script_engine,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_script_engine);
		vi->get();
	}

	void	 ArrayIteratorObjectZs_next(ScriptEngine *_script_engine,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_script_engine);
		vi->next();
	}

	bool	 ArrayIteratorObjectZs_end(ScriptEngine *_script_engine,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_script_engine);
		return vi->end();
	}
}
