function fib(n)
  if n < 2 then return n end
  return fib(n - 2) + fib(n - 1)
end

for i = 1, 10 do
  local start = os.clock()
  io.write(string.format("fib: %d time: %.8f\n",fib(34), os.clock() - start))
end




