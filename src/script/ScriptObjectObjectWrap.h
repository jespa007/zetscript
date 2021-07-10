/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	class	ScriptObjectObjectIterator;

	ScriptObjectVector *			ScriptObjectObjectWrap_keys(ScriptObjectObject *o1);
	bool 							ScriptObjectObjectWrap_contains(ScriptObjectObject *o1, std::string * key);
	void 							ScriptObjectObjectWrap_clear(ScriptObjectObject *o1);
	void 							ScriptObjectObjectWrap_erase(ScriptObjectObject *o1, std::string * key);
	ScriptObjectObjectIterator * 	ScriptObjectObjectWrap_iter(ScriptObjectObject *oo);


}
