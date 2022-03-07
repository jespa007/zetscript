/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

void test_anonymous_scopes(
		zetscript::ZetScript *_zs
		){
	_zs->eval("var a1=1\n");
	_zs->eval("{var b=0}\n");
	_zs->eval("var a2=2\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("var a3=3\n");
	_zs->eval("var a4=4\n");
	_zs->eval("var a5=5\n"
			 "{var d=0 { var e=1}}\n"
			 "var a6=6\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");
	_zs->eval("{var d=0 { var e=1}}\n");

	_zs->eval("var a7=7\n");
	_zs->eval("var a8=8\n",zetscript::EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE);


	// Check 1st: all vars at the top should have idx as the end of its symbol has (i.e a1, a2, a3)
	zetscript::Scope *main_scope=_zs->getScopeFactory()->getMainScope();
	for(int i=0; i < main_scope->symbol_variables->count; i++){
		zetscript::Symbol *s=(zetscript::Symbol *)main_scope->symbol_variables->items[i];
		zetscript::zs_string str_to_compare="a"+zetscript::zs_strutils::zs_int_to_str(s->idx_position+1);
		printf("%s %i\n",s->name.c_str(),(s->idx_position+1));
		if(s->name != str_to_compare){
			throw new std::runtime_error(zetscript::zs_strutils::format("Symbol are not ordered ('%s' != '%s')",s->name.c_str(),str_to_compare.c_str()).c_str());
		}

	}

	// Should have only one scope


}


void test_consistency_function_override(
	zetscript::ZetScript *_zs
){

	_zs->eval(
		"class A{\n"
		"	constructor(){\n"
		//		should call A::function1 of the instance of var is A
		//		should call B::function1 of the instance of var is B
		"       this.function1();\n"
		"	}\n"
		"	function1(){\n"
		"       Console::outln(\"from A \");\n"
		"	}\n"
		"};\n"
	);

	_zs->eval(
		"class B extends A{\n"
		"	constructor(){\n"
		"		super();\n"
		"	}\n"
		"	function1(){\n"
		"		super();\n"
		"       Console::outln(\"from B \");\n"
		"	}\n"
		"};\n"
	);

	// should print
	// from A
	// from B
	_zs->eval(
			"new B()"
	);

}


int main(){

	// instances ZetScript
	zetscript::ZetScript zs;

	try{

		test_consistency_function_override(&zs);
		test_anonymous_scopes(&zs);

	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}

}