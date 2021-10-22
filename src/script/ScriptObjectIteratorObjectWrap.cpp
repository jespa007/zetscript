/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ScriptObjectIteratorObjectWrap_next(ScriptObjectIteratorObject *oi){
		oi->next();
	}

	void	ScriptObjectIteratorObjectWrap_get(ScriptObjectIteratorObject *oi){
		oi->get();
	}


	bool	 ScriptObjectIteratorObjectWrap_end(ScriptObjectIteratorObject *oi){
		return oi->end();
	}
}
