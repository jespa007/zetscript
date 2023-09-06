var object={ // structure
	i: 10,
	f: 0.5,
	s: "a string",
	b: true,
	v: [1,0.5, "a string", true]
};


// add property 'k' through '.'
object.k=false;
Console::outln("object.k:"+object.k);

// you can also add properties through []
object["general.info"]=false;
Console::outln("object[\"general.info\"]:"+object["general.info"]);

var obj1={a:1,b:2}
var obj2={c:3,d:4}

// get object keys for iteration...
var key1=Object::keys(obj1);
var key2=Object::keys(obj2);

for(var i=0; i < key1.length; i++){
	Console::outln(key1[i]);
}

if(Object::contains(obj1,"a")){
	Console::outln("a exist")
}

var obj3=obj1+obj2;
var key3=Object::keys(obj3);

for(var i=0; i < key3.length; i++){
	Console::outln(key3[i]);
}

Object::clear(obj3)

key3=Object::keys(obj3);

for(var i=0; i < key3.length; i++){
	Console::outln(key3[i]);
}






