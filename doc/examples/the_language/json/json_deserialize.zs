var json_string="{\n"+
	    "\"encoding\" : \"UTF-8\",\n"+
	    "\"number\": 3.34E-5\n"+
	    ",\"plug_ins\" : [\n"+
	        "\"python\",\n"+
	        "\"c++\",\n"+
	        "\"ruby\"\n"+
	      "]\n"+
	    ",\"indent\" : { \"length\" : 3, \"use_space\": true }\n"+
	"}"

var deseriazed_json_string=Json::deserialize(json_string);
Console::outln("Deserialized json result:\n")
for(var k,v in deseriazed_json_string){
  	Console::outln("key => '{0}' value => {1} ",k,v)	    	
}
