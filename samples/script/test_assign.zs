function ret_expression_sequence_3(){
	return 1,2,3;
}

function ret_expression_sequence_2(){
	return 1,2;
}


// var declaration
var a,b,c,d,e,f;

//test 0: one value right, multiple value left
a=b=c=10
System::assert(a==10,"Test0. a!=10");
System::assert(b==10,"Test0. b!=10");
System::assert(c==10,"Test0. c!=10");

d=e=f=a*2
System::assert(d==20,"Test0. d!=20");
System::assert(e==20,"Test0. e!=20");
System::assert(f==20,"Test0. f!=20");


a,b,c,d,e,f=0,1,2,[0,1,2,3];

// test 1: multi-assigment
a,b,c=0,1,2
System::assert(a==0,"Test1. a!=0");
System::assert(b==1,"Test1. b!=1");
System::assert(c==2,"Test1. c!=2");

// test 2: multi-assigment: puts null the ones that has no assignment on the right
a,b,c=3,4
System::assert(a==3,"Test2. a!=3");
System::assert(b==4,"Test2. b!=4");
System::assert(c==null,"Test2. c!=null");

// test 3: multi-assigment: ignore excess of assigments on the right
a,b,c=3,4,5,6,7,8,9
System::assert(a==3,"Test3. a!=3");
System::assert(b==4,"Test3. b!=4");
System::assert(c==5,"Test3. c!=5");

//test 4: multi-assigment: swap values
a,b=1,2
a,b=b,a
System::assert(a==2,"Test4. a!=2");
System::assert(b==1,"Test4. b!=1");

// test 5: multi-assigment: test function returns 3 values and assigns 3 vars on the left
a,b,c=ret_expression_sequence_3()
System::assert(a==1,"Test5. a!=1");
System::assert(b==2,"Test5. b!=2");
System::assert(c==3,"Test5. c!=3");

//test 6: multi-assigment: test function returns 2 values and assigns 3 vars on the left
a,b,c=ret_expression_sequence_2()
System::assert(a==1,"Test6. a!=1");
System::assert(b==2,"Test6. b!=2");
System::assert(c==null,"Test6. c!=null");
