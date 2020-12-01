/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIterator::ScriptObjectIterator(ZetScript *_zs):ScriptObjectBase{
	}



	void	 ScriptObjectIterator::nextSf(ScriptObjectIterator *si){
		si->next();
	}

	void	 ScriptObjectIterator::next(){

	}


	bool	 ScriptObjectIterator::endSf((ScriptObjectIterator *si){
		return si->end();
	}

	bool	 ScriptObjectIterator::end(){
		return false;
	}

}
