/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectAnonymousIterator:public ScriptObject{

	public:

		ScriptObjectAnonymousIterator(ZetScript *_zs);

		static StackElement 	_getSf(ScriptObjectAnonymousIterator *si);
		static void				_nextSf(ScriptObjectAnonymousIterator *si);
		static bool				_endSf(ScriptObjectAnonymousIterator *si);



		StackElement _get();
		void _next();
		bool _end();

	};

}
