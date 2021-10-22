/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectIteratorVectorWrap_get(ScriptObjectIteratorVector *vi){
		vi->get();
	}

	void	 ScriptObjectIteratorVectorWrap_next(ScriptObjectIteratorVector *vi){
		vi->next();
	}

	bool	 ScriptObjectIteratorVectorWrap_end(ScriptObjectIteratorVector *vi){
		return vi->end();
	}
}
