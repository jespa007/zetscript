class A{}

System::assert(typeof 0==Integer,"typeof 0 != Integer")
System::assert(typeof 1.0==Float,"typeof 1.0 != Float")
System::assert(typeof true==Boolean,"typeof true != Boolean")
System::assert(typeof "str"==String,"typeof \"str\" != String")
System::assert(typeof [0,1,2]==Vector,"typeof [0,1,2] != Vector")
System::assert(typeof {a:0,b:1}==Object,"typeof {a:0,b:1} != Object")
System::assert(typeof new A()==A,"typeof new A() != A")




