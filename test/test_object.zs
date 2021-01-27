var object={ // structure
	i: 10,
	f: 0.5,
	s: "a string",
	b: true,
	v: [1,0.5, "a string", true]
};


// add property test
object.k=false;
object["k"]=false;

print("dictionary.k:"+object.k);

/*for(var k in dictionary){
	
}*/
/*class A{
	
	var j;
	
	function A(){
		this.j=-10;
	}
};

var s={
	i:0+1,
	j:"string1",
	n: 1.0, // like struct  
	f:function(){return 2+1;},
	k:[0,1,2,3],
	l:{
		i:2+3,
		j:"string2",
		k:[4,5,6,7],
		m:[
			{
				i:4+5,
				j:"string3",
				k:[8,9,10,11]
			},
			{
				i:6+7,
				j:"string4",
				k:[12,13,14,15]
			}
			]
	}
		
};


print("s.f():"+s.f());
s.add("a",new A());
s.add("b","dadas");
print("s.a.j:"+s.a.j);

s.remove("a");
s.remove("b");*/




