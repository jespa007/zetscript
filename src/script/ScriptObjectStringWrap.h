/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	ScriptObjectIteratorString;

	zs_int 							ScriptObjectStringWrap_size(ZetScript *_zs,ScriptObjectString *so);
	bool 							ScriptObjectStringWrap_contains(ZetScript *_zs,ScriptObjectString *so, std::string *str);
	bool 							ScriptObjectStringWrap_contains(ZetScript *_zs,ScriptObjectString *so, zs_int ch);
	void 							ScriptObjectStringWrap_clear(ZetScript *_zs,ScriptObjectString *so);
	ScriptObjectString *			ScriptObjectStringWrap_replace(ZetScript *_zs,ScriptObjectString *str_in,std::string *str_old, std::string *str_new);
	void 							ScriptObjectStringWrap_eraseAt(ZetScript *_zs,ScriptObjectString *so, zs_int idx);
	void 							ScriptObjectStringWrap_insertAt(ZetScript *_zs,ScriptObjectString *so, zs_int idx,zs_int ch);
	ScriptObjectVector * 			ScriptObjectStringWrap_split(ZetScript *_zs,ScriptObjectString *so,zs_int ch_delim);
	ScriptObjectVector * 			ScriptObjectStringWrap_split(ZetScript *_zs,ScriptObjectString *so,std::string * str_token);

	zs_int 							ScriptObjectStringWrap_indexOf(ZetScript *_zs,ScriptObjectString *so,zs_int search);
	zs_int 							ScriptObjectStringWrap_indexOf(ZetScript *_zs,ScriptObjectString *so,std::string *search);
	bool 							ScriptObjectStringWrap_startsWith(ZetScript *_zs,ScriptObjectString *so,std::string *prefix);
	bool 							ScriptObjectStringWrap_endsWith(ZetScript *_zs,ScriptObjectString *so,std::string *suffix);
	ScriptObjectString * 			ScriptObjectStringWrap_substring(ZetScript *_zs,ScriptObjectString *str_in,zs_int start,zs_int end);

	void 							ScriptObjectStringWrap_append(ZetScript *_zs,ScriptObjectString *str_in,ScriptObjectString *str_append);

	ScriptObjectIteratorString *	ScriptObjectStringWrap_iter(ZetScript *_zs,ScriptObjectString *str_in);


}
