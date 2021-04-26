var obj=Json::deserialize(
	"// Configuration options"+
	"{"+
	    "// Default encoding for text"+
	    "\"encoding\" : \"UTF-8\","+
	    "\"number\": 3.34E-5,"+
	    "// Plug-ins loaded at start-up"+
	    "\"plug-ins\" : ["+
	        "\"python\","+
	        "\"c++\","+
	        "\"ruby\""+
	      "],"+
	    "// Tab indent size"+"+
	    "\"indent\" : { \"length\" : 3, \"use_space\": true },"+
	    "// set of languages"+
	    "\"languages\":[{"+
	        "\"code\" : \"en\","+
		    "\"general_texts\": {"+
	            "\"general.hello_word\":\"Hello world!\""+
	            ",\"general.yes\":\"Yes\""+
	           " ,\"general.no\":\"No\""+
	        "}"+
		"},{"+
		    "\"code\" : \"es\","+
		    "\"general_texts\": {"+
	            "\"general.hello_word\":\"Hola mundo!\""+
	            ",\"general.yes\":\"Si\""+
	           " ,\"general.no\":\"No\""+
	        "}"+
		"},{"+
		    "\"code\" : \"zh-CN\","+
		    "\"general_texts\": {"+
	            "\"general.hello_word\":\"你好词\""+
	            ",\"general.yes\":\"是\""+
	           " ,\"general.no\":\"没有\""+
	        "}"+
		"}]"+
		"// set of interpolators"+
		",\"interpolations\":{"+
			"\"id_1\":{"+
				 "\"type\":\"material\""+
				",\"channels\":\"rgb\""+
				",\"data\":["+
				 "// r    g   b   t"+
				 "//---- --- --- ----"+
					"0.0,1.0,0.0,1000"+
					",0.0,0.0,0.0,0"+
				"]"+
			"},\"id_2\":{"+
				 "\"type\":\"transform\""+
				",\"channels\":\"xyz\""+
				",\"data\":["+
				 "// x    y   z   t"+
				  " //---- --- --- ----"+
					"0.0,1.0,0.0,1000"+
					",0.0,0.0,0.0,0"+
				"]"+
			"}"+
		"}"+
	"}"+
);

Console::println("result:"+Json::serialize(obj))