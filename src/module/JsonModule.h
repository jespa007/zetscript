/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class ObjectScriptObject;
	class StringScriptObject;

	namespace json{
		String serialize(ScriptEngine *_se, StackElement *_stk, bool _is_formatted, bool _strict_json_format=true);
	}

	void JsonModule_deserialize(ScriptEngine *_se,StringScriptObject *str_json);
	StringScriptObject * JsonModule_serialize(ScriptEngine *_se,StackElement *stk,bool *minimized);
	StringScriptObject * JsonModule_serialize(ScriptEngine *_se,StackElement *stk);
}
