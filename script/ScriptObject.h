/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define TYPE_SCRIPT_VARIABLE 						"__ScriptObject__"

namespace zetscript{

	struct _InfoSharedList;
	struct VirtualMachine;
	class ScriptObject;
	class ScriptFunction;
	//struct EvalInstruction;

	typedef struct _SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		//_InfoSharedList *zero_shares;
		int						created_idx_call;
	} SharedPointerInfo;

	typedef struct _InfoSharedPointerNode{
		SharedPointerInfo data;
	//		unsigned short currentStack;
		_InfoSharedPointerNode *previous, *next;
	} InfoSharedPointerNode;

	typedef struct _InfoSharedList{
		InfoSharedPointerNode *first, *last;
	}InfoSharedList;


	struct StackMemberFunction{
	public:
		ScriptFunction 		*so_function; // make function pointer first to make compatible with stk
		ScriptObject		*so_object;

		StackMemberFunction(
				ScriptObject		*_so_object
				,ScriptFunction 	*_so_function){
			so_object = _so_object;
			so_function= _so_function;
		}
	};

	struct MemberAttribute;
	struct StackMemberAttribute{
	public:
		MemberAttribute 	*member_attribute; // make function pointer first to make compatible with stk
		ScriptObject		*so_object;

		StackMemberAttribute(
				ScriptObject		*_so_object
				,MemberAttribute 	*_member_attribute){
			so_object = _so_object;
			member_attribute= _member_attribute;
		}
	};

	class ScriptObject{
	public:
		InfoSharedPointerNode 	* 	shared_pointer; // 8
		short						idx_script_class; // 2

		ScriptObject();

		ScriptFunction *getGetter();
		zs_vector *getSetterList();

		virtual				size_t length();

		ZetScript      * 	getZetScript();
		StackElement 	* getBuiltinProperty(const std::string & property_name);
		//int			   		getBuiltinPropertyIdx(const std::string & varname);
		StackElement * getBuiltinElementAt(short idx);
		virtual StackElement * addProperty(
				const std::string & symbol_value
				,std::string & error
				,StackElement * stk_element = NULL
		);
		virtual StackElement 	* getProperty(const std::string & property_name);


		zs_vector * getStkBuiltinListElements();
		ScriptClass * 	    getNativeScriptClass();
		bool 				isNativeObject();
		const std::string & getClassName();
		ScriptClass * 	    getScriptClass();
		StackElement 	*getThisProperty();
		virtual void	*getNativeObject();
		virtual std::string toString(const std::string & _format="");

		virtual ~ScriptObject();
	protected:

		StackElement stk_this;

		ZetScript 				*	zs; // 8
		VirtualMachine 			*	vm; // 8
		zs_vector				stk_builtin_elements;
		zs_map					*map_builtin_property_keys; // to search faster each property by its name


		void init(ZetScript *zs);

		//StackElement 			stk_length;
		virtual StackElement * newBuiltinSlot();
		virtual StackElement * addBuiltinProperty(const std::string & symbol_value, StackElement stk=k_stk_undefined);
		bool unrefAndFreeStackElementContainer(StackElement *si);

	};

}
