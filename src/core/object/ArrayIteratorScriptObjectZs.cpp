/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ArrayIteratorObjectZs_get(ScriptEngine *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->get();
	}

	void	 ArrayIteratorObjectZs_next(ScriptEngine *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->next();
	}

	bool	 ArrayIteratorObjectZs_end(ScriptEngine *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		return vi->end();
	}
}
