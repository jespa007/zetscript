/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectObjectClassIterator:public ScriptObject{

	public:

		ScriptObjectObjectClassIterator();

		static StackElement 	_getSf(ScriptObjectObjectClassIterator *si);
		static void				_nextSf(ScriptObjectObjectClassIterator *si);
		static bool				_endSf(ScriptObjectObjectClassIterator *si);

		StackElement _get();
		void _next();
		bool _end();

	};

}
