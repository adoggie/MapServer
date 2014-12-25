mapserver
=========

地图绘制服务器

渲染效果可见:  http://114.215.178.29:8080/wms/

mapserver提供地理网格图形绘制服务(tile renderring),通信部分采用tce的RPC调用(见 wms-server).
最早采用qt作为图形库，但其在windows平台下多线程(multithread)没问题，到了linux却无法工作，原因
在于qt的图形部件类都必须在ui主线程工作，导致多线程工作在signal/slot之后变成了但线程处理，性能不佳，
最终还是选用cairo，在64bit系统下表现可以.

mapserver本是个多年前开始的项目，最早在2009做出了第一个版本，当初的目的是为了一个wince的导航地图而启动开发计划。

mapserver也是个人爱好项目，期间不乏颇多的不合理和混乱。
mapserver的windows版本已经被停止，linux的版本基于cairo和qt，如果需移植到android和ios，也需要一定的工作量来替换掉工程中使用的
一些图形ui类和基础数据包装类型

环境
====
* centos 6.+
* libcairo , cairomm
* eclipse+cdt
* libtce
