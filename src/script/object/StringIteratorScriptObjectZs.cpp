/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorScriptObject_get(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->get();
	}

	void	 StringIteratorScriptObject_next(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->next();
	}

	bool	 StringIteratorScriptObject_end(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		return si->end();
	}


}
