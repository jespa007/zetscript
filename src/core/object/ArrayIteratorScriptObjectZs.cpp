/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ArrayIteratorObjectZs_get(ScriptEngine *_se,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_se);
		vi->get();
	}

	void	 ArrayIteratorObjectZs_next(ScriptEngine *_se,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_se);
		vi->next();
	}

	bool	 ArrayIteratorObjectZs_end(ScriptEngine *_se,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_se);
		return vi->end();
	}
}
