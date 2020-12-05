/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectStringIterator::ScriptObjectStringIterator(ZetScript *_zs):ScriptObject(_zs){
	}



	void	 ScriptObjectStringIterator::_nextSf(ScriptObjectStringIterator *si){
		si->_next();
	}

	void	 ScriptObjectStringIterator::_next(){

	}


	bool	 ScriptObjectStringIterator::_endSf(ScriptObjectStringIterator *si){
		return si->_end();
	}

	bool	 ScriptObjectStringIterator::_end(){
		return false;
	}

}
