/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObjectZs_push(ScriptEngine *_script_engine,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObjectZs_pop(ScriptEngine *_script_engine,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObjectZs_length(ScriptEngine *_script_engine,ArrayScriptObject *sv);
	void 							ArrayScriptObjectZs_insertAt(ScriptEngine *_script_engine,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObjectZs_eraseAt(ScriptEngine *_script_engine,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObjectZs_clear(ScriptEngine *_script_engine,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObjectZs_join(ScriptEngine *_script_engine,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObjectZs_iter(ScriptEngine *_script_engine,ArrayScriptObject *so);
	bool 							ArrayScriptObjectZs_contains(ScriptEngine *_script_engine,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObjectZs_concat(ScriptEngine *_script_engine,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObjectZs_extend(ScriptEngine *_script_engine,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObjectZs_equal(ScriptEngine *_script_engine,ArrayScriptObject *so, ArrayScriptObject *stk);

}
