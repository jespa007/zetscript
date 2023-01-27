var o={
	  a:1
	  ,b:2
	  ,c:3
	  ,d:4
};

Console::outln("o => {0}",o)
Object::erase(o,"b");
Console::outln("Object::erase(o,\"b\") => {0}",o)