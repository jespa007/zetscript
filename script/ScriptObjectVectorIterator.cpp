/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVectorIterator::ScriptObjectVectorIterator(ZetScript *_zs):ScriptObject(_zs){

	}

	void	 ScriptObjectVectorIterator::_nextSf(ScriptObjectVectorIterator *si){
		si->_next();
	}

	void	 ScriptObjectVectorIterator::_next(){

	}


	bool	 ScriptObjectVectorIterator::_endSf(ScriptObjectVectorIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectVectorIterator::_end(){
		return false;
	}

}
