mapserver
=========

###地图绘制服务器

渲染效果可见:  http://www.wallizard.com:10092
<div>
<img src="./doc/image01.jpg" width="400px" height="320px"/>
</div>
<p></p>

  mapserver提供地理网格图形绘制服务(tile renderring),通信部分采用tce的RPC调用(见 wms-server).最早采用qt作为图形库，但其在windows平台下多线程(multithread)没问题，到了linux却无法工作，原因在于qt的图形部件类都必须在ui主线程工作，导致多线程工作在signal/slot之后变成了但线程处理，性能不佳，最终还是选用cairo，在64bit系统下表现可以.
	
  mapserver本是个多年前开始的项目，最早在2009做出了第一个版本，当初的目的是为了一个wince的导航地图而启动开发计划。

  mapserver也是个人爱好项目，期间不乏颇多的不合理和混乱。
  
  mapserver的windows版本已经被停止，linux的版本基于cairo和qt，如果需移植到android和ios，也需要一定的工作量来替换掉工程中使用的一些图形ui类和基础数据包装类型

docker 
=====

	地图系统由若干个服务构成，已经打包成 docker 镜像  sw2us_webgis_0.2.1.docker 
	
	1. 下载 docker 镜像, 挂载image  
		docker load -i sw2us_webgis_0.2.1.docker
		
	2. 启动docker ： 
		docker run --name webgis  -it -v /home:/data -p 10092:8080  webgis:0.2.2 /bin/bash
		
	3 .启动服务: 
		cd /home/sw2us
		bash start.sh    

	4.访问 ：  浏览器打开 http://192.168.10.22:10092  
	

###Image Download
1. 2015.11.27 <a href="http://pan.baidu.com/s/1bn7cXVh">sw2us_webgis_0.2.2.docker.tar.gz</a>      

环境
====
* centos 6.+
* libcairo , cairomm
* eclipse+cdt
* libtce
* qt4-devel
* mongoose-httpserver
* docker-1.7.1
