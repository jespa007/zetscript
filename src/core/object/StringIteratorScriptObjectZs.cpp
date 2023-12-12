/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorScriptObjectZs_get(ScriptEngine *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->get();
	}

	void	 StringIteratorScriptObjectZs_next(ScriptEngine *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->next();
	}

	bool	 StringIteratorScriptObjectZs_end(ScriptEngine *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		return si->end();
	}


}
