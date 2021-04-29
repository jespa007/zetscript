/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ScriptObjectObjectIteratorWrap_next(ScriptObjectObjectIterator *si){
		si->next();
	}

	StackElement	ScriptObjectObjectIteratorWrap_get(ScriptObjectObjectIterator *si){
		return si->get();
	}


	bool	 ScriptObjectObjectIteratorWrap_end(ScriptObjectObjectIterator *si){
		return si->end();
	}
}
