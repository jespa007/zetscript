/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectObjectIterator::ScriptObjectObjectIterator(){
	}


	void	 ScriptObjectObjectIterator::_nextSf(ScriptObjectObjectIterator *si){
		si->_next();
	}

	StackElement	ScriptObjectObjectIterator::_getSf(ScriptObjectObjectIterator *si){
		return si->_get();
	}


	bool	 ScriptObjectObjectIterator::_endSf(ScriptObjectObjectIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectObjectIterator::_end(){
		return false;
	}

	StackElement	 ScriptObjectObjectIterator::_get(){
		return {};
	}


	void	 ScriptObjectObjectIterator::_next(){

	}
}
