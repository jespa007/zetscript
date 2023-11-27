/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class DictionaryObject;
	class StringObject;

	namespace json{
		String serialize(ZetScript *_zs, StackElement *_stk, bool _is_formatted, bool _strict_json_format=true);
	}

	void JsonModule_deserialize(ZetScript *_zs,StringObject *str_json);
	StringObject * JsonModule_serialize(ZetScript *_zs,StackElement *stk,bool *minimized);
	StringObject * JsonModule_serialize(ZetScript *_zs,StackElement *stk);
}
