/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	IteratorScriptObject::IteratorScriptObject(ZetScript *_zs):ScriptObjectBase(_zs){
	}


	void	 IteratorScriptObject::nextSf(IteratorScriptObject *si){
		si->next();
	}

	void	 IteratorScriptObject::next(){

	}


	bool	 IteratorScriptObject::endSf(IteratorScriptObject *si){
		return si->end();
	}

	bool	 IteratorScriptObject::end(){
		return false;
	}

}
