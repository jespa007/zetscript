/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ObjectIteratorScriptObject;

	ArrayScriptObject *			ObjectScriptObject_keys(ZetScript *_zs,ObjectScriptObject *o1);
	bool 							ObjectScriptObject_contains(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	void 							ObjectScriptObject_clear(ZetScript *_zs,ObjectScriptObject *o1);
	void 							ObjectScriptObject_erase(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	ObjectIteratorScriptObject * 	ObjectScriptObject_iter(ZetScript *_zs,ObjectScriptObject *oo);
	void						 	ObjectScriptObject_extend(ZetScript *_zs,ObjectScriptObject *o1,ObjectScriptObject *o2);

}
