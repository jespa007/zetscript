/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_current_call])

namespace zetscript{


	void VirtualMachine::doStackDump(){
		// derefer all variables in all scopes (except main )...
		while(vm_scope<(vm_current_scope)){
			popVmScope(false);
		}

		removeEmptySharedPointers(IDX_CALL_STACK_MAIN);
		vm_idx_call=0;
	}

	VirtualMachine::VirtualMachine(ZetScript *_zs){
		//-----------------------------------------------------------
		// set memory manager

		memset(&zero_shares,0,sizeof(zero_shares));
		memset(&shared_vars,0,sizeof(shared_vars));
		StackElement *aux=vm_stack;

		for(int i=0; i < VM_STACK_LOCAL_VAR_MAX;i++){
			*aux++=stk_undefined;
		}

		vm_idx_call=0;
		stk_vm_current=NULL;
		vm_current_scope = vm_scope;

		f_aux_value1=0;
		f_aux_value2=0;

		idx_last_statment=0;

		vm_scope_max = &vm_scope[VM_SCOPE_MAX-1];

		main_function_object = NULL;
		main_class_object=NULL;

		current_call_c_function = NULL;
		zs=_zs;

		script_function_factory=NULL;
		script_class_factory=NULL;
		scope_factory=NULL;
		vm_error=false;
		vm_error_str="";
	}

	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr ZS_PRINT_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	void VirtualMachine::init(){
		// script class factory should be created and initialized
		script_function_factory=this->zs->getScriptFunctionFactory();
		script_class_factory=this->zs->getScriptClassFactory();
		scope_factory = this->zs->getScopeFactory();
		main_function_object = MAIN_FUNCTION(this);
		main_class_object = SCRIPT_CLASS_MAIN(this);
	}


	//============================================================================================================================================
	// POINTER MANANAGER

	InfoSharedPointerNode * VirtualMachine::newSharedPointer(ScriptObject *_var_ptr){
		//int index = VirtualMachine::getFreeCell();
		InfoSharedPointerNode *_node = (InfoSharedPointerNode *)malloc(sizeof(InfoSharedPointerNode));
		// init
		_node->previous=NULL;
		_node->next=NULL;
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->data.zero_shares=&zero_shares[vm_idx_call];

		// insert node into shared nodes ...
		if(!insertShareNode(_node->data.zero_shares,_node)){
			return NULL;
		}
		return _node;
	}

	bool VirtualMachine::sharePointer(InfoSharedPointerNode *_node){

		unsigned short *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			VM_SET_USER_ERROR(this,"MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
			return false;
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			if(!deattachShareNode(_node->data.zero_shares,_node)){
				return false;
			}
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			if(!insertShareNode(&shared_vars,_node)){
				return false;
			}

			// node is not in list of zero refs anymore...
			_node->data.zero_shares=NULL;
		}
		return true;
	}

