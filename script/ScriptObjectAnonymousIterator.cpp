/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectAnonymousClassIterator::ScriptObjectAnonymousClassIterator(){
	}


	void	 ScriptObjectAnonymousClassIterator::_nextSf(ScriptObjectAnonymousClassIterator *si){
		si->_next();
	}

	StackElement	ScriptObjectAnonymousClassIterator::_getSf(ScriptObjectAnonymousClassIterator *si){
		return si->_get();
	}


	bool	 ScriptObjectAnonymousClassIterator::_endSf(ScriptObjectAnonymousClassIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectAnonymousClassIterator::_end(){
		return false;
	}

	StackElement	 ScriptObjectAnonymousClassIterator::_get(){
		return {};
	}


	void	 ScriptObjectAnonymousClassIterator::_next(){

	}
}
