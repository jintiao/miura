# miura
real-time ocean rendering using opengl

## 简介
使用opengl实现实时海洋渲染。

![screenshot.jpg](https://github.com/jintiao/miura/raw/master/ref/screenshot.jpg)

## 运行

使用cmake生成项目文件然后编译运行即可。
请使用支持c++11标准的编译器进行编译。

### visual studio 2015
在项目目录下执行
"mkdir build && cd build && cmake -G "Visual Studio 14 2015 Win64" .. && cd .."
然后打开build/miura.sln编译运行

### xcode
在项目目录下执行
"mkdir build; cd build; cmake -G "Xcode" ..; cd .."
然后打开build/miura.xcode编译运行

## 技术细节

## 参考资料
* Jerry Tessendorf, “Simulating Ocean Water”.
* Paul Bourke, "2 Dimensional FFT".
