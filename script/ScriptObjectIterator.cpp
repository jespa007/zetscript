/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIterator::ScriptObjectIterator(ZetScript *_zs):ScriptObject(_zs){
	}


	void	 ScriptObjectIterator::_nextSf(ScriptObjectIterator *si){
		si->_next();
	}

	StackElement	ScriptObjectIterator::_getSf(ScriptObjectIterator *si){
		return si->_get();
	}


	bool	 ScriptObjectIterator::endSf(ScriptObjectIterator *si){
		return si->end();
	}

	bool	 ScriptObjectIterator::end(){
		return false;
	}

	StackElement	 ScriptObjectIterator::_get(){
		return {};
	}


	void	 ScriptObjectIterator::_next(){

	}
}
