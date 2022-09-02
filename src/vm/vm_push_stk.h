/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define VM_PUSH_STK_UNDEFINED \
		STK_SET_UNDEFINED(data->stk_vm_current);\
		data->stk_vm_current++; \

#define VM_PUSH_STK_NULL \
		STK_SET_NULL(data->stk_vm_current);\
		data->stk_vm_current++; \

#define VM_PUSH_STK_BOOLEAN(init_value) \
data->stk_vm_current->value=init_value; \
data->stk_vm_current->properties=STK_PROPERTY_BOOL; \
data->stk_vm_current++;


#define VM_PUSH_STK_ZS_INT(init_value) \
data->stk_vm_current->value=init_value; \
data->stk_vm_current->properties=STK_PROPERTY_ZS_INT; \
data->stk_vm_current++;

#define VM_PUSH_STK_SCRIPT_OBJECT(obj_value) \
data->stk_vm_current->value=(zs_int)obj_value; \
data->stk_vm_current->properties=STK_PROPERTY_SCRIPT_OBJECT; \
data->stk_vm_current++;


#define VM_PUSH_STK_ZS_FLOAT(init_value) \
	*((zs_float *)&data->stk_vm_current->value)=(init_value);\
	data->stk_vm_current->properties=STK_PROPERTY_ZS_FLOAT; \
	data->stk_vm_current++; \


#define VM_PUSH_STK_SCRIPT_FUNCTION(ref) \
data->stk_vm_current->value=ref; \
data->stk_vm_current->properties=STK_PROPERTY_FUNCTION; \
data->stk_vm_current++;

#define VM_PUSH_STK_TYPE(ref) \
data->stk_vm_current->value=ref; \
data->stk_vm_current->properties=STK_PROPERTY_TYPE; \
data->stk_vm_current++;


#define VM_PUSH_STK_PTR(stk_ptr) \
	data->stk_vm_current->value=(intptr_t)(stk_ptr);\
	data->stk_vm_current->properties=STK_PROPERTY_PTR_STK;\
	data->stk_vm_current++;

#define VM_PUSH_CONTAINER_SLOT_DATA(so_container_slot_ref,so_aux,str_symbol_aux1,stk_var) \
	data->stk_vm_current->value=(zs_int)(new ContainerSlotData(\
		so_container_slot_ref\
		,(ContainerScriptObject *)so_aux\
		,(zs_int)str_symbol_aux1\
		,stk_var\
		)\
	);\
	data->stk_vm_current->properties=STK_PROPERTY_CONTAINER_SLOT_READ_TO_CONTAINER_SLOT_WRITE;\
	data->stk_vm_current++;
