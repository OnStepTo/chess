一款用stm32f103单片机制作的中国象棋,

程序主要代码：
main.c 
chess.c
displayChess.c
check.c
加上正点原子的驱动和自己添加的少量驱动,程序编译有很多警告，但不影响使用

特别感谢B站网友“领航员a”将其移植到mini开发板


 **已适配平台：正点原子（精英、迷你）、 信盈达开发板** 

正点原子精英板，已经适配好2.8寸和4.3寸屏幕。一个程序两种屏幕共用。
如果是其它的正点原子的屏幕，留好了接口，在displayChess.c里面更改参数即可。

如果是战舰版，2.8寸屏不用移植，其它屏的话，则要找到HARDWARE/transplant文件夹，把里面的文件夹用战舰版例程里相应的文件夹替换

其它的板子移植的话，主要是更改驱动和displayChess.c,把displayChess.c里的函数的功能实现出来，供上层函数调用。

转载代码请注明出处