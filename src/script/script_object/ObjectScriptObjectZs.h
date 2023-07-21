/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ObjectIteratorScriptObject;

	ArrayScriptObject *				ObjectScriptObjectZs_keys(ZetScript *_zs,ObjectScriptObject *o1);
	bool 							ObjectScriptObjectZs_contains(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	void 							ObjectScriptObjectZs_clear(ZetScript *_zs,ObjectScriptObject *o1);
	void 							ObjectScriptObjectZs_erase(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	ObjectIteratorScriptObject * 	ObjectScriptObjectZs_iter(ZetScript *_zs,ObjectScriptObject *oo);
	void						 	ObjectScriptObjectZs_extend(ZetScript *_zs,ObjectScriptObject *o1,ObjectScriptObject *o2);

}
