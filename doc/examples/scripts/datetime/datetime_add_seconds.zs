var now = DateTime::now();
Console::outln("now => {0}", now);
now.addSeconds(10);
Console::outln("after +10 seconds from now => {0}", now);