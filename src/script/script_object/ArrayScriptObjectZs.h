/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObjectZs_push(ZetScript *_zs,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObjectZs_pop(ZetScript *_zs,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObjectZs_length(ZetScript *_zs,ArrayScriptObject *sv);
	void 							ArrayScriptObjectZs_insertAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObjectZs_eraseAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObjectZs_clear(ZetScript *_zs,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObjectZs_join(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObjectZs_iter(ZetScript *_zs,ArrayScriptObject *so);
	bool 							ArrayScriptObjectZs_contains(ZetScript *_zs,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObjectZs_concat(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObjectZs_extend(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObjectZs_equal(ZetScript *_zs,ArrayScriptObject *so, ArrayScriptObject *stk);

}
