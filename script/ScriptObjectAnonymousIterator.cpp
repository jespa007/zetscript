/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectAnonymousIterator::ScriptObjectAnonymousIterator(ZetScript *_zs):ScriptObject(_zs){
	}


	void	 ScriptObjectAnonymousIterator::_nextSf(ScriptObjectAnonymousIterator *si){
		si->_next();
	}

	StackElement	ScriptObjectAnonymousIterator::_getSf(ScriptObjectAnonymousIterator *si){
		return si->_get();
	}


	bool	 ScriptObjectAnonymousIterator::_endSf(ScriptObjectAnonymousIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectAnonymousIterator::_end(){
		return false;
	}

	StackElement	 ScriptObjectAnonymousIterator::_get(){
		return {};
	}


	void	 ScriptObjectAnonymousIterator::_next(){

	}
}
