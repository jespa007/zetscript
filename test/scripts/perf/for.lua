
function test_sum()
	local list = {}
	for i = 0, 999999 do
	  list[i] = i
	end
	
	local sum = 0
	for k, i in pairs(list) do
	  sum = sum + i
	end
	return sum
end

for i = 1, 10 do
  local start = os.clock()
  io.write(string.format("sum: %d time: %.8f\n",test_sum(), os.clock() - start))
end

