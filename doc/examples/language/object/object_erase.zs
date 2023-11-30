var o={a:1,b:2,c:3,d:4};

Console::outln("o => {0}",o)
ScriptObject::erase(o,"b");
Console::outln("ScriptObject::erase(o,\"b\") => {0}",o)