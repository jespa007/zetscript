/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ArrayIteratorScriptObjectWrap_get(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->get();
	}

	void	 ArrayIteratorScriptObjectWrap_next(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->next();
	}

	bool	 ArrayIteratorScriptObjectWrap_end(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		return vi->end();
	}
}
