/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;

	namespace json{
		zs_string serialize(ZetScript *_zs, StackElement *stk, bool is_formatted);
	}

	void JsonModule_deserialize(ZetScript *_zs,ScriptObjectString *str_json);
	ScriptObjectString * JsonModule_serialize(ZetScript *_zs,StackElement *stk,bool *minimized);
	ScriptObjectString * JsonModule_serialize(ZetScript *_zs,StackElement *stk);
}
