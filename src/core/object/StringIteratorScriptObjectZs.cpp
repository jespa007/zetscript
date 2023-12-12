/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorScriptObjectZs_get(ScriptEngine *_se,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_se);
		si->get();
	}

	void	 StringIteratorScriptObjectZs_next(ScriptEngine *_se,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_se);
		si->next();
	}

	bool	 StringIteratorScriptObjectZs_end(ScriptEngine *_se,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_se);
		return si->end();
	}


}
