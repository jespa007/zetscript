/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectIterator:public ScriptObjectBase{

	public:

		ScriptObjectIterator(ScriptObject *so);

		static StackElement 	_getSf();
		static void				_nextSf();
		static bool				endSf();

	protected:

		StackElement _get();
		void _inc();
		bool end();

	};

}
