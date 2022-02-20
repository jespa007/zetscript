/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorStringWrap_get(ZetScript *_zs,ScriptObjectIteratorString *si){
		si->get();
	}

	void	 ScriptObjectIteratorStringWrap_next(ZetScript *_zs,ScriptObjectIteratorString *si){
		si->next();
	}

	bool	 ScriptObjectIteratorStringWrap_end(ZetScript *_zs,ScriptObjectIteratorString *si){
		return si->end();
	}


}
