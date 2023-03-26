/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	template<typename _T>
	_T	ObjectScriptObject::get(const zs_string & _key){
		StackElement *stk;
		if((stk=getStackElement(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.c_str());
		}

		return this->zs->stackElementTo<_T>(stk);
	}
}
