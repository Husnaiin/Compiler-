; ModuleID = 'toy_module'
source_filename = "toy_module"

@global_x = global i32 0
@global_y = global i32 0

define i32 @add(i32 %a, i32 %b) {
entry:
  %b.addr = alloca i32
  %a.addr = alloca i32
  %t0.addr = alloca i32
  store i32 %a, i32* %a.addr
  store i32 %b, i32* %b.addr
  %tmp0.l = load i32, i32* %a.addr
  %tmp0.r = load i32, i32* %b.addr
  %tmp0.add = add i32 %tmp0.l, %tmp0.r
  store i32 %tmp0.add, i32* %t0.addr
  %tmp1.r = load i32, i32* %t0.addr
  ret i32 %tmp1.r
  ret i32 0
}

define i32 @add(i32 %a, i32 %b, i32 %c) {
entry:
  %a.addr = alloca i32
  store i32 %a, i32* %a.addr
  %b.addr = alloca i32
  store i32 %b, i32* %b.addr
  %c.addr = alloca i32
  store i32 %c, i32* %c.addr
  %tmp0.r = add i32 0, 0
  ret i32 %tmp0.r
  ret i32 0
}

define i32 @main() {
entry:
  %local_y.addr = alloca i32
  %global_y.addr = alloca i32
  %global_x.addr = alloca i32
  %local_x.addr = alloca i32
  %nested.addr = alloca i32
  %t0.addr = alloca i32
  %tmp0.x = load i32, i32* %local_x.addr
  store i32 %tmp0.x, i32* %nested.addr
  %tmp1.x = load i32, i32* %global_x.addr
  store i32 %tmp1.x, i32* %local_x.addr
  %t0.call = call i32 @add(i32 0, i32 0)
  store i32 %t0.call, i32* %t0.addr
  %tmp5.x = load i32, i32* %t0.addr
  store i32 %tmp5.x, i32* %local_y.addr
  %tmp6.r = add i32 0, 0
  ret i32 %tmp6.r
  ret i32 0
}

