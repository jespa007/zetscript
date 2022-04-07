var v1=[1,2]
var v2=[4,5]
var v4=["s1","s2","s3","s4"]

//check push
v1.push("s");
var k_result=0
var v_result=""
for(var k,v in v1){
	k_result+=k
	v_result+=v_result;
}

//check pop
v1.pop();
k_result=0
v_result=""
for(var k,v in v1){
	k_result+=k
	v_result+=v_result;
}


//check insertAt
v2.insertAt(1,"string")
k_result=0
v_result=""
for(var k,v in v2){
	k_result+=k
	v_result+=v_result;
}

//check eraseAt
v2.eraseAt(1)
k_result=0
v_result=""
for(var k,v in v2){
	k_result+=k
	v_result+=v_result;
}

// check concatenation
var v3=v1+v2
k_result=0
v_result=""
for(var k,v in v3){
	k_result+=k
	v_result+=v_result;
}

// check clear
v3.clear()
k_result=0
v_result=""
for(var k,v in v3){
	k_result+=k
	v_result+=v_result;
}

// check join
k_result=0
v_result=""
for(var k,v in v4.join('_')){
	k_result+=k
	v_result+=v_result;
}


