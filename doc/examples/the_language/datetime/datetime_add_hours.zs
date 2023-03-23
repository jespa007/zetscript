var now = DateTime::now();
Console::outln("now => {0}", now);
now.addHours(4);
Console::outln("after +4 hours from now => {0}", now);