/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


void test_anonymous_scopes(zetscript::ScriptEngine *_script_engine, bool _show_print=true){


	_script_engine->compileAndRun("var a1=1\n");
	_script_engine->compileAndRun("{var b=0}\n");
	_script_engine->compileAndRun("var a2=2\n");
	_script_engine->compileAndRun("{var d=0; { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0; { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("var a3=3\n");
	_script_engine->compileAndRun("var a4=4\n");
	_script_engine->compileAndRun("var a5=5\n"
			 "{var d=0;  { var e=1}}\n"
			 "var a6=6\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0; { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");
	_script_engine->compileAndRun("{var d=0;  { var e=1}}\n");

	_script_engine->compileAndRun("var a7=7\n");
	_script_engine->compileAndRun("var a8=8\n");

	if(_show_print){
		_script_engine->printGeneratedCode();
	}


	// Check all vars ordered as at the top should have idx as the end of its symbol has (i.e a1, a2, a3)
	zetscript::ScriptScope *main_scope=_script_engine->getScriptScopesFactory()->getMainScope();
	for(int i=0; i < main_scope->symbol_variables->length(); i++){
		zetscript::Symbol *s=(zetscript::Symbol *)main_scope->symbol_variables->get(i);
		zetscript::String str_to_compare="a"+zetscript::Integer::toString(s->idx_position+1);
		if(_show_print) printf("%s %i\n",s->name.toConstChar(),(s->idx_position+1));
		if(s->name != str_to_compare){
			throw new std::runtime_error(
				zetscript::String::format(
						"Symbol are not ordered ('%s' != '%s')"
						,s->name.toConstChar()
						,str_to_compare.toConstChar()).toConstChar()
			);
		}

	}
}


void test_anonymous_scopes_no_print(zetscript::ScriptEngine *_script_engine){
	test_anonymous_scopes(_script_engine,false);
}

void test_consistency_function_override(zetscript::ScriptEngine *_script_engine, bool _show_print=true){

	_script_engine->compile(zetscript::String::format(
		"class A{\n"
		"	constructor(){\n"
		//		should call A::function1 of the instance of var is A
		//		should call B::function1 of the instance of var is B
		"       this.function1();\n"
		"	}\n"
		"	function1(){\n"
		"		if(%s){\n"
		"       Console::outln(\"from A \");\n"
		"		}\n"
		"	}\n"
		"};\n",_show_print?"true":"false")
	);

	_script_engine->compile(zetscript::String::format(
		"class B extends A{\n"
		"	constructor(){\n"
		"		super();\n"
		"	}\n"
		"	function1(){\n"
		"		super();\n"
		"		if(%s){\n"
		"       	Console::outln(\"from B \");\n"
		"		}\n	"
		"	}\n"
		"};\n"
		,_show_print?"true":"false")
	);

	// should print
	// from A
	// from B
	_script_engine->compileAndRun(
			"new B()"
	);
}

void test_consistency_function_override_no_print(zetscript::ScriptEngine *_script_engine){
	test_consistency_function_override(_script_engine, false);
}

#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;

	try{
		test_anonymous_scopes(&script_engine);
		test_consistency_function_override(&script_engine);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
	}
	return 0;
}
#endif

