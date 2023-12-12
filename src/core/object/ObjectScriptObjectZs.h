/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ObjectIteratorScriptObject;

	ArrayScriptObject *				ObjectScriptObjectZs_keys(ScriptEngine *_se,ObjectScriptObject *o1);
	bool 							ObjectScriptObjectZs_contains(ScriptEngine *_se,ObjectScriptObject *o1, String * key);
	void 							ObjectScriptObjectZs_clear(ScriptEngine *_se,ObjectScriptObject *o1);
	void 							ObjectScriptObjectZs_erase(ScriptEngine *_se,ObjectScriptObject *o1, String * key);
	ObjectIteratorScriptObject * 	ObjectScriptObjectZs_iter(ScriptEngine *_se,ObjectScriptObject *oo);
	void						 	ObjectScriptObjectZs_extend(ScriptEngine *_se,ObjectScriptObject *o1,ObjectScriptObject *o2);

}
