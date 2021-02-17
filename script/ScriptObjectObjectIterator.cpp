/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectObjectClassIterator::ScriptObjectObjectClassIterator(){
	}


	void	 ScriptObjectObjectClassIterator::_nextSf(ScriptObjectObjectClassIterator *si){
		si->_next();
	}

	StackElement	ScriptObjectObjectClassIterator::_getSf(ScriptObjectObjectClassIterator *si){
		return si->_get();
	}


	bool	 ScriptObjectObjectClassIterator::_endSf(ScriptObjectObjectClassIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectObjectClassIterator::_end(){
		return false;
	}

	StackElement	 ScriptObjectObjectClassIterator::_get(){
		return {};
	}


	void	 ScriptObjectObjectClassIterator::_next(){

	}
}
