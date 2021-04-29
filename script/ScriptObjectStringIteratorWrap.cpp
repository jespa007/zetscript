/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectStringIteratorWrap_next(ScriptObjectStringIterator *si){
		si->next();
	}


	bool	 ScriptObjectStringIteratorWrap_end(ScriptObjectStringIterator *si){
		return si->end();
	}


}
