/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectAnonymousClassIterator:public ScriptObject{

	public:

		ScriptObjectAnonymousClassIterator();

		static StackElement 	_getSf(ScriptObjectAnonymousClassIterator *si);
		static void				_nextSf(ScriptObjectAnonymousClassIterator *si);
		static bool				_endSf(ScriptObjectAnonymousClassIterator *si);

		StackElement _get();
		void _next();
		bool _end();

	};

}
