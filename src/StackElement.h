/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IS_STK_FUNCTION(stk)							((stk->properties & ZS_STK_PROPERTY_FUNCTION) || (stk->properties & ZS_STK_PROPERTY_MEMBER_FUNCTION))

#define ZS_STK_PROPERTY_TYPE_PRIMITIVES					(ZS_STK_PROPERTY_FUNCTION-1)
#define GET_ZS_STK_PROPERTY_PRIMITIVE_TYPES(prop)			((prop)&ZS_STK_PROPERTY_TYPE_PRIMITIVES)

#define ZS_STK_PROPERTY_TYPES								(ZS_STK_PROPERTY_MAX-1)
#define GET_ZS_STK_PROPERTY_TYPES(prop)					((prop)&ZS_STK_PROPERTY_TYPES)

#define STK_IS_OBJECT_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define STK_IS_ITERATOR_OBJECT_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))
#define STK_IS_STRING_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_STRING))
#define STK_IS_ITERATOR_STRING_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING))
#define STK_IS_ARRAY_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ARRAY))
#define STK_IS_ITERATOR_ARRAY_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY))
#define STK_IS_CLASS_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type>=zetscript::IDX_TYPE_SCRIPT_OBJECT_CLASS))
#define STK_IS_VAR_REF_SCRIPT_OBJECT(stk) 				(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_VAR_REF))
#define STK_IS_MEMBER_FUNCTION_SCRIPT_OBJECT(stk)		(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER))
#define STK_IS_ZETSCRIPT_OBJECT_SCRIPT_OBJECT(stk) 		(((stk)->properties & zetscript::ZS_STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ZETSCRIPT))

#define STK_GET_STK_VAR_REF(stk)  						(((VarRefScriptObject *)((stk))->value)->getStackElementPtr())

#define MSK_STK_OP1_BOOL_OP2_BOOL						((ZS_STK_PROPERTY_BOOL<<16)	|	ZS_STK_PROPERTY_BOOL)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_INT					((ZS_STK_PROPERTY_INT<<16)	|	ZS_STK_PROPERTY_INT)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT					((ZS_STK_PROPERTY_INT<<16)	|	ZS_STK_PROPERTY_FLOAT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT					((ZS_STK_PROPERTY_FLOAT<<16)|	ZS_STK_PROPERTY_INT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT				((ZS_STK_PROPERTY_FLOAT<<16)|	ZS_STK_PROPERTY_FLOAT)
#define MSK_STK_OP1_TYPE_OP2_TYPE						((ZS_STK_PROPERTY_TYPE<<16)	|	ZS_STK_PROPERTY_TYPE)

#define ZS_STK_PROPERTY_RUNTIME							(((0x1<<(MAX_BIT_RUNTIME-BIT_ZS_STK_PROPERTY_IS_C_VAR_PTR))-1)<<(BIT_ZS_STK_PROPERTY_IS_C_VAR_PTR))
#define GET_ZS_STK_PROPERTY_RUNTIME(prop)					((prop)&ZS_STK_PROPERTY_RUNTIME)

#define STK_VALUE_IS_ZS_UNDEFINED(stk) 					(stk->properties == ZS_STK_PROPERTY_UNDEFINED)
#define STK_VALUE_IS_ZS_NULL(stk) 						(stk->properties == ZS_STK_PROPERTY_NULL)


// Check types
#define STK_VALUE_IS_ZS_FLOAT(stk) 						((stk)->properties & ZS_STK_PROPERTY_FLOAT)
#define STK_VALUE_IS_ZS_INT(stk) 						((stk)->properties & ZS_STK_PROPERTY_INT)
#define STK_VALUE_IS_BOOLEAN(stk) 						((stk)->properties & ZS_STK_PROPERTY_BOOL)
#define STK_VALUE_IS_PROPERTY_PTR_STK(stk) 				((stk)->properties & ZS_STK_PROPERTY_PTR_STK)
#define STK_VALUE_IS_UNDEFINED(stk) 					((stk)->properties == ZS_STK_PROPERTY_UNDEFINED)
#define STK_VALUE_IS_NULL(stk) 							((stk)->properties == ZS_STK_PROPERTY_NULL)
#define STK_VALUE_IS_CONTAINER_SLOT(stk)				((stk)->properties & ZS_STK_PROPERTY_CONTAINER_SLOT)
#define STK_VALUE_IS_FUNCTION(stk) 						((stk)->properties & ZS_STK_PROPERTY_FUNCTION)
#define STK_VALUE_IS_TYPE(stk) 							((stk)->properties & ZS_STK_PROPERTY_TYPE)
#define STK_VALUE_IS_MEMBER_PROPERTY(stk) 				((stk)->properties & ZS_STK_PROPERTY_MEMBER_PROPERTY)
#define STK_VALUE_IS_MEMBER_FUNCTION(stk) 				((stk)->properties & ZS_STK_PROPERTY_MEMBER_FUNCTION)
#define STK_VALUE_IS_SCRIPT_OBJECT(stk) 				((stk)->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT)

#define STK_VALUE_TO_BOOL(ptr_result_instruction) 		(((bool)(ptr_result_instruction->value)))

#define STK_SET_UNDEFINED(stk)\
	(stk)->value=0;\
	(stk)->properties=ZS_STK_PROPERTY_UNDEFINED;


#define STK_SET_NULL(stk)\
	(stk)->value=0;\
	(stk)->properties=ZS_STK_PROPERTY_NULL;


namespace zetscript{
	class ZetScript;

	// properties shared by compiler + VM
	typedef enum:unsigned short {
		//---------- BEGIN PRIMITIVE TYPES HERE--------------------
		ZS_STK_PROPERTY_UNDEFINED			=0x0000,
		ZS_STK_PROPERTY_NULL				=0x0001,
		ZS_STK_PROPERTY_CHAR_PTR			=0x0002,
		ZS_STK_PROPERTY_INT					=0x0004,
		ZS_STK_PROPERTY_FLOAT				=0x0008,
		ZS_STK_PROPERTY_BOOL				=0x0010,
		ZS_STK_PROPERTY_TYPE				=0x0020,
		//---------- END PRIMITIVE TYPES HERE--------------------
		ZS_STK_PROPERTY_FUNCTION			=0x0040,
		ZS_STK_PROPERTY_MEMBER_FUNCTION		=0x0080,
		ZS_STK_PROPERTY_MEMBER_PROPERTY		=0x0100,
		ZS_STK_PROPERTY_SCRIPT_OBJECT		=0x0200,
		ZS_STK_PROPERTY_CONTAINER_SLOT		=0x0400,
		ZS_STK_PROPERTY_MAX					=0x0800,
		//-- VM RUNTIME
		ZS_STK_PROPERTY_PTR_STK				= ZS_STK_PROPERTY_MAX,	 				// 0x1000
		ZS_STK_PROPERTY_READ_ONLY			=0x1000,

	}ZS_StkProperty;

	struct StackElement {


		zs_int 					value; // operable value
		uint16_t 				properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)
		void 					setUndefined();


		//zs_string				toString();
		StackElement			typeOf();

	};

	extern const StackElement k_stk_undefined;

}
