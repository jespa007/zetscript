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

System::assert(typeof json_obj.a==float		,"deserialize json_obj.a != typeof float");
System::assert(typeof json_obj.b==bool		,"deserialize json_obj.b != typeof bool");
System::assert(typeof json_obj.c==String	,"deserialize json_obj.c != typeof String");
System::assert(typeof json_obj.d==Vector	,"deserialize json_obj.d != typeof Vector");
System::assert(typeof json_obj.d[0]==float	,"deserialize json_obj.d[0] != typeof float");
System::assert(typeof json_obj.d[1]==bool	,"deserialize json_obj.d[1] != typeof bool");
System::assert(typeof json_obj.d[2]==String	,"deserialize json_obj.d[2] != typeof String");

System::assert(typeof json_obj.e==Object	,"deserialize json_obj.e != typeof Object");

System::assert(Object::keys(json_obj.e).size()==4,"deserialize Object::keys(json_obj.e).size() != 4");

System::assert(typeof json_obj.e.a==float		,"deserialize json_obj.e.a != typeof float");
System::assert(typeof json_obj.e.b==bool		,"deserialize json_obj.e.b != typeof bool");
System::assert(typeof json_obj.e.c==String		,"deserialize json_obj.e.c != typeof String");
System::assert(typeof json_obj.e.d==Vector		,"deserialize json_obj.e.d != typeof Vector");
System::assert(typeof json_obj.e.d[0]==float	,"deserialize json_obj.e.d[0] != typeof float");
System::assert(typeof json_obj.e.d[1]==bool		,"deserialize json_obj.e.d[1] != typeof bool");
System::assert(typeof json_obj.e.d[2]==String	,"deserialize json_obj.e.d[2] != typeof String");


var json_str_dst=Json::serialize(json_obj)
System::assert(json_str_src==json_str_dst,"serialize A ('"+json_str_src+"') != B ("+json_str_dst+")")


