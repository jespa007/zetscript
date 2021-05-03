/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	class	ScriptObjectVectorIterator;

	void    						ScriptObjectVectorWrap_push(ScriptObjectVector *sv,StackElement  * stk);
	void   							ScriptObjectVectorWrap_pop(ScriptObjectVector *sv);
	zs_int 							ScriptObjectVectorWrap_size(ScriptObjectVector *sv);
	void 							ScriptObjectVectorWrap_insertAt(ScriptObjectVector *sv, zs_int idx,StackElement  * stk);
	void 							ScriptObjectVectorWrap_eraseAt(ScriptObjectVector *sv, zs_int idx);
	void 							ScriptObjectVectorWrap_clear(ScriptObjectVector *sv);
	ScriptObjectString			*	ScriptObjectVectorWrap_join(ScriptObjectVector *sv, zs_int idx_char);
	ScriptObjectVectorIterator 	* 	ScriptObjectVectorWrap_iter(ScriptObjectVector *so);

}
