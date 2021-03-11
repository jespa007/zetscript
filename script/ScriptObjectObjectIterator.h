/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectObjectIterator:public ScriptObject{

	public:

		ScriptObjectObjectIterator();

		static StackElement 	_getSf(ScriptObjectObjectIterator *si);
		static void				_nextSf(ScriptObjectObjectIterator *si);
		static bool				_endSf(ScriptObjectObjectIterator *si);

		StackElement _get();
		void _next();
		bool _end();

	};

}
