function sum(str,...args) {
  var sum = 0;
  for (var arg in args) {
  		sum += arg;
  }
  return str+sum;
}

var res=sum("Total sum is = ",1, 10, 100, 1000, 10000);

Console::outln(res)