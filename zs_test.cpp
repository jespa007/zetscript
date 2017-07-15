#include "CZetScript.h"

using namespace zetscript;


#define TEST_ARITHMETIC_INT_OP(val1, op, val2) \
{ \
	int aux_value=0; \
	string str= STR(val1) \
				STR(op) \
				STR(val2) \
				";"; \
	if((aux_value=CZetScript::eval<int>(str)) != (val1 op val2)){ \
		fprintf(stderr,"error test \"%s\" expected %i but it was %i!",str.c_str(),val1 op val2,aux_value); \
		exit(-1); \
	} \
}

int main(int argc, char * argv[]) {

	int aux_value;

	CZetScript *zet_script = CZetScript::getInstance();


/*	"test_arithmetic_operations.zs",
	"test_binary_operations.zs",
	"test_class.zs",
	"test_for.zs",
	"test_function.zs",
	"test_if_else.zs",
	"test_logic_operations.zs",
	"test_metamethod.zs",
	"test_recursion.zs",
	"test_struct.zs",
	"test_switch.zs",
	"test_ternari.zs",
	"test_vector.zs",
	"test_while.zs"*/

	TEST_ARITHMETIC_INT_OP(4,+,4);

	CZetScript::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