	bool VirtualMachine::unrefSharedScriptObject(InfoSharedPointerNode *_node, int idx_current_call, bool remove_if_0){

		unsigned short *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){ // already zero
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				if(!deattachShareNode(&shared_vars,_node)){
					return false;
				}

				if(remove_if_0){ // force remove node and data ...
					delete _node->data.shared_ptr;
					free(_node);
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->data.zero_shares=&zero_shares[idx_current_call];
					if(!insertShareNode(_node->data.zero_shares,_node)){
						return false;
					}

				}
			}
		}

		return true;
	}

	const ScriptFunction * VirtualMachine::getCurrent_C_FunctionCall(){
		return current_call_c_function;
	}

	void VirtualMachine::setError(const std::string & str){
		vm_error = true;
		vm_error_str=str;
	}

	std::string VirtualMachine::getError(){
		return vm_error_str;
	}

	void VirtualMachine::setErrorFileLine(const char *file, int line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		setError(zs_strutils::format("[%s:%i] %s",file,line,out_txt));
	}

	StackElement  VirtualMachine::execute(
		 ScriptObjectClass 		*	this_object
		 ,ScriptFunction 	*	calling_function
		 ,StackElement 		*  	stk_params
		 ,unsigned	char  		n_stk_params
		 , const char *file
		 , int line
	){

		StackElement stk_result=stk_undefined;
		StackElement info=stk_undefined;

		if(vm_idx_call==0){ // set stack and Init vars for first call...

			vm_error=false;
			vm_error_str="";
			this->vm_error_callstack_str="";

			stk_vm_current=vm_stack;

			// calls script function from C : preserve stack space for global vars
			if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){
				stk_vm_current=&vm_stack[main_function_object->registered_symbols->count];
			}
			//vm_foreach_current=&vm_foreach[0];
		}else{ // Not main function -> allow params for other functions
			// push param stack elements...
			for(unsigned i = 0; i < n_stk_params; i++){
				*stk_vm_current++=stk_params[i];
			}
		}

		// byte code executing starts here. Later script function can call c++ function, but once in c++ function is not possible by now call script function again.

		info=callFunctionScript(
			this_object,
			calling_function,
			stk_vm_current,
			n_stk_params);

		// if string or object do not remove empty shared pointers if they are 0s
		if(info.properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
			// add generated
			insertLifetimeObject(file,line,(ScriptObjectAnonymous *)info.stk_value);
			/*ScriptObjectAnonymous * so=(ScriptObjectAnonymous *)info.var_ref;
			if(so->shared_pointer==NULL){ // is not shared, add on the list for next time...
				so->initSharedPtr();
			}*/
		}


		if(vm_error){
			// it was error so reset stack and stop execution ? ...
			doStackDump();
			throw std::runtime_error(this->vm_error_callstack_str+"\n"+vm_error_str);
		}

		return info;
	}

	void VirtualMachine::insertLifetimeObject(const char *file, int line, ScriptObjectAnonymous *script_object){
		InfoLifetimeObject *info = (InfoLifetimeObject *)malloc(sizeof(InfoLifetimeObject));

		info->file=file;
		info->line=line;
		info->script_object=script_object;

		lifetime_object[script_object]=info;
	}

	void VirtualMachine::destroyLifetimeObject(ScriptObject *script_object){
		if(lifetime_object.count(script_object)==0){
			THROW_RUNTIME_ERROR("Cannot find stack element lifetime");
		}

		InfoLifetimeObject *info = lifetime_object[script_object];

		lifetime_object.erase(script_object);

		delete info->script_object;
		free(info);
	}

	bool VirtualMachine::setStackElementAt(unsigned int idx, StackElement stk){
		if(idx >= VM_STACK_LOCAL_VAR_MAX){
			VM_SET_USER_ERROR(this,"setStackElement: out of bounds");
			return false;
		}

		vm_stack[idx]=stk;
		return true;
	}

	StackElement *VirtualMachine::getStkVmCurrent(){
		return stk_vm_current;
	}

	StackElement * VirtualMachine::getStackElementAt(unsigned int idx_glb_element){

		if(idx_glb_element < main_function_object->registered_symbols->count){
			return &vm_stack[idx_glb_element];
		}else{
			VM_SET_USER_ERROR(this,"getStackElement: out of bounds");
		}
		return NULL;
	}


	StackElement  * VirtualMachine::getLastStackValue(){
		return (stk_vm_current-1);
	}


	VirtualMachine::~VirtualMachine(){

		if(lifetime_object.size()>0){

			std::string error="\n\nSome lifetime objects created by virtual machine were not destroyed:\n\n";

			for(auto it=lifetime_object.begin(); it !=lifetime_object.end();it++ ){
				error+=zs_strutils::format("* Object lifetime created at [%s:%i] was not destroyed \n",zs_path::get_filename(it->second->file).c_str(),it->second->line);
			}

			error+="\n\n";
			//error+="\nPlease destroy lifetime objects through destroyLifetimeObject() before destroy zetscript to avoid this exception\n";

			fprintf(stderr,"%s",error.c_str());

		}
	}
}


#include "VirtualMachine_callFunctionNative.cpp"
#include "VirtualMachine_callFunctionScript.cpp"
