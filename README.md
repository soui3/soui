<div align= "center">
<img align="center" width=200 src="./doc/logo.png" />
</div>


<div align= "center">
 <h1>SOUI 3</h1>

 <a>
    <img alt="DirectUI" src="https://img.shields.io/badge/C%2B%2B-DirectUI%E5%BA%93-brightgreen?style=for-the-badge" />
 </a>
 <a href="https://github.com/soui3/soui/releases">
    <img alt="releases" src="https://img.shields.io/github/v/release/soui3/soui?color=blueviolet&include_prereleases&style=for-the-badge" />
 </a>
<a href="https://github.com/soui3/soui/">
    <img alt="star" src="https://img.shields.io/github/stars/soui3/soui?style=for-the-badge">
</a>

 <a href="https://2926295173.github.io/SoUIDocs">
    <img alt="usage" src="https://img.shields.io/badge/%E4%BD%BF%E7%94%A8-%E6%96%87%E6%A1%A3-yellow?style=for-the-badge"/>
</a>


</div>


 _SOUI3_ 是一套已经持续开发维护 **12年** 的轻量级Windows平台客户端开发框架,
（内核编译后体积1M左右），
框架设计吸收了`WTL`，`Android`，`QT`，`Flash`等流行框架的特点，
UI展示基于`xml`配置，性能高效（可选择GDI或[Skia](https://www.oschina.net/p/skia)渲染），
控件丰富，扩展灵活（可使用 _xml_ + _lua_ ），真正左到UI界面与逻辑分离。



##  **demo基本界面** 
![demo1](./doc/demo_png/souidemo1.png)
![demo2](./doc/demo_png/souidemo2.png)

如果您初次了解此库，可以下载自动化静态编译的[demo.exe](https://github.com/soui3/soui/actions)进行尝鲜体验（无需下载源代码和其他依赖，直接下载运行）
具体方法
[![CMake Test](https://github.com/soui3/soui/actions/workflows/cmaketest.yml/badge.svg)](https://github.com/soui3/soui/actions/workflows/cmaketest.yml)

```
1.点击上方图标,进入页面
2 随便点击一个绿色圆圈后面的链接进入
3 下拉界面，找到`Artifacts`栏
4 点击栏中的`PC_Demo`进行下载解压运行即可。
```
如您想自己编译更多的界面案例，请拉取 DEMO仓库：

- https://github.com/soui3-demo

每一个Demo一个单独的仓库，自己分别clone就好了。

如果想一次拉取所有demo,可以直接运行SOUI仓库代码下的`clone_demos.bat`来clone，当然前提是已经安装了git命令行工具。


##  **开始** 

使用SOUI需要一定的c++与win32的基础，请您确保您掌握了上述技能。

除本页说明外，您还可以查看更详细的[官方文档](https://2926295173.github.io/SoUIDocs/)

### Git下载地址(SOUI3)

- [国外Git下载站点](https://github.com/soui3/soui)

- [国内Git下载站点](https://gitee.com/setoutsoft/soui3)(作为镜像使用)

### Git下载地址(SOUI4)，SOUI4在SOUI3的基础上调整了内核代码结构，导出类COM接口，方便C语言调用
- [国外Git下载站点](https://github.com/soui4/soui)

- [国内Git下载站点](https://gitee.com/setoutsoft/soui4)(作为镜像使用)

### [编译](./doc/build.md)

[![CMake Test](https://github.com/soui3/soui/actions/workflows/cmaketest.yml/badge.svg)](https://github.com/soui3/soui/actions/workflows/cmaketest.yml)

soui可以使用cmake或者qmake进行编译（前提:安装好Visual Studio），具体编译方法查看附带[编译文档](./doc/build.md)

### UI可视化设计

`SOUI3 Editor`,仓库地址https://github.com/soui3-demo/SouiEditor

使用教程见文档

### 其他教程

- 阅读作者[博客](http://www.cnblogs.com/setoutsoft/)，能让你更快掌握如何使用SOUI快速开发Window桌面应用

- 作者录制的b站[视频教程](https://space.bilibili.com/110611388)

## 使用soui的软件展示

请点击[链接](https://2926295173.github.io/SoUIDocs/Show/show/)查看，持续更新

##  **QQ群** 

- SOUI讨论#1: [点击加入](http://shang.qq.com/wpa/qunwpa?idkey=9653a811a72365d798a5247d6ba6885a568bdcf51c624f906c8ce7b8fd9e4eda)

- SOUI讨论#2: [点击加入](http://shang.qq.com/wpa/qunwpa?idkey=03d3294a2551beb1b54b4012086cec14b3f66d5c253debaeed241d9c623966e0)

##  **LICENSE** 

请查看[`license.txt`](./license.txt)文档