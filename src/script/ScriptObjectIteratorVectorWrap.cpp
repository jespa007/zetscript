/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorVectorWrap_get(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->get();
	}

	void	 ScriptObjectIteratorVectorWrap_next(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->next();
	}

	bool	 ScriptObjectIteratorVectorWrap_end(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		ZS_UNUSUED_PARAM(_zs);
		return vi->end();
	}
}
