var v1=[0,10,false]
var v2=["string_v1","string_v2"]
var v3=[]

Console::outln("v1: "+v1)
Console::outln("v2: "+v2)
Console::outln("v3: "+v3)

Console::outln("v3=v1+v2=>"+(v3=v1+v2))
Console::outln("[1,2,3].joint('_')=>"+[1,2,3].join('_'))

var i=0;

// 3 types iteration
// iterator by index
Console::outln("Iterator by index:")
for( i=0; i < v3.size(); i++){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
}
Console::outln()


// iterator value only
Console::outln("Iterator by value only:")
i=0;
for(var v in v3){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
	i++;
}
Console::outln()

// itarator pair k,v
Console::outln("Iterator by pair k,v:")
for(var k,v in v3){
	if(k>0){
		Console::out(",");
	}
	Console::out("({0},{1})",k,v);
	
}
Console::outln()

v3.push("pushed_3");
v3.push("pushed_4");
Console::outln("After 2 pushes:")
for( i=0; i < v3.size(); i++){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
}
Console::outln()

v3.pop();

Console::outln("After pop lat value:")
for( i=0; i < v3.size(); i++){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
}
Console::outln()
Console::outln("After insert 'string' value at position 1:")
v3.insertAt(1,"string")

for(i=0; i < v3.size(); i++){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
}
Console::outln()
v3.eraseAt(2)
Console::outln("After remove element at position 2:")
for(i=0; i < v3.size(); i++){
	if(i>0){
		Console::out(",");
	}
	Console::out(v3[i]);
}
Console::outln()



