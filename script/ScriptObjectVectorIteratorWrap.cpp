/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	void	 ScriptObjectVectorIteratorWrap_next(ScriptObjectVectorIterator *si){
		si->_next();
	}

	bool	 ScriptObjectVectorIteratorWrap_end(ScriptObjectVectorIterator *si){
		return si->_end();
	}
}
