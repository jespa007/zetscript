/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectVectorIterator::ScriptObjectVectorIterator(ZetScript *_zs):ScriptObjectBase(_zs){

	}

	void   ScriptObjectVectorIterator::pushSf(ScriptObjectVectorIterator *si,StackElement  * stk){
		si->push(stk);
	}


	void	 ScriptObjectVectorIterator::nextSf(ScriptObjectVectorIterator *si){
		si->next();
	}

	void	 ScriptObjectVectorIterator::next(){

	}


	bool	 ScriptObjectVectorIterator::endSf(ScriptObjectVectorIterator *si){
		return si->end();
	}

	bool	 ScriptObjectVectorIterator::end(){
		return false;
	}

}
