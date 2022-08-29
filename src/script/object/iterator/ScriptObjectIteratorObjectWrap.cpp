/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ScriptObjectIteratorObjectWrap_next(ZetScript *_zs,ScriptObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->next();
	}

	void	ScriptObjectIteratorObjectWrap_get(ZetScript *_zs,ScriptObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->get();
	}


	bool	 ScriptObjectIteratorObjectWrap_end(ZetScript *_zs,ScriptObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		return oi->end();
	}
}
