var object={ // structure
	i: 10,
	f: 0.5,
	s: "a string",
	b: true,
	v: [1,0.5, "a string", true]
};


// check it can add property 'k' through '.'
object.k=false;

System::assert(object.k==false,"object.k!=false")

// check it can also add properties through []
object["general.info"]=false;
System::assert(object["general.info"]==false,"object[\"general.info\"]!=false");

var obj1={a:1,b:2}
var obj2={c:3,d:4}

// get object keys for iteration...
var key1=Object::keys(obj1);
var key2=Object::keys(obj2);
var concat_keys="";

// iteration through the keys
for(var i=0; i < key1.length; i++){
	concat_keys+=key1[i];
}

System::assert(concat_keys=="ab","keys(obj1) failed, concat_keys!=\"ab\"")

System::assert(Object::contains(obj1,"a")==true,"obj1 does not have key 'a'")


var obj3=obj1+obj2;
var key3=Object::keys(obj3);
concat_keys="";

for(var i=0; i < key3.length; i++){
	concat_keys+=key3[i];
}

System::assert(concat_keys=="abcd","keys(obj3) failed, concat_keys!=\"abcd\"")

Object::clear(obj3)
key3=Object::keys(obj3);
concat_keys="";

for(var i=0; i < key3.length; i++){
	concat_keys+=key3[i];
}

System::assert(concat_keys=="","keys(obj3) after clear failed, concat_keys!=\"\"")






