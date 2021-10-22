/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ScriptObjectIteratorObject;

	ScriptObjectVector *			ScriptObjectObjectWrap_keys(ScriptObjectObject *o1);
	bool 							ScriptObjectObjectWrap_contains(ScriptObjectObject *o1, zs_string * key);
	void 							ScriptObjectObjectWrap_clear(ScriptObjectObject *o1);
	void 							ScriptObjectObjectWrap_erase(ScriptObjectObject *o1, zs_string * key);
	ScriptObjectIteratorObject * 	ScriptObjectObjectWrap_iter(ScriptObjectObject *oo);


}
