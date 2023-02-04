function sum(...args) {
  var sum = 0;
  for (var arg in args) {
  		sum += arg;
  }
  return sum;
}

Console::outln("sum = "+sum(4, 9, 16, 25, 29, 100, 66, 77));