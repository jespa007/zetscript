#pragma once

class CNativeFunction{

public:

	CNativeFunction();
	~CNativeFunction();

	static CNativeFunction * getInstance();
	static void destroySingletons();


	template <class _R, typename _F>
	void * new_proxy_function(unsigned int n_args, _F fun_obj){
		using namespace std::placeholders;
		void *proxy_function=NULL;

		tInfoProxyFunction ipf;


		switch(n_args){
		case 0:
			proxy_function=(void *)(new std::function<_R ()>(std::bind((_R (*)())fun_obj)));
			break;
		case 1:
			proxy_function=(void *)( new std::function<_R (int)>(std::bind((_R (*)(int))fun_obj, _1)));
			break;
		case 2:
			proxy_function=(void *)( new std::function<_R (int,int)>(std::bind((_R (*)(int,int))fun_obj, 1,_2)));
			break;
		case 3:
			proxy_function= (void *)(new std::function<_R (int,int,int)>(std::bind((_R (*)(int,int,int))fun_obj, 1,_2,_3)));
			break;
		case 4:
			proxy_function=(void *)( new std::function<_R (int,int,int,int)>(std::bind((_R (*)(int,int,int,int))fun_obj, 1,_2,_3,_4)));
			break;
		case 5:
			proxy_function=(void *)( new std::function<_R (int,int,int,int,int)>(std::bind((_R (*)(int,int,int,int,int))fun_obj, 1,_2,_3,_4,_5)));
			break;
		case 6:
			proxy_function=(void *)( new std::function<_R (int,int,int,int,int,int)>(std::bind((_R (*)(int,int,int,int,int,int))fun_obj, 1,_2, _3, _4, _5, _6)));
			break;
		default:
			print_error_cr("Max argyments reached!");
			break;

		}


		ipf.n_args=n_args;
		ipf.fun_ptr=proxy_function;

		if(typeid(_R) == typeid(void)){
			c_void_function.push_back(ipf);
		}else{
			c_int_function.push_back(ipf);
		}

		return proxy_function;
	}

	template <class _R>
	bool delete_proxy_function(unsigned int n_args, void *obj){
		using namespace std::placeholders;




		switch(n_args){
		case 0:
			delete (std::function<_R ()>*)obj;
			break;
		case 1:
			delete (std::function<_R (int)>*)obj;
			break;
		case 2:
			delete (std::function<_R (int,int)>*)obj;
			break;
		case 3:
			delete (std::function<_R (int,int,int)>*)obj;
			break;
		case 4:
			delete (std::function<_R (int,int,int,int)>*)obj;
			break;
		case 5:
			delete (std::function<_R (int,int,int,int,int)>*)obj;
			break;
		case 6:
			delete (std::function<_R (int,int,int,int,int,int)>*)obj;
			break;
		default:
			print_error_cr("Max argyments reached!");
			return false;
		}



		return true;
	}


	template <class _T, class _R,typename _F>
	void * c_member_class_function_proxy(unsigned int n_args, _F fun_obj){
		using namespace std::placeholders;
		std::function<void *(void *,PROXY_CREATOR)> *c_function_builder=NULL;



		switch(n_args){
		case 0:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val= (void *)(new std::function<_R ()>(std::bind((_R (_T::*)())(fun_obj), (_T *)obj)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R ()> *)obj;
				}
				return return_val;
			});
			break;
		case 1:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int)>(std::bind((_R (_T::*)(int))(fun_obj), (_T *)obj, _1)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int)> *)obj;
				}
				return return_val;
			});
			break;
		case 2:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int)>(std::bind((_R (_T::*)(int,int))(fun_obj), (_T *)obj, _1,_2)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int)> *)obj;
				}
				return return_val;
			});
			break;
		case 3:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int)>(std::bind((_R (_T::*)(int,int,int))(fun_obj), (_T *)obj, _1,_2,_3)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int)> *)obj;
				}
				return return_val;
			});
			break;
		case 4:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int,int)> *)obj;
				}
				return return_val;
			});

			break;
		case 5:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4,_5)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int,int,int)> *)obj;
				}
				return return_val;
			});

			break;
		case 6:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val= (void *)(new std::function<_R (int,int,int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4,_5,_6)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<int (int,int,int,int,int,int)> *)obj;
				}
				return return_val;
			});
			break;
		default:
			print_error_cr("Max argyments reached!");

		}

		class_function_member.push_back(c_function_builder);


		return c_function_builder;
	}





private:

	typedef struct{
		int n_args;
		void *fun_ptr;
	}tInfoProxyFunction;

	static CNativeFunction *m_instance;

	vector<tInfoProxyFunction>	c_int_function;
	vector<tInfoProxyFunction>	 c_void_function;
	vector<void *>	 class_function_member;

};
