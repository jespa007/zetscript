/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectVectorIteratorWrap_get(ScriptObjectVectorIterator *vi){
		vi->get();
	}

	void	 ScriptObjectVectorIteratorWrap_next(ScriptObjectVectorIterator *vi){
		vi->next();
	}

	bool	 ScriptObjectVectorIteratorWrap_end(ScriptObjectVectorIterator *vi){
		return vi->end();
	}
}
