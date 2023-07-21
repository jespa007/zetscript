/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ArrayIteratorScriptObjectZs_get(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->get();
	}

	void	 ArrayIteratorScriptObjectZs_next(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->next();
	}

	bool	 ArrayIteratorScriptObjectZs_end(ZetScript *_zs,ArrayIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		return vi->end();
	}
}
