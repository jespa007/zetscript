/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

class ClassA{
	public:
	 void fun1(const zetscript::zs_string & _s,bool _show_print){
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


class Num{
public:
	int x,y;

	Num(){
		x=0;
		y=0;
	}
};

Num *num_ref=NULL;

class ClassD{};
class ClassC:public ClassB{
public:
	ClassD d;
	Num num;
	ClassC(){
	}

    void fun1(bool _b,bool _show_print){
    	if(_show_print){
    		printf("from C: %s\n",_b?"true":"false");
    	}
    }
};

void setNumRef(Num *_num){
	num_ref=_num;
}


struct ParamA{zetscript::zs_string _s;ParamA(){_s="";}};
struct ParamB{zetscript::zs_float _f;ParamB(){_f=0;}};

ClassA *ClassA_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ClassA;
}

void ClassA_fun1(zetscript::ZetScript *_zs, ClassA *_this, ParamA *_param_a,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_this->fun1(_param_a->_s.c_str(),*_show_print);
}

bool ClassA_lt(zetscript::ZetScript *_zs, ClassA *_n1, ClassA *_n2){
	ZS_UNUSUED_3PARAMS(_zs,_n1,_n2);
	return true;
}

bool ClassA_lt(zetscript::ZetScript *_zs, ClassA *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_3PARAMS(_zs,_n1,_n2);
	printf("from float 1!\n");
	return true;
}

bool ClassA_lt(zetscript::ZetScript *_zs, zetscript::zs_float *_n1, ClassA *_n2){
	ZS_UNUSUED_3PARAMS(_zs,_n1,_n2);
	printf("from float 2!\n");
	return true;
}


void ClassA_delete(zetscript::ZetScript *_zs,ClassA *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


ClassB *ClassB_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ClassB;
}

void ClassB_fun1(zetscript::ZetScript *_zs, ClassB *_this, ParamB *_param_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_this->fun1(_param_b->_f,*_show_print);
}

void ClassB_delete(zetscript::ZetScript *_zs,ClassB *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

ClassC *ClassC_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ClassC;
}

void ClassC_fun1(zetscript::ZetScript *_zs, ClassC *_c, bool *_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_c->fun1(*_b,*_show_print);
}

/*void ClassC_num_setter(zetscript::ZetScript *_zs, ClassC *_c, zetscript::zs_int _num){
	ZS_UNUSUED_PARAM(_zs);
	_c->num=_num;
}*/

Num * ClassC_num_getter(zetscript::ZetScript *_zs, ClassC *_c){
	ZS_UNUSUED_PARAM(_zs);
	return &_c->num;
}

Num *ClassC_newNum(zetscript::ZetScript *_zs,ClassC *_c){
	ZS_UNUSUED_2PARAMS(_zs,_c);
	return num_ref;
}

zetscript::zs_int Num_x_getter(zetscript::ZetScript *_zs, Num *_this){
	ZS_UNUSUED_PARAM(_zs);
	return _this->x;
}

void Num_setPosition(zetscript::ZetScript *_zs, Num *_this){
	ZS_UNUSUED_2PARAMS(_zs,_this);
}

void Num_pre_increment(zetscript::ZetScript *_zs, Num *_n){
	ZS_UNUSUED_PARAM(_zs);
	++_n->x;
	++_n->y;
}

void Num_pre_decrement(zetscript::ZetScript *_zs, Num *_n){
	ZS_UNUSUED_PARAM(_zs);
	--_n->x;
	--_n->y;
}


ClassD * ClassC_get_d(zetscript::ZetScript *_zs, ClassC *_c){
	ZS_UNUSUED_PARAM(_zs);
	return &_c->d;
}

void ClassC_delete(zetscript::ZetScript *_zs,ClassC *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

bool allCharsTheSame(const zetscript::zs_string & input){

    for(int i =1; i < input.length(); i++){
        if(input[i-1] != input[i]) return false;
    }
    return true;
}

bool allValuesTheSame(zetscript::ZetScript *_zs,zetscript::ArrayScriptObject * sov){
	ZS_UNUSUED_PARAM(_zs);
	zetscript::zs_vector<zetscript::StackElement *> *stk_elements = sov->getStkListElements();
   for(int i =1; i < stk_elements->size(); i++){
	   zetscript::StackElement *stk_1=stk_elements->get(i-1);
	   zetscript::StackElement *stk_2=stk_elements->get(i-0);
       if(stk_1->value != stk_2->value) return false;
   }
   return true;
}

zetscript::zs_vector<zetscript::zs_int> newRandomCountExt(zetscript::ZetScript *_zs,zetscript::zs_int max_number, zetscript::zs_int n_elements){
	ZS_UNUSUED_PARAM(_zs);
	if(max_number==0 || n_elements==0){
		throw("max number or n_elements are 0s");
	}
		//new throw("invalid number");

	zetscript::zs_vector<zetscript::zs_int> index_rand;// = new int [n_elements];

	zetscript::zs_int item=0;
	bool found = false;
	for(int i = 0; i < n_elements; i++){

		do{
			 item = rand()%(max_number);
			found = false;
		 	for(int j=0; j < i;j++){
				if(index_rand.get(j) == item){
					found=true;
				}
		 	}
		}while(found);

		index_rand.push_back(item);

	}
	return index_rand;
}

zetscript::ArrayScriptObject * reorderValuesFromIntArray(zetscript::ZetScript *_zs,zetscript::ArrayScriptObject *_input){

	zetscript::ArrayScriptObject *output=zetscript::ArrayScriptObject::newArrayScriptObject(_zs);
	zetscript::zs_vector<zetscript::StackElement *> *input=_input->getStkListElements();
	uint16_t input_count=input->size();
	zetscript::zs_vector<zetscript::zs_int> rand_txt;

    if(allValuesTheSame(_zs,_input)){
          return _input;
    }


    bool equal = true;
    try{
		do{
			rand_txt= newRandomCountExt(_zs,input_count,input_count);
			 for(zetscript::zs_int i =0; i < input_count; i++){
		         equal&=(i==rand_txt.get(i)); // check if not consecutive...
			 }
		}while(equal);
    }catch(std::exception & ex){
    	fprintf(stderr,"%s",ex.what());
    }

    for(int i =0; i < input_count; i++){
    	// save resulting
    	zetscript::StackElement *stk=output->newSlot();
    	stk->value=_input->getStackElementByIndex(rand_txt.get(i))->value;
    	stk->properties=zetscript::ZS_STK_PROPERTY_INT;
    }

     return output;
}



//-------
// PARAMS

ParamA *ParamA_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ParamA;
}


void ParamA_delete(zetscript::ZetScript *_zs,ParamA *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

ParamB *ParamB_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ParamB;
}


void ParamB_delete(zetscript::ZetScript *_zs,ParamB *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

void test_call_function_member(zetscript::ZetScript *_zs, bool _show_print=true){

	Num num;
	setNumRef(&num);

	// ensure working doing various clear-eval
	for(int i=0; i < 5; i++){

		_zs->clear();

		try{
		_zs->eval(
			zetscript::zs_strutils::format(
					//"import \"include.zs\"\n"
					"function class_c_load(_class_c){\n"
						"_class_c.num_ref=_class_c.newNum()\n"
						"if(%s){\n"
						"Console::outln(\"load\")\n"
						"}\n"
						//"_class_c.num_ref.load()\n"
					"}\n"
					"class ClassCWrap{\n"
						"constructor(_this){\n"
							"this.self=_this;\n"
						"}\n"
						"load(){\n"
							"class_c_load(this)\n"
							//"Console::outln(\"load\")"
						"}\n"
						"newNum()\n{"
							"return this.self.newNum();\n"
						"}\n"
						"ini(n){\n"
							"ini2(n)\n"
							//"n.load()"
							//"Console::outln()"
						"}\n"
						"updateTutorial(){\n"
							"for(var i=0; i < 5; i++){\n"
							"	this.v2d_target=this.num_ref\n"
							"}\n"
							"this.v2d_target.setPosition();\n"
							//"n.load()"
							"if(%s){\n"
							"	Console::outln(\"update\")\n"
							"}\n"
						"}\n"
						"update(){\n"
							"this.updateTutorial();\n"
						"}\n"
					"}\n"
					"function ini2(n){n.load()}"
					"var c=new ClassCWrap(new ClassC())\n"
					"ini2(c);\n"
					"c.update();\n"
					/*"var c=new ClassC();\n"
					"c.fun1(new ParamA(),%s);\n"
					"Console::outln(\"decrement\");\n"
					//"c.num.x=10;\n"
					"c.num;\n"
					//"c.num.x.y.z.t==10;\n"
					"Console::outln(c);\n"
					"c.load()\n"*/
					//"c.get_d.x=0;\n"
					//"(new ClassC()).fun1(1.5,%s)\n"
					//"(new ClassC()).fun1(false,%s)\n"
					,_show_print?"true":"false"
					,_show_print?"true":"false"
			)
		//,zetscript::EvalOption::ZS_EVAL_OPTION_PRINT_BYTE_CODE

		);

		//_zs->clear();

		}catch(std::exception & ex){
			fprintf(stderr,"%s\n",ex.what());
		}

		/*auto ini=new std::function<void(Num *)>(_zs->bindScriptFunction<void (Num *)>("c.ini"));
		(*ini)(&num);
		delete ini;*/

		//_zs->clear();

		//_zs->clear();

		//(*ini)();

		//delete ini;

	}

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
	ZS_UNUSUED_PARAM(_zs);
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

	_zs->registerType<ClassA>("ClassA", ClassA_new, ClassA_delete);
	_zs->registerType<ClassB>("ClassB",ClassB_new,ClassB_delete);
	_zs->registerType<ClassC>("ClassC",ClassC_new,ClassC_delete);
	_zs->registerType<Num>("Num");
	_zs->registerType<ClassD>("ClassD");

	_zs->registerType<ParamA>("ParamA",ParamA_new,ParamA_delete);
	_zs->registerType<ParamB>("ParamB",ParamB_new,ParamB_delete);


	_zs->registerFunction("reorderValuesFromIntArray",reorderValuesFromIntArray);

	_zs->registerMemberFunction<ClassA>("fun1",ClassA_fun1);
	_zs->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,ClassA *,ClassA *)>(ClassA_lt));
	_zs->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,ClassA *,zetscript::zs_float *)>(ClassA_lt));
	_zs->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,zetscript::zs_float *,ClassA *)>(ClassA_lt));


	_zs->extends<ClassB,ClassA>();

	_zs->registerMemberFunction<ClassB>("fun1",ClassB_fun1);


	_zs->extends<ClassC,ClassB>();

	_zs->registerMemberFunction<ClassC>("newNum",ClassC_newNum);

	_zs->registerMemberPropertyMetamethod<ClassC>("num","_get",ClassC_num_getter);
	_zs->registerMemberFunction<Num>("_preinc",Num_pre_increment);
	_zs->registerMemberFunction<Num>("_predec",Num_pre_decrement);

	_zs->registerMemberPropertyMetamethod<Num>("x","_get",Num_x_getter);
	_zs->registerMemberFunction<Num>("setPosition",Num_setPosition);

	_zs->registerMemberFunction<ClassC>("get_d",ClassC_get_d);
	//_zs->extends<ClassC,ClassB>();

	//_zs->registerMemberFunction<ClassC>("fun1",ClassC_fun1);


	_zs->registerFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	//a->printListMemberFunctions();

	//b->printListMemberFunctions();

	//c->printListMemberFunctions();

	_zs->saveState();


	test_call_native_function_with_nulls(_zs);

	test_call_function_member(_zs,_show_print);

}

void test_call_native_function_no_print(zetscript::ZetScript *_zs){
	test_call_native_function(_zs,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_call_native_function(&zs);
		//test_bug();
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
	return 0;
}
#endif


