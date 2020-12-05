/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectIterator:public ScriptObject{

	public:

		ScriptObjectIterator(ZetScript *_zs);

		static StackElement 	_getSf(ScriptObjectIterator *si);
		static void				_nextSf(ScriptObjectIterator *si);
		static bool				_endSf(ScriptObjectIterator *si);



		StackElement _get();
		void _next();
		bool _end();

	};

}
