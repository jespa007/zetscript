/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ObjectIteratorScriptObjectWrap_next(ZetScript *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->next();
	}

	void	ObjectIteratorScriptObjectWrap_get(ZetScript *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->get();
	}


	bool	 ObjectIteratorScriptObjectWrap_end(ZetScript *_zs,ObjectIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		return oi->end();
	}
}
