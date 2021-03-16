var v1=[1,2]
var v2=[4,5]
var v4=["s1","s2","s3","s4"]

v1.push(3);

for(var i=0; i < v1.size(); i++){
	System::println(v1[i]);
}

var item=v1.pop();

for(var i=0; i < v1.size(); i++){
	System::println(v1[i]);
}

System::println("---")
System::println(item)
System::println("---")

//System::println("---")

v2.insertAt(1,"string")

for(var i=0; i < v2.size(); i++){
	System::println(v2[i]);
}

System::println("---")

v2.eraseAt(1)

for(var i=0; i < v2.size(); i++){
	System::println(v2[i]);
}

System::println("---")

var v3=v1+v2

for(var i=0; i < v3.size(); i++){
	System::println(v3[i]);
}

System::println("---")

v3.clear()

for(var i=0; i < v3.size(); i++){
	System::println(v3[i]);
}

System::println("---")

System::println(v4.join('_'))

var v1d=["George",0,true]

var v2d=[
	 [0,1,2,3]
	,[0,1,2,3]
]

//vv[0]++;

/*var i=0;
i++;*/

/*vv[1]++; // increments 0 to 1...

vv.pop(); // pops true

vv["grow"]=100; // adds a container elements...


foreach(var e in vv){
	print("e: "+e);
}

print("vv[\"grow\"]="+vv["grow"]);*/