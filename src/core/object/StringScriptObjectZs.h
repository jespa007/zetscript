/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorScriptObject;

	zs_int 							StringScriptObjectZs_length(ScriptEngine *_script_engine,StringScriptObject *_so);
	bool 							StringScriptObjectZs_contains(ScriptEngine *_script_engine,StringScriptObject *so, String *_str);
	bool 							StringScriptObjectZs_contains(ScriptEngine *_script_engine,StringScriptObject *so, zs_int _ch);
	void 							StringScriptObjectZs_clear(ScriptEngine *_script_engine,StringScriptObject *_so);
	StringScriptObject *			StringScriptObjectZs_replace(ScriptEngine *_script_engine,StringScriptObject *str_in,String *str_old, String *_str_new);
	void 							StringScriptObjectZs_eraseAt(ScriptEngine *_script_engine,StringScriptObject *so, zs_int _idx);
	void 							StringScriptObjectZs_insertAt(ScriptEngine *_script_engine,StringScriptObject *so, zs_int idx,zs_int _ch);
	void 							StringScriptObjectZs_insertAt(ScriptEngine *_script_engine,StringScriptObject *so, zs_int idx,String *_str);
	ArrayScriptObject * 			StringScriptObjectZs_split(ScriptEngine *_script_engine,StringScriptObject *so,zs_int _ch_delim);
	ArrayScriptObject * 			StringScriptObjectZs_split(ScriptEngine *_script_engine,StringScriptObject *so,String * _str_token);

	zs_int 							StringScriptObjectZs_indexOf(ScriptEngine *_script_engine,StringScriptObject *so,zs_int search);
	zs_int 							StringScriptObjectZs_indexOf(ScriptEngine *_script_engine,StringScriptObject *so,String *search);
	bool 							StringScriptObjectZs_startsWith(ScriptEngine *_script_engine,StringScriptObject *so,String *prefix);
	bool 							StringScriptObjectZs_endsWith(ScriptEngine *_script_engine,StringScriptObject *so,String *suffix);

	void 							StringScriptObjectZs_append(ScriptEngine *_script_engine,StringScriptObject *str_in,StringScriptObject *_str_append);
	void 							StringScriptObjectZs_append(ScriptEngine *_script_engine,StringScriptObject *str_in,zs_int _ch);

	StringScriptObject * 			StringScriptObjectZs_toUpperCase(ScriptEngine *_script_engine,StringScriptObject *so);
	StringScriptObject * 			StringScriptObjectZs_toLowerCase(ScriptEngine *_script_engine,StringScriptObject *so);

	StringScriptObject * 			StringScriptObjectZs_substring(ScriptEngine *_script_engine,StringScriptObject *_so, zs_int _start_index, zs_int _end_index);
	StringScriptObject * 			StringScriptObjectZs_substring(ScriptEngine *_script_engine,StringScriptObject *_so, zs_int _start_index);


	StringIteratorScriptObject *	StringScriptObjectZs_iter(ScriptEngine *_script_engine,StringScriptObject *_str_in);


}
