Lua中有8个基本类型: nil, boolean, number, string, userdata, function, thread, table.
函数type可以测试给定变量或值的类型:
print(type("Hello World"))

lua中没有整数，用实数代替整数。
lua中字符串不可修改的，可以创建新的字符串。
连接运算符 .. :
> print( "hello" .. 123 )
hello123
> print( 456 .. 123 )
456123
> print(type( 456 .. 123 ))
string

table表是lua特有的功能强大的东西，最简单的构造函数是{}, 用来创建一个空表。
days = { "Sunday", "Monday" }

返回多相结果:
> s, e = string.find("hello Lua users", "Lua")
> print(s,e)

协同程序(coroutine)与多线程情况下的线程比较类似：有自己的堆栈，自己的局部变量，有自己的指令指针，但是和其他协同程序共享全局变量等很多信息.

