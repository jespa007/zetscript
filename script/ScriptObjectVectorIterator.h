/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectIterator{

	public:

		ScriptObjectIterator(ScriptObjectVector *sov);

		static StackElement 	_getSf(ScriptObjectIterator *si);
		static void				_nextSf(ScriptObjectIterator *si);
		static bool				endSf();

	protected:

		StackElement get();
		void next();
		bool end();

	};

}
