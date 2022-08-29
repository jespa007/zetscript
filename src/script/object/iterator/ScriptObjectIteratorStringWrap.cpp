/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorStringWrap_get(ZetScript *_zs,ScriptObjectIteratorString *si){
		ZS_UNUSUED_PARAM(_zs);
		si->get();
	}

	void	 ScriptObjectIteratorStringWrap_next(ZetScript *_zs,ScriptObjectIteratorString *si){
		ZS_UNUSUED_PARAM(_zs);
		si->next();
	}

	bool	 ScriptObjectIteratorStringWrap_end(ZetScript *_zs,ScriptObjectIteratorString *si){
		ZS_UNUSUED_PARAM(_zs);
		return si->end();
	}


}
