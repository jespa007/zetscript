/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define MSK_STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES				(STACK_ELEMENT_PROPERTY_FUNCTION-1)
#define STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES(prop)			((prop)&MSK_STACK_ELEMENT_PRIMITIVE_TYPE_PROPERTIES)

#define MSK_STACK_ELEMENT_TYPE_PROPERTIES						(STACK_ELEMENT_PROPERTY_MAX-1)
#define STACK_ELEMENT_TYPE_PROPERTIES(prop)						((prop)&MSK_STACK_ELEMENT_TYPE_PROPERTIES)

#define STACK_ELEMENT_IS_OBJECT_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_ITERATOR_OBJECT_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_OBJECT_ITERATOR_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk) 					(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_STRING_ITERATOR_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_ARRAY_SCRIPT_OBJECT(stk) 						(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_ITERATOR_ARRAY_SCRIPT_OBJECT(stk)				(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_CLASS_OBJECT(stk) 						(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id>=zetscript::SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_VAR_REF_OBJECT(stk) 					(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_VAR_REF_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_MEMBER_FUNCTION_OBJECT(stk)			(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT))
#define STACK_ELEMENT_IS_ZETOBJECT_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::STACK_ELEMENT_PROPERTY_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->script_type_id==zetscript::SCRIPT_TYPE_ID_ZETSCRIPT_SCRIPT_OBJECT))

#define ZS_GET_STK_VAR_REF(stk)  								(((VarRefObject *)((stk))->value)->getStackElementPtr())

#define STACK_ELEMENT_OP1_BOOL_OP2_BOOL							((STACK_ELEMENT_PROPERTY_BOOL<<16)	|	STACK_ELEMENT_PROPERTY_BOOL)
#define STACK_ELEMENT_OP1_INT_OP2_INT							((STACK_ELEMENT_PROPERTY_INT<<16)	|	STACK_ELEMENT_PROPERTY_INT)
#define STACK_ELEMENT_OP1_INT_OP2_FLOAT							((STACK_ELEMENT_PROPERTY_INT<<16)	|	STACK_ELEMENT_PROPERTY_FLOAT)
#define STACK_ELEMENT_OP1_FLOAT_OP2_INT							((STACK_ELEMENT_PROPERTY_FLOAT<<16)	|	STACK_ELEMENT_PROPERTY_INT)
#define STACK_ELEMENT_OP1_FLOAT_OP2_FLOAT						((STACK_ELEMENT_PROPERTY_FLOAT<<16)	|	STACK_ELEMENT_PROPERTY_FLOAT)
#define STACK_ELEMENT_OP1_TYPE_OP2_TYPE							((STACK_ELEMENT_PROPERTY_TYPE_ID<<16)	|	STACK_ELEMENT_PROPERTY_TYPE_ID)

// Check types
#define STACK_ELEMENT_IS_FLOAT(stk) 							((stk)->properties & STACK_ELEMENT_PROPERTY_FLOAT)
#define STACK_ELEMENT_IS_INT(stk) 								((stk)->properties & STACK_ELEMENT_PROPERTY_INT)
#define STACK_ELEMENT_IS_BOOLEAN(stk) 							((stk)->properties & STACK_ELEMENT_PROPERTY_BOOL)
#define STACK_ELEMENT_IS_PROPERTY_PTR_STK(stk) 					((stk)->properties & STACK_ELEMENT_PROPERTY_PTR_STK)
#define STACK_ELEMENT_IS_UNDEFINED(stk) 						((stk)->properties == STACK_ELEMENT_PROPERTY_UNDEFINED)
#define STACK_ELEMENT_IS_NULL(stk) 								((stk)->properties == STACK_ELEMENT_PROPERTY_NULL)
#define STACK_ELEMENT_IS_CONTAINER_SLOT(stk)					((stk)->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)
#define STACK_ELEMENT_IS_FUNCTION(stk)							(((stk)->properties & STACK_ELEMENT_PROPERTY_FUNCTION) | ((stk)->properties & STACK_ELEMENT_PROPERTY_MEMBER_FUNCTION))
#define STACK_ELEMENT_IS_TYPE(stk) 								((stk)->properties & STACK_ELEMENT_PROPERTY_TYPE_ID)
#define STACK_ELEMENT_IS_MEMBER_PROPERTY(stk) 					((stk)->properties & STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY)
#define STACK_ELEMENT_IS_MEMBER_FUNCTION(stk) 					((stk)->properties & STACK_ELEMENT_PROPERTY_MEMBER_FUNCTION)
#define STACK_ELEMENT_IS_OBJECT(stk) 							((stk)->properties & STACK_ELEMENT_PROPERTY_OBJECT)

#define STK_VALUE_TO_BOOL(ptr_result_instruction) 				(((bool)(ptr_result_instruction->value)))

#define STACK_ELEMENT_SET_UNDEFINED(stk)\
	(stk)->value=0;\
	(stk)->properties=STACK_ELEMENT_PROPERTY_UNDEFINED;


#define STACK_ELEMENT_SET_NULL(stk)\
	(stk)->value=0;\
	(stk)->properties=STACK_ELEMENT_PROPERTY_NULL;


namespace zetscript{
	class ZetScript;

	// properties shared by compiler + VM
	typedef enum :uint16_t {
		//---------- BEGIN PRIMITIVE TYPES HERE--------------------
		//ZS_MSK_STACK_ELEMENT_PROPERTY
		STACK_ELEMENT_PROPERTY_UNDEFINED			=0x0000,
		STACK_ELEMENT_PROPERTY_NULL					=0x0001,
		STACK_ELEMENT_PROPERTY_CHAR_PTR				=0x0002,
		STACK_ELEMENT_PROPERTY_INT					=0x0004,
		STACK_ELEMENT_PROPERTY_FLOAT				=0x0008,
		STACK_ELEMENT_PROPERTY_BOOL					=0x0010,
		STACK_ELEMENT_PROPERTY_TYPE_ID				=0x0020,
		//---------- END PRIMITIVE TYPES HERE--------------------
		STACK_ELEMENT_PROPERTY_FUNCTION				=0x0040,
		STACK_ELEMENT_PROPERTY_MEMBER_FUNCTION		=0x0080,
		STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY		=0x0100,
		STACK_ELEMENT_PROPERTY_OBJECT				=0x0200,
		STACK_ELEMENT_PROPERTY_CONTAINER_SLOT		=0x0400,
		STACK_ELEMENT_PROPERTY_MAX					=0x0800,
		//-- VM RUNTIME
		STACK_ELEMENT_PROPERTY_PTR_STK				= STACK_ELEMENT_PROPERTY_MAX,	 				// 0x1000
		STACK_ELEMENT_PROPERTY_READ_ONLY			=0x1000,

	}StackElementProperty;

	struct StackElement {


		zs_int 					value; // operable value
		uint16_t 				properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)
		void 					setUndefined();


		//String				toString();
		StackElement			typeOf();

	};

	extern const StackElement k_stk_undefined;

}
