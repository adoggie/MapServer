# -*- coding:utf-8 -*-

#splitTiles.py
#切割指定地理区域的网格参数，为了是提供TileServer在生成缓冲网格的参数
#为每一个缓冲网格区域生成xml配置信息

import os,os.path,sys,time,math

METERS_PER_MINUTE=1860.0
MINUTES_PER_DEGREE=60.0

METERS_PER_DEGREE = METERS_PER_MINUTE*MINUTES_PER_DEGREE

DPI=96.0
METERS_PER_PIXEL= 0.0254/DPI

RESOLUTION = 0.0
SCALE=5000

VIEWEXT=256




def setScale(s):
	global SCALE,RESOLUTION
	RESOLUTION = METERS_PER_PIXEL*s/METERS_PER_DEGREE
	SCALE = s

def setResolution(r):
	global SCALE,RESOLUTION
	SCALE = r*METERS_PER_DEGREE/METERS_PER_PIXEL
	RESOLUTION = r
	
def s2r(s):
	global SCALE,RESOLUTION
	setScale(s)
	return RESOLUTION
	
def r2s(r):
	global SCALE,RESOLUTION
	setResolution(r)
	return SCALE
	
def reset():
	global SCALE,RESOLUTION
	SCALE = 0
	RESOLUTION=0

	
def xy_view2geo(res,gcxy,vxy,vsize):	
	offx = (vxy[0] - vsize[0]/2)*res*1.0
	offy =  -(vxy[1] - vsize[1]/2)*res*1.0
	return (gcxy[0]+offx,gcxy[1]+offy)
	
def xy_geo2view(res,gcxy,gxy,vsize):
	offx = (gxy[0]-gcxy[0])/(res*1.0)
	offy = -(gxy[1]-gcxy[1])/(res*1.0)
	return (vsize[0]/2.0+offx,vsize[1]/2.0+offy)
	
def geoExt(res,vlen):
	return res*vlen

def viewExt(res,glen):
	return res/glen


def geoRect(res,gcxy,vrc):
	offx = gcxy[0] - vrc[2]/2.0*res
	offy = gcxy[0] - vrc[3]/2.0*res
	
	pass

#计算地理坐标点在网格中的编号 	
# o - original coordinate 
def geoPoint2TileIndex(res,o,gxy,tilevsize):
	gtw = tilevsize[0]*res
	gth = tilevsize[1]*res
	offx =  (gxy[0]-o[0])/gtw
	offy = (gxy[1]-o[1])/gth
	offx = int(offx)
	offy = int(offy)
	return offx,offy
	
def TileIndex2geoPoint(res,o,xy,tilevsize):
	gtw = tilevsize[0]*res
	gth = tilevsize[1]*res
	x = gtw*xy[0]+o[0]
	y = gth*xy[1]+o[1]
	return (x,y)
	
#####################################

# print r2s(0.0310000)
# print s2r(r2s(0.0310000))

# print xy_view2geo(0.0034,(121.40,30.34),(250,150),(400,400))
# print xy_geo2view(0.0034,(121.40,30.34),(121.57,30.51),(400,400))

x,y =  geoPoint2TileIndex(s2r(500),(70.0,10.0),(121.33,30.44),(256,256))

x,y =  geoPoint2TileIndex(s2r(5000),(70.0,10.0),(180,90),(256,256))

###########################################
#调整一下参数，用于生成缓冲绘制的任务
ORIGIN=(70.0,10.0) #
TILE_VIEWSIZE=(256,256)
#TARGET_GEORECT=(121.0,30.0,1.0,1.0) #指定一度的范围  上海(120.794,30.70 - 122.175,31.89)
TARGET_GEORECT=(120.794,30.70 ,122.175-120.794,31.89-30.70 ) #指定一度的范围  上海(120.794,30.70 - 122.175,31.89)

#SCALES=(5000,10000,20000,50000) #显示比例,下标就是缩放级别 0 -n
#SLICE_NUM=(2,10,5,3) #对应SCALES，每个显示比例被分割为n部分
SCALES=(5000,15000,30000,60000,100000,160000,240000,300000,400000)
SLICE_NUM=(1,1,1,1,1,1,1,1,1) #对应SCALES，每个显示比例被分割为n部分

