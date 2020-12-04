/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectVectorIterator:public ScriptObject{

	public:

		ScriptObjectVectorIterator(ZetScript *_zs);

		static StackElement 	_getSf(ScriptObjectVectorIterator *si);
		static void				_nextSf(ScriptObjectVectorIterator *si);
		static bool				endSf(ScriptObjectVectorIterator *si);



		StackElement _get();
		void _next();
		bool end();

	};

}
