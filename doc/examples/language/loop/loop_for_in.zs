var object={a:10,b:10.5,c:"string",d:true}

Console::outln("Iterate object and get value\n")

for(var v in object){
	Console::outln("v => "+v);
}

Console::outln("\nIterate object and get pair key-value\n")

for(var k,v in object){
	Console::outln("k => "+k+" v => "+v);
}



