/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;
class MyNumber{
public:
  int num;

  MyNumber(){
    this->num=0;
  }

  void _construct(int _n){
     printf("_construct MyNumber to %i\n",_n);
     this->num=_n;
  }

  void _construct(MyNumber * _n){
     printf("_construct MyNumber to %i\n",_n->num);
     this->num=_n->num;
  }

  void _set(int _n){
     printf("assign v: %i\n",_n);
     this->num=_n;
  }

  void _set(MyNumber * _n){
     printf("assign object v:%i\n",_n->num);
     this->num=_n->num;
  }

};

int main(int argc, char * argv[]) {

	/* CZetScript *zs = CZetScript::getInstance();

	  // register class MyNumber
	  register_C_Class<MyNumber>("MyNumber");

	  // register variable member num
	  register_C_VariableMember("num",&MyNumber::num);

	  // register constructor through function MyNumber::_set
	  register_C_FunctionMember("MyNumber",static_cast<void (MyNumber ::*)(int)>(&MyNumber::_construct));
	  register_C_FunctionMember("MyNumber",static_cast<void (MyNumber ::*)(MyNumber *)>(&MyNumber::_construct));


	  // register function _set as metamethod (same as constructor)
	  register_C_FunctionMember("_set",static_cast<void (MyNumber ::*)(int)>(&MyNumber::_set));
	  register_C_FunctionMember("_set",static_cast<void (MyNumber ::*)(MyNumber *)>(&MyNumber::_set));*/

	/*  if(!zs->eval(
	"var n1 = new MyNumber (10);\n"
"var n2 = new MyNumber (20);\n"
"var n3=n1; //  n3 is undefined!\n"
"n3=n2;\n"
"print(\"n1:\"+n1.num);\n"
"print(\"n3:\"+n3.num);\n"
"n3=10;\n"
"print(\"n3:\"+n3.num);\n"

	  )){
	     fprintf(stderr,"%s",ZS_GET_ERROR_MSG());
	  }


	  return 0;*/


	CZetScript *zetscript = CZetScript::getInstance();


	if (argc < 2) {
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	//if (
			if(!zetscript->eval_file(argv[1])){
				fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
			}
	 //  )
//	{
//		zetscript->execute();
//	}

	CZetScript::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
