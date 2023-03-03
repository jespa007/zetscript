var json_str_src="{"
+"\"a\":10.000000"
+",\"b\":true"
+",\"c\":\"string\""
+",\"d\":[0.000000,true,\"string\"]"
+",\"e\":{"
+	"\"a\":10.000000"
+	",\"b\":true"
+	",\"c\":\"string\""
+	",\"d\":[0.000000,true,\"string\"]"
+"}"
+"}";

var json_obj=Json::deserialize(json_str_src);

System::assert(typeof json_obj.a==Float		,"deserialize json_obj.a != typeof Float");
System::assert(typeof json_obj.b==Boolean	,"deserialize json_obj.b != typeof Boolean");
System::assert(typeof json_obj.c==String	,"deserialize json_obj.c != typeof String");
System::assert(typeof json_obj.d==Array	,"deserialize json_obj.d != typeof Array");
System::assert(typeof json_obj.d[0]==Float	,"deserialize json_obj.d[0] != typeof Float");
System::assert(typeof json_obj.d[1]==Boolean,"deserialize json_obj.d[1] != typeof Boolean");
System::assert(typeof json_obj.d[2]==String	,"deserialize json_obj.d[2] != typeof String");

System::assert(typeof json_obj.e==Object	,"deserialize json_obj.e != typeof Object");

System::assert(Object::keys(json_obj.e).length==4,"deserialize Object::keys(json_obj.e).length != 4");

System::assert(typeof json_obj.e.a==Float		,"deserialize json_obj.e.a != typeof Float");
System::assert(typeof json_obj.e.b==Boolean		,"deserialize json_obj.e.b != typeof Boolean");
System::assert(typeof json_obj.e.c==String		,"deserialize json_obj.e.c != typeof String");
System::assert(typeof json_obj.e.d==Array		,"deserialize json_obj.e.d != typeof Array");
System::assert(typeof json_obj.e.d[0]==Float	,"deserialize json_obj.e.d[0] != typeof Float");
System::assert(typeof json_obj.e.d[1]==Boolean	,"deserialize json_obj.e.d[1] != typeof Boolean");
System::assert(typeof json_obj.e.d[2]==String	,"deserialize json_obj.e.d[2] != typeof String");


var json_str_dst=Json::serialize(json_obj)
System::assert(json_str_src==json_str_dst,"serialize A ('"+json_str_src+"') != B ("+json_str_dst+")")


