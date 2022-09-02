/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorScriptObjectWrap_get(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->get();
	}

	void	 StringIteratorScriptObjectWrap_next(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->next();
	}

	bool	 StringIteratorScriptObjectWrap_end(ZetScript *_zs,StringIteratorScriptObject *si){
		ZS_UNUSUED_PARAM(_zs);
		return si->end();
	}


}
