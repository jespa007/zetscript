/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	ScriptObjectIteratorString;

	zs_int 							ScriptObjectStringWrap_size(ScriptObjectString *so);
	bool 							ScriptObjectStringWrap_contains(ScriptObjectString *so, zs_string *str);
	bool 							ScriptObjectStringWrap_contains(ScriptObjectString *so, zs_int ch);
	void 							ScriptObjectStringWrap_clear(ScriptObjectString *so);
	ScriptObjectString *			ScriptObjectStringWrap_replace(ScriptObjectString *str_in,zs_string *str_old, zs_string *str_new);
	void 							ScriptObjectStringWrap_eraseAt(ScriptObjectString *so, zs_int idx);
	void 							ScriptObjectStringWrap_insertAt(ScriptObjectString *so, zs_int idx,zs_int ch);
	ScriptObjectVector * 			ScriptObjectStringWrap_split(ScriptObjectString *so,zs_int ch_delim);
	ScriptObjectVector * 			ScriptObjectStringWrap_split(ScriptObjectString *so,zs_string * str_token);

	zs_int 							ScriptObjectStringWrap_indexOf(ScriptObjectString *so,zs_int search);
	zs_int 							ScriptObjectStringWrap_indexOf(ScriptObjectString *so,zs_string *search);
	bool 							ScriptObjectStringWrap_startsWith(ScriptObjectString *so,zs_string *prefix);
	bool 							ScriptObjectStringWrap_endsWith(ScriptObjectString *so,zs_string *suffix);
	ScriptObjectString * 			ScriptObjectStringWrap_substring(ScriptObjectString *str_in,zs_int start,zs_int end);

	void 							ScriptObjectStringWrap_append(ScriptObjectString *str_in,ScriptObjectString *str_append);

	ScriptObjectIteratorString *	ScriptObjectStringWrap_iter(ScriptObjectString *str_in);


}
