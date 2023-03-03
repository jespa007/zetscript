class A{}
class B extends A{}

Console::outln("10 instanceof Integer => " + (10 instanceof Integer))
Console::outln("10 instanceof Float => " + (10 instanceof Float))
Console::outln("10.5 instanceof Integer => "+ (10.5 instanceof Integer))
Console::outln("10.5 instanceof Float => "+ (10.5 instanceof Float))
Console::outln("\"string\" instanceof String  =>  "+ ("string" instanceof String))
Console::outln("[] instanceof Array  => "+ ([] instanceof Array))
Console::outln("{} instanceof Object => "+ ({} instanceof Object))
Console::outln("function(){} instanceof Function  => "+ (function(){} instanceof Function))
Console::outln("new A() instanceof A => "+ (new A() instanceof A))
Console::outln("new B() instanceof A => "+ (new B() instanceof A))
Console::outln("new A() instanceof B => "+ (new A() instanceof B))
