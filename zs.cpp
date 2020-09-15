/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZETSCRIP_COPYRIGHT "ZetScript %i.%i.%i Copyright (C) 2016-2020 Jordi Espada\n",ZETSCRIPT_MAJOR_VERSION,ZETSCRIPT_MINOR_VERSION,ZETSCRIPT_PATCH_VERSION

using namespace zetscript;


class Object{
public:
	int a;

	Object(){
		a=10;
	}

	virtual void size(int i){printf("size() 1 %i %i",a,i);}

	 virtual ~Object(){}

};

class Object2:public Object{
public:
	void size(int i){printf("size() 2 %i %i",a,i);}
	virtual ~Object2(){}
};

template <typename _C, typename _R, typename..._A>
void * build_function(_R (_C:: *fun_obj)(_A...)){
	return (void *)(new std::function<void (
					intptr_t
					,intptr_t
				)> (
					[fun_obj](
						intptr_t obj
						,intptr_t param1
					){
						(((_C *)obj)->* ((_R (_C    ::*)(intptr_t))fun_obj)) (
								param1
						);
					}
				));
}

//#define CAPTURE_VAR_MEMBER()

template <typename _C>
void * access_variable_member(int _C::*var_ptr){
	return (void *)(new std::function<intptr_t (
		intptr_t
	)> (
		[var_ptr](
			intptr_t obj
		){

			//p_C_i
			//((_C *)obj)->var_ptr;
			//((_C *)obj)->*var_ptr;

			return (intptr_t)(&((((_C *)obj)->*var_ptr)));
			//return (intptr_t)(&((_C *)obj)->a);
			/*int _C::*p_C_i=&_C::a;

			(intptr_t)(&(((_C *)obj)->* ((_R (_C    ::*)))var_ptr));

			return (intptr_t)(p_C_i);*/
			//int Class::*p_C_i
			//return offset= &_C::a; //(intptr_t)(&(((_C *)obj)->* ((_R (_C    ::*)))var_ptr));
		}
	));
}


/*
template<typename A, typename B>
void baseOf(std::string & var_symbol){
	intptr_t B::var_symbol
}
*/

int main(int argc, char * argv[]) {
/*
	auto fun_ptr=build_function<Object>(&Object::size);
	auto fun_ptr_solve_offset_a=access_variable_member<Object>(&Object::a); //&Object::a)

	Object  *p_obj1=new Object();
	Object2 *p_obj2=new Object2();

	intptr_t obj1=(intptr_t)(p_obj1);
	intptr_t obj2=(intptr_t)(p_obj2);

	std::function<void (
						intptr_t
						,intptr_t
					)> * fun_cast=(std::function<void (
							intptr_t
							,intptr_t
						)> *)fun_ptr;

	(*fun_cast)(obj1,10);
	(*fun_cast)(obj2,20);

	// solve a offset variable member ...
	std::function<intptr_t (
						intptr_t
					)> * fun_ptr_solve_offset_a_cast=(std::function<intptr_t (
							intptr_t
						)> *)fun_ptr_solve_offset_a;

	intptr_t ptr_a_object1=(*fun_ptr_solve_offset_a_cast)(obj1);
	intptr_t ptr_a_object2=(*fun_ptr_solve_offset_a_cast)(obj2);

	printf("Object::a:%p %p\n",&p_obj1->a,(void *)ptr_a_object1);
	printf("Object2::a:%p %p\n",&p_obj2->a,(void *)ptr_a_object2);

	return 0;*/

	ZetScript *zs = new ZetScript();

	if (argc > 1) {
		bool vm_execute=true;
		bool show_bytecode=false;
		std::string file;

		for(int i=1; i < argc; i++){

			std::vector<std::string> a=zs_strutils::split(argv[i],'=');
			switch(a.size()){
			case 1:

				if(strcmp(argv[i],"--no_execute")==0){
					vm_execute = false;
				}else if(strcmp(argv[i],"--show_byte_code")==0){
					show_bytecode=true;

				}else if(strcmp(argv[i],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					exit(0);
				}else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			case 2:

				if(strcmp(a[0].c_str(),"--file")==0){
					file=a[1];
				}
				else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			default:
				fprintf(stderr,"invalid argument %s\n",argv[i]);
				exit(-1);
				break;
			}

		}

		if(zs_strutils::is_empty(file)){
			fprintf(stderr,"Program with arguments you must specify file (i.e --file=filename )\n");
			exit(-1);
		}


		try{
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			try{
				zs->evalFile(file,vm_execute,show_bytecode);
			}catch(std::exception & ex){
				fprintf(stderr,"%s\n",ex.what());
			}
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> time_span=t2-t1;
			printf("executed %s %.0fms\n", zs_path::get_file_name(file).c_str(),time_span.count());
		}catch(std::exception & ex){
			fprintf(stderr,"%s\n",ex.what());
		}
	}
	else{

		bool exit = false;
		std::string expression;
		printf(ZETSCRIP_COPYRIGHT);
		printf("\n");

		do{
			printf("zs>");
			std::getline(std::cin,expression);

			exit = expression=="exit" || expression=="quit";
			if(!exit){ // evaluate expression

				try{
					zs->eval(expression.c_str());
				}catch(std::exception & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));
	}

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif

	return 0;
}
