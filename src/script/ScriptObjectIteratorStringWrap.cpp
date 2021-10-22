/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorStringWrap_get(ScriptObjectIteratorString *si){
		si->get();
	}

	void	 ScriptObjectIteratorStringWrap_next(ScriptObjectIteratorString *si){
		si->next();
	}

	bool	 ScriptObjectIteratorStringWrap_end(ScriptObjectIteratorString *si){
		return si->end();
	}


}
