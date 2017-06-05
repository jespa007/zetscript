var mask_and=0x00ff001f & 0x00ff00ff;
var mask_or=0xff00 | 0xff;
var mask_xor=0x00 ^ 0xff;
var mask_not= 0xff^1;
var mask_shift_right = 0xffff >> 8;
var mask_shift_left  = 0x00ff << 8;

print("mask_and:"+mask_and);
print("mask_or:"+mask_or);
print("mask_xor:"+mask_xor);
print("mask_not:"+mask_not);
print("mask_shift_right:"+mask_shift_right);
print("mask_shift_left:"+mask_shift_left);