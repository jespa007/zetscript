#pragma once

namespace zetscript{

	namespace eval{

		bool 	eval_set_instruction_static_symbol(Instruction * instruction, Symbol *static_symbol,std::string & static_error);
		void 	eval(ZetScript *zs,const char * str, const char *  _filename=NULL, int _line=1);
	}
}
