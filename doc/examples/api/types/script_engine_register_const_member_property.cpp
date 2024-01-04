#include "zetscript.h"

class Number{
//...
};

zetscript::zs_float NumberZs_MAX_VALUE(
	zetscript::ScriptEngine *_script_engine
){
	ZS_UNUSUED_PARAM(_script_engine);
	return FLT_MAX;
}


int main(){
	zetscript::ScriptEngine script_engine;

	script_engine.registerType<Number>("Number");

	script_engine.registerConstMemberProperty<Number>("MAX_VALUE",NumberZs_MAX_VALUE);

	script_engine.compileAndRun(

		"Console::outln(\"Number::MAX_VALUE => {0}\",Number::MAX_VALUE);"
	);

	return 0;
}