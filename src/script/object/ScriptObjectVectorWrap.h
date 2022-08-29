/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ScriptObjectIteratorVector;

	void    						ScriptObjectVectorWrap_push(ZetScript *_zs,ScriptObjectVector *sv,StackElement  * stk);
	void   							ScriptObjectVectorWrap_pop(ZetScript *_zs,ScriptObjectVector *sv);
	zs_int 							ScriptObjectVectorWrap_size(ZetScript *_zs,ScriptObjectVector *sv);
	void 							ScriptObjectVectorWrap_insertAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx,StackElement  * stk);
	void 							ScriptObjectVectorWrap_eraseAt(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx);
	void 							ScriptObjectVectorWrap_clear(ZetScript *_zs,ScriptObjectVector *sv);
	ScriptObjectString			*	ScriptObjectVectorWrap_join(ZetScript *_zs,ScriptObjectVector *sv, zs_int idx_char);
	ScriptObjectIteratorVector 	* 	ScriptObjectVectorWrap_iter(ZetScript *_zs,ScriptObjectVector *so);
	bool 							ScriptObjectVectorWrap_contains(ZetScript *_zs,ScriptObjectVector *so, StackElement *stk);
	bool 							ScriptObjectVectorWrap_equal(ZetScript *_zs,ScriptObjectVector *so, ScriptObjectVector *stk);

}
