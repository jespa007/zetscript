/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectStringIterator:public ScriptObject{

	public:

		ScriptObjectStringIterator(ZetScript *_zs);

		static StackElement 	_getSf(ScriptObjectStringIterator *si);
		static void				_nextSf(ScriptObjectStringIterator *si);
		static bool				endSf(ScriptObjectStringIterator *si);


		StackElement _get();
		void _next();
		bool end();

	};

}
