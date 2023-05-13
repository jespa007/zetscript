/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorScriptObject;

	zs_int 							StringScriptObjectZs_length(ZetScript *_zs,StringScriptObject *_so);
	bool 							StringScriptObjectZs_contains(ZetScript *_zs,StringScriptObject *so, zs_string *_str);
	bool 							StringScriptObjectZs_contains(ZetScript *_zs,StringScriptObject *so, zs_int _ch);
	void 							StringScriptObjectZs_clear(ZetScript *_zs,StringScriptObject *_so);
	StringScriptObject *			StringScriptObjectZs_replace(ZetScript *_zs,StringScriptObject *str_in,zs_string *str_old, zs_string *_str_new);
	void 							StringScriptObjectZs_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int _idx);
	void 							StringScriptObjectZs_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int _ch);
	void 							StringScriptObjectZs_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_string *_str);
	ArrayScriptObject * 			StringScriptObjectZs_split(ZetScript *_zs,StringScriptObject *so,zs_int _ch_delim);
	ArrayScriptObject * 			StringScriptObjectZs_split(ZetScript *_zs,StringScriptObject *so,zs_string * _str_token);

	zs_int 							StringScriptObjectZs_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search);
	zs_int 							StringScriptObjectZs_indexOf(ZetScript *_zs,StringScriptObject *so,zs_string *search);
	bool 							StringScriptObjectZs_startsWith(ZetScript *_zs,StringScriptObject *so,zs_string *prefix);
	bool 							StringScriptObjectZs_endsWith(ZetScript *_zs,StringScriptObject *so,zs_string *suffix);

	void 							StringScriptObjectZs_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *_str_append);
	void 							StringScriptObjectZs_append(ZetScript *_zs,StringScriptObject *str_in,zs_int _ch);

	StringScriptObject * 			StringScriptObjectZs_toUpperCase(ZetScript *_zs,StringScriptObject *so);
	StringScriptObject * 			StringScriptObjectZs_toLowerCase(ZetScript *_zs,StringScriptObject *so);

	StringScriptObject * 			StringScriptObjectZs_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index, zs_int _end_index);
	StringScriptObject * 			StringScriptObjectZs_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index);


	StringIteratorScriptObject *	StringScriptObjectZs_iter(ZetScript *_zs,StringScriptObject *_str_in);


}
