var v1=["v1","v2"]
var v2=[4,5]
var v4=["s1","s2","s3","s4"]

v1.push("v3");

// 3 types iteration

// iterator by index
Console::outln("Iterator by index:")
for(var i=0; i < v1.size(); i++){
	Console::outln("v:{0}",v1[i]);
}


// iterator value only
Console::outln("Iterator by value only:")
for(var v in v1){
	Console::outln("v:{0}",v);
}

// itarator pair k,v
Console::outln("Iterator by pair k,v:")
for(var k,v in v1){
	Console::outln("k:{0} v:{1}",k,v);
}


var item=v1.pop();

for(var i=0; i < v1.size(); i++){
	Console::outln(v1[i]);
}

Console::outln("---")
Console::outln(item)
Console::outln("---")

//Console::outln("---")

v2.insertAt(1,"string")

for(var i=0; i < v2.size(); i++){
	Console::outln(v2[i]);
}

Console::outln("---")

v2.eraseAt(1)

for(var i=0; i < v2.size(); i++){
	Console::outln(v2[i]);
}

Console::outln("---")

var v3=v1+v2

for(var i=0; i < v3.size(); i++){
	Console::outln(v3[i]);
}

Console::outln("---")

v3.clear()

for(var i=0; i < v3.size(); i++){
	Console::outln(v3[i]);
}

Console::outln("---")

Console::outln(v4.join('_'))

