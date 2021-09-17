namespace zetscript{

	template<typename T>
	zs_vector<T> ScriptObjectVector::toStdVector(ScriptObjectVector *v_in){
		zs_vector<T> v_out;
		const char * dst_convert_type = typeid(T).name();
		zs_float aux_flt;
		zs_vector * elements = v_in->getStkUserListElements();


		for(unsigned i = 0; i < elements->count; i++){

			StackElement *sv=elements->get(i);

			switch(sv->properties & STK_PROPERTY_TYPE_PRIMITIVES)
			{
				case STK_PROPERTY_ZS_FLOAT:
					if(ZS_STRCMP(dst_convert_type, ==,typeid(zs_float).name())){
						ZS_FLOAT_COPY(&aux_flt, &sv->value);
						v_out.push_back(aux_flt);
					}else if(ZS_STRCMP(dst_convert_type, ==,typeid(zs_int).name())){
						v_out.push_back((zs_int)sv->value);
					}else{
						THROW_RUNTIME_ERROR("Error trying to cast element on zs_vector<%s>",ZS_STR(zs_float));
						return v_out;
					}
					break;
				default:
				case STK_PROPERTY_NULL:
				case STK_PROPERTY_BOOL:
				case STK_PROPERTY_FUNCTION:
					THROW_RUNTIME_ERROR("type not implemented yet");
					return v_out;
					break;
				case STK_PROPERTY_ZS_INT:
					if(ZS_STRCMP(dst_convert_type, ==,typeid(zs_int).name()) || ZS_STRCMP(dst_convert_type, ==,typeid(zs_float).name())){// typeid(int).name()) || ){
						v_out.push_back((zs_int)sv->value);
					}else{
						THROW_RUNTIME_ERROR("Error trying to cast element on zs_vector<zs_int>");
						return v_out;
					}
					break;
			}

		}
		return v_out;
	}

}
