/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ScriptObjectObjectIteratorWrap_next(ScriptObjectObjectIterator *oi){
		oi->next();
	}

	void	ScriptObjectObjectIteratorWrap_get(ScriptObjectObjectIterator *oi){
		oi->get();
	}


	bool	 ScriptObjectObjectIteratorWrap_end(ScriptObjectObjectIterator *oi){
		return oi->end();
	}
}
