/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorVectorWrap_get(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		vi->get();
	}

	void	 ScriptObjectIteratorVectorWrap_next(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		vi->next();
	}

	bool	 ScriptObjectIteratorVectorWrap_end(ZetScript *_zs,ScriptObjectIteratorVector *vi){
		return vi->end();
	}
}
