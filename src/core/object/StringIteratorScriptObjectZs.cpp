/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorScriptObjectZs_get(ScriptEngine *_script_engine,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_script_engine);
		si->get();
	}

	void	 StringIteratorScriptObjectZs_next(ScriptEngine *_script_engine,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_script_engine);
		si->next();
	}

	bool	 StringIteratorScriptObjectZs_end(ScriptEngine *_script_engine,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_script_engine);
		return si->end();
	}


}
