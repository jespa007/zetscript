/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

class ClassA{
	public:
	 void fun1(const std::string & _s,bool _show_print){
		 if(_show_print){
			 printf("from A: %s\n",_s.c_str());
		 }
	 }
};

class ClassB:public ClassA{
public:
    void fun1(float _f,bool _show_print){
    	if(_show_print){
    		printf("from B: %.02f\n",_f);
    	}
    }
};

class ClassC:public ClassB{
public:
    void fun1(bool _b,bool _show_print){
    	if(_show_print){
    		printf("from C: %s\n",_b?"true":"false");
    	}
    }
};

ClassA *ClassAWrap_new(zetscript::ZetScript *_zs){
	return new ClassA;
}

void ClassAWrap_fun1(zetscript::ZetScript *_zs, ClassA *_a, zetscript::zs_string *_s,bool *_show_print){
   _a->fun1(_s->c_str(),*_show_print);
}

void ClassAWrap_delete(zetscript::ZetScript *_zs,ClassA *_this){
	delete _this;
}


ClassB *ClassBWrap_new(zetscript::ZetScript *_zs){
	return new ClassB;
}

void ClassBWrap_fun1(zetscript::ZetScript *_zs, ClassB *_b, zetscript::zs_float *_f,bool *_show_print){
  _b->fun1(*_f,*_show_print);
}

void ClassBWrap_delete(zetscript::ZetScript *_zs,ClassB *_this){
	delete _this;
}

ClassC *ClassCWrap_new(zetscript::ZetScript *_zs){
	return new ClassC;
}

void ClassCWrap_fun1(zetscript::ZetScript *_zs, ClassC *_c, bool *_b,bool *_show_print){
  _c->fun1(*_b,*_show_print);
}

void ClassCWrap_delete(zetscript::ZetScript *_zs,ClassC *_this){
	delete _this;
}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

void test_call_function_member(zetscript::ZetScript *_zs, bool _show_print=true){

	_zs->eval(
		zetscript::zs_strutils::format(
				"(new ClassC()).fun1(\"hello world\",%s)\n"
				//"(new ClassC()).fun1(1.5,%s)\n"
				//"(new ClassC()).fun1(false,%s)\n"
				,_show_print?"true":"false"
				,_show_print?"true":"false"
				,_show_print?"true":"false"
		)
	);

}

//------------------------------
// TEST NATIVE CALL WITH NULLS
void test_native_function_with_nulls(
		zetscript::ZetScript *_zs
		,zetscript::zs_int _p1
		,zetscript::zs_float *_p2
		,bool *_p3
		,ClassA *_p4
){
	if(_p1 != 0){std::runtime_error("_p1 not 0");}
	if(_p2 != NULL){std::runtime_error("_p2 not NULL");}
	if(_p3 != NULL){std::runtime_error("_p3 not NULL");}
	if(_p4 != NULL){std::runtime_error("_p4 not NULL");}

}

void test_call_native_function_with_nulls(zetscript::ZetScript *_zs){


	_zs->eval(
		// calls C function 'test_native_function_with_nulls' passing all parameters as null.
		"test_native_function_with_nulls(\n"
			"null\n"
			",null\n"
			",null\n"
			",null\n"
		");\n"
		""
	);
}


void test_call_native_function(zetscript::ZetScript *_zs, bool _show_print=true){

	auto a=_zs->bindType<ClassA>("ClassA",ClassAWrap_new,ClassAWrap_delete);
	auto b=_zs->bindType<ClassB>("ClassB",ClassBWrap_new,ClassBWrap_delete);
	auto c=_zs->bindType<ClassC>("ClassC",ClassCWrap_new,ClassCWrap_delete);

	_zs->bindMemberFunction<ClassA>("fun1",ClassAWrap_fun1);

	_zs->nativeTypeExtendsFrom<ClassB,ClassA>();

	_zs->bindMemberFunction<ClassB>("fun1",ClassBWrap_fun1);


	_zs->nativeTypeExtendsFrom<ClassC,ClassB>();
	//_zs->nativeTypeExtendsFrom<ClassC,ClassB>();

	//_zs->bindMemberFunction<ClassC>("fun1",ClassCWrap_fun1);


	_zs->bindFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	a->printListMemberFunctions();

	b->printListMemberFunctions();

	c->printListMemberFunctions();


	test_call_native_function_with_nulls(_zs);

	test_call_function_member(_zs,_show_print);

}



#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_call_native_function(&zs);
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
	return 0;
}
#endif