SCALES=(3500 ,5000, 7500 ,10000 ,15000, 20000, 35000 ,50000 ,75000, 100000, 150000, 200000,300000,400000,600000,800000)
SLICE_NUM=(1,)*len(SCALES)

MAPID=0	#地图编号
PATH='./mapdata'
TASKPATH='./maptask'
TASKNAME='shanghai'	#网格文件名称

#缓冲网格任务文件命名: 任务名称_缩放级别_切割部分编号

#google地图分析:
# 20级缩放，比例 : 20 50 100 200 500 1000 ...
# 最小比例： 76像素 20米
###########################################
def splitTileTask():
	for s in range(len(SCALES)):
		gpt_lb = TARGET_GEORECT[:2]
		gpt_rt = (gpt_lb[0]+TARGET_GEORECT[2],gpt_lb[1]+TARGET_GEORECT[3])
		
		x1,y1 = geoPoint2TileIndex(s2r(SCALES[s]),ORIGIN,gpt_lb,TILE_VIEWSIZE)
		x2,y2 = geoPoint2TileIndex(s2r(SCALES[s]),ORIGIN,gpt_rt,TILE_VIEWSIZE)
		
		#print (x1,y1),(x2,y2)
		#print TileIndex2geoPoint( s2r(SCALES[s]),ORIGIN,(x2,y2),TILE_VIEWSIZE)
		
		#按行进行切割
		parts = SLICE_NUM[s]
		rows = (y2-y1+1)/parts
		rows = int(rows)
		left = (y2-y1+1) - rows*parts
		if  left >0:
			parts +=1
		
		count=0
		for p  in range(parts): #切割为n个任务
			yy1 = y1+ p*rows
			yy2 = yy1 + rows -1
			if left > 0:
				yy2 = yy1+left-1
			f= open('%s/task_%s_%s_%s.xml'%(TASKPATH,TASKNAME,SCALES[s],count),'w')
			xml = ''
			xml+='<TileTask>\n'
			xml+='<Index>%s</Index>\n'%count
			xml+='<MapId>%s</MapId>\n'%MAPID
			xml+='<Origin>%s %s</Origin>\n'%ORIGIN
			xml+='<TileSize>%s %s</TileSize>\n'%TILE_VIEWSIZE
			
			xml+='<TileIndexRect>%s %s %s %s</TileIndexRect>\n'%(x1,yy1,x2,yy2)
			#print (x1,yy1,x2,yy2)
			#print TileIndex2geoPoint( s2r(SCALES[s]),ORIGIN,(x1,yy1),TILE_VIEWSIZE)
			#print TileIndex2geoPoint( s2r(SCALES[s]),ORIGIN,(x2,yy2),TILE_VIEWSIZE)
			xml+='<Scale>%s</Scale>\n'%(SCALES[s])
			xml+='<ScaleLevel>%s</ScaleLevel>\n'%s
			xml+='<Path>%s</Path>\n'%(PATH)
			xml+='<Name>%s</Name>\n'%(TASKNAME)
			
			
			gw = s2r(SCALES[s])*TILE_VIEWSIZE[0]
			gh = s2r(SCALES[s])*TILE_VIEWSIZE[1]
			gpt = TileIndex2geoPoint( s2r(SCALES[s]),ORIGIN,(x1,yy1),TILE_VIEWSIZE)
			
			xml+='<TileGeoSize>%s %s</TileGeoSize>\n'%(gw,gh)
			xml+='<GeoRect>%s %s %s %s</GeoRect>\n'%(gpt[0],gpt[1],gw,gh)
			
			xml+='</TileTask>'
			f.write(xml)
			f.close()
			count+=1

	
############################33
ExeName='TileServer.exe'
if __name__=='__main__':
	if len(sys.argv[1:]):
		if sys.argv[1] == 'createbatch': #创建批处理调用
			for f in os.listdir(TASKPATH):						
				if os.path.splitext(f)[1].lower()=='.xml':
					print '%s -create %s/%s'%(ExeName,TASKPATH,f)
		else: 
			print 'Error:unsupported command'
		sys.exit()
		
	splitTileTask()
