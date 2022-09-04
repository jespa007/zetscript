/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_STK_UNDEFINED \
		STK_SET_UNDEFINED(data->vm_stk_current);\
		data->vm_stk_current++; \

#define VM_PUSH_STK_NULL \
		STK_SET_NULL(data->vm_stk_current);\
		data->vm_stk_current++; \

#define VM_PUSH_STK_BOOLEAN(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=STK_PROPERTY_BOOL; \
data->vm_stk_current++;


#define VM_PUSH_STK_ZS_INT(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=STK_PROPERTY_ZS_INT; \
data->vm_stk_current++;

#define VM_PUSH_STK_SCRIPT_OBJECT(obj_value) \
data->vm_stk_current->value=(zs_int)obj_value; \
data->vm_stk_current->properties=STK_PROPERTY_SCRIPT_OBJECT; \
data->vm_stk_current++;


#define VM_PUSH_STK_ZS_FLOAT(init_value) \
	*((zs_float *)&data->vm_stk_current->value)=(init_value);\
	data->vm_stk_current->properties=STK_PROPERTY_ZS_FLOAT; \
	data->vm_stk_current++; \


#define VM_PUSH_STK_SCRIPT_FUNCTION(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=STK_PROPERTY_FUNCTION; \
data->vm_stk_current++;

#define VM_PUSH_STK_TYPE(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=STK_PROPERTY_TYPE; \
data->vm_stk_current++;


#define VM_PUSH_STK_PTR(stk_ptr) \
	data->vm_stk_current->value=(intptr_t)(stk_ptr);\
	data->vm_stk_current->properties=STK_PROPERTY_PTR_STK;\
	data->vm_stk_current++;

#define VM_PUSH_CONTAINER_SLOT(so_container_slot_ref,so_aux,str_symbol_aux1,stk_var) \
	data->vm_stk_current->value=(zs_int)(new ContainerSlot(\
		(ContainerScriptObject *)so_aux\
		,(zs_int)str_symbol_aux1\
		,stk_var\
		)\
	);\
	data->vm_stk_current->properties=STK_PROPERTY_CONTAINER_SLOT;\
	data->vm_stk_current++;
