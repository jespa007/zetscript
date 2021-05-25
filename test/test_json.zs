var obj=Json::deserialize(
	"// Configuration options\n"+
	"{\n"+
	    "// Default encoding for text\n"+
	    "\"encoding\" : \"UTF-8\",\n"+
	    "\"number\": 3.34E-5,\n"+
	    "// Plug-ins loaded at start-up\n"+
	    "\"plug_ins\" : [\n"+
	        "\"python\",\n"+
	        "\"c++\",\n"+
	        "\"ruby\"\n"+
	      "],\n"+
	    "// Tab indent size\n"+
	    "\"indent\" : { \"length\" : 3, \"use_space\": true },\n"+
	    "// set of languages\n"+
	    "\"languages\":[{\n"+
	        "\"code\" : \"en\",\n"+
		    "\"general_texts\": {\n"+
	            "\"general.hello_word\":\"Hello world!\"\n"+
	            ",\"general.yes\":\"Yes\"\n"+
	           " ,\"general.no\":\"No\"\n"+
	        "}\n"+
		"},{\n"+
		    "\"code\" : \"es\",\n"+
		    "\"general_texts\": {\n"+
	            "\"general.hello_word\":\"Hola mundo!\"\n"+
	            ",\"general.yes\":\"Si\"\n"+
	           " ,\"general.no\":\"No\"\n"+
	        "}\n"+
		"},{\n"+
		    "\"code\" : \"zh-CN\",\n"+
		    "\"general_texts\": {\n"+
	            "\"general.hello_word\":\"你好词\"\n"+
	            ",\"general.yes\":\"是\"\n"+
	           " ,\"general.no\":\"没有\"\n"+
	        "}\n"+
		"}]\n"+
		"// set of interpolators\n"+
		",\"interpolations\":{\n"+
			"\"id_1\":{\n"+
				 "\"type\":\"material\"\n"+
				",\"channels\":\"rgb\"\n"+
				",\"data\":[\n"+
				 "// r    g   b   t\n"+
				 "//---- --- --- ----\n"+
					"0.0,1.0,0.0,1000\n"+
					",0.0,0.0,0.0,0\n"+
				"]\n"+
			"},\"id_2\":{\n"+
				 "\"type\":\"transform\"\n"+
				",\"channels\":\"xyz\"\n"+
				",\"data\":[\n"+
				 "// x    y   z   t\n"+
				  " //---- --- --- ----\n"+
					"0.0,1.0,0.0,1000\n"+
					",0.0,0.0,0.0,0\n"+
				"]\n"+
			"}\n"+
		"}\n"+
	"}"
);


Console::outln("result:"+Json::serialize(obj,true))

