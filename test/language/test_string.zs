var s0="1,2,3";
System::assert(s0.length==5,"s0.length != 5");

var v0=s0.split(",")
System::assert(v0.length==3,"v0.length != 3");
System::assert(v0[0]=="1","v0[0] != \"1\"");
System::assert(v0[1]=="2","v0[1] != \"2\"");
System::assert(v0[2]=="3","v0[2] != \"3\"");


var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")
System::assert(v.length==2,"v.length != 2");

System::assert(v[0]=="456","v[0] != \"456\"");
System::assert(v[1]=="678","v[1] != \"678\"");

// assert that we have 123
System::assert(s1=="123","s1!=\"123\"");

// Test modify character. In this case it has position s1[1] changing its current value  "123" by "1b3"
s1[1]='f'
System::assert(s1=="1f3","s1!=\"1f3\"");

// Test insertAt(_ch). Inserts 'a' character at position 1 changing its current value "1f3" by "1af3"
s1.insertAt(1,'a');
System::assert(s1=="1af3","s1!=\"1af3\"");

// Test insertAt(_str). Inserts 'a' character at position 1 changing its current value "1f3" by "1af3"
s1.insertAt(2,"bcde");
System::assert(s1=="1abcdef3","s1!=\"1abcdef3\"");

// Test eraseAt. In this case it erases position s1[1] changing its current value "1b3" by "13"
s1.eraseAt(1);
System::assert(s1=="1bcdef3","s1!=\"1bcdef3\"");

// Test +=
s1+="aaa";
System::assert(s1=="1bcdef3aaa","s1!=\"1bcdef3aaa\"");

// Test append. (append is like += )
s1.append("bbb");
System::assert(s1=="1bcdef3aaabbb","s1!=\"1bcdef3aaabbb\"");

// Test clear
s1.clear();
System::assert(s1=="","s1!=\"\"");

// Test indexOf. Get index first ocurrence of character 'b'
System::assert("abc".indexOf('b')==1,"\"abc\".indexOf('b')!=1");

// Test indexOf. Get index first ocurrence of string "world"
System::assert("hello world".indexOf("world")==6,"\"hello world\".indexOf(\"world\")!=6");

// Test startsWidth. Check whether string starts with "hello" 
System::assert("hello world".startsWith("hello"),"\"hello world\".startsWith(\"hello\")");

// Test endsWidth. Check whether string ends with "world" 
System::assert("hello world".endsWith("world"),"\"hello world\".endsWith(\"world\")");

// Test substring. Check whether substring == "hello world" 
System::assert("!!!hello world!!!".substring(3)=="hello world!!!","\"!!!hello world!!!\".substring(3)!=\"hello world!!!\"");
System::assert("!!!hello world!!!".substring(2,4)=="!he","\"!!!hello world!!!\".substring(2,4)!=\"!he\"");
System::assert("!!!hello world!!!".substring(3,-4)=="hello world","\"!!!hello world!!!\".substring(3,-3)!=\"hello world\"");

// Test replace string "world" by "people"
System::assert("hello world world".replace("world","people")=="hello people people","\"hello world\".replace(\"world\",\"people\")!=\"hello people people\"")

// Test format
System::assert(StringUtils::format("hello {0} {1}","people1","people2")=="hello people1 people2","StringUtils::format(\"hello {0} {1}\",\"people1\",\"people2\")!=\"hello people1 people2\"")
