#include "zetscript.h"

// C++ class to be registered
class Number{
public:
	float value;

	Number(){
		value=0;
	}
};

// defines new function for Number object
Number *NumberZs_new(
	zetscript::ScriptEngine *_script_engine
){
	return new  Number();
}

// defines new function for Number constructor
void NumberZs_constructor(
	zetscript::ScriptEngine *_script_engine
	,Number *_this
	,zetscript::zs_float * _value
){
	printf("Constructor value : %.02f\n",*_value);
	_this->value=*_value;
}

// defines delete function for Number object
void NumberZs_delete(
	zetscript::ScriptEngine *_script_engine
	, Number *_this
){
	delete _this;
}


int main(){
	zetscript::ScriptEngine script_engine;

	// Register class Number as instanciable
	script_engine.registerType<Number>("Number",NumberZs_new,NumberZs_delete);

	// Register class Number constructor
	script_engine.registerConstructor<Number>(NumberZs_constructor);

    // Prints the result of the evaluated script by console
    script_engine.compileAndRun(
        "var n=new Number(20.5);"
 	);

    return 0;
}