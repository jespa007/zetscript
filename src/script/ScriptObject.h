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

	typedef struct _SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		int						created_idx_call;
	} SharedPointerInfo;

	typedef struct _InfoSharedPointerNode{
		SharedPointerInfo data;
		_InfoSharedPointerNode *previous, *next;
	} InfoSharedPointerNode;

	typedef struct _InfoSharedList{
		InfoSharedPointerNode *first, *last;
	}InfoSharedList;


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
		StackElement 	* getBuiltinProperty(const zs_string & property_name);

		StackElement * getBuiltinElementAt(short idx);
		virtual StackElement * addProperty(
				const zs_string & symbol_value
				,zs_string & error
				,StackElement * stk_element = NULL
		);
		virtual StackElement 	* getProperty(const zs_string & property_name);


		zs_vector * getStkBuiltinListElements();
		ScriptClass * 	    getNativeScriptClass();
		bool 				isNativeObject();
		const zs_string & getClassName();
		ScriptClass * 	    getScriptClass();
		StackElement 	*getThisProperty();
		virtual void	*getNativeObject();
		virtual zs_string toString();

		virtual ~ScriptObject();
	protected:

		StackElement stk_this;

		ZetScript 				*	zs; // 8
		VirtualMachine 			*	vm; // 8
		zs_vector				stk_builtin_elements;
		zs_map					*map_builtin_property_keys; // to search faster each property by its name


		void init(ZetScript *zs);

		virtual StackElement * newBuiltinSlot();
		virtual StackElement * addBuiltinProperty(const zs_string & symbol_value, StackElement stk=k_stk_null);
		bool unrefAndFreeStackElementContainer(StackElement *si);

	};

}
