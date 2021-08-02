var obj={
	a:0
	,b:10
	,c:"k"
	,d:true
	,obj:{
		k1:0
		,k2:2
	}
};

for(var v in obj){
	 Console::outln("v:{0}",v);
}

for(var k,v in obj 	){
	 Console::outln("k[\"{0}\"]:{1}",k,v);
}

