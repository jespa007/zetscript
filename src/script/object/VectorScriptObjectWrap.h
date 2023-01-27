/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	VectorIteratorScriptObject;

	void    						VectorScriptObjectWrap_push(ZetScript *_zs,VectorScriptObject *sv,StackElement  * stk);
	void   							VectorScriptObjectWrap_pop(ZetScript *_zs,VectorScriptObject *sv);
	zs_int 							VectorScriptObjectWrap_length(ZetScript *_zs,VectorScriptObject *sv);
	void 							VectorScriptObjectWrap_insertAt(ZetScript *_zs,VectorScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							VectorScriptObjectWrap_eraseAt(ZetScript *_zs,VectorScriptObject *sv, zs_int idx);
	void 							VectorScriptObjectWrap_clear(ZetScript *_zs,VectorScriptObject *sv);
	StringScriptObject			*	VectorScriptObjectWrap_join(ZetScript *_zs,VectorScriptObject *sv, zs_int idx_char);
	VectorIteratorScriptObject 	* 	VectorScriptObjectWrap_iter(ZetScript *_zs,VectorScriptObject *so);
	bool 							VectorScriptObjectWrap_contains(ZetScript *_zs,VectorScriptObject *so, StackElement *stk);
	VectorScriptObject 			*	VectorScriptObjectWrap_concat_static(ZetScript *_zs,VectorScriptObject *_so1, VectorScriptObject *_so2);
	void 							VectorScriptObjectWrap_concat(ZetScript *_zs,VectorScriptObject *_so1, VectorScriptObject *_so2);
	bool 							VectorScriptObjectWrap_equal(ZetScript *_zs,VectorScriptObject *so, VectorScriptObject *stk);

}
