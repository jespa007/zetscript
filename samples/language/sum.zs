var sum = 0.0;

for (var i = 1; i <= 1000000; ++i) {
    if (i % 2 == 0) {
    	sum += 1.0 / i;
    }
    else {
    	sum += 1.0 / ((i) * i);
    }
}
Console::outln("number:"+sum);
