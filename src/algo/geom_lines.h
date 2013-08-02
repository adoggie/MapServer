#ifndef _GEOM_ALGORITHM_LINES_H
#define _GEOM_ALGORITHM_LINES_H

#include "../base.h"

typedef swFloatPoint swga_Coordinate;

typedef std::deque<swga_Coordinate> swga_line;		//多线段折线





//typedef std::vector< swga_lineseg*> swga_multiline;		//多线段折线



//表示折线某一头的端点
// struct swga_di{
// 	swga_multiline* lines;		
// 	swUInt32	p;	//点的索引
// };


//swga_Coordinate* swga_peerEndPoint(swga_endPoint* a);			//折线的另外一头端点

swga_line* swga_createReverseLine(swga_line*);	//反向线段


float swga_length(swga_line*);			//计算折线长度

//从折线ml开始offstart的位置开始计算len跨域了多少线段
//如果坐标点在某一线段中，不产生新的折线哦
swga_line* swga_extraLines(swga_line* lines,float offstart,float len);  


float swga_distance(swga_Coordinate* a,swga_Coordinate* b);	//计算两坐标点距离

float swga_radian(swga_Coordinate* a,swga_Coordinate* b);		//计算弧度
float swga_angle(swga_Coordinate* a, swga_Coordinate* b);		//计算从a到b的角度
//计算 a-b线段上离开a距离dist的坐标点,返回坐标点和坐标点所在线段的首位端点的角度
swga_Coordinate	swga_interceptPoint(swga_Coordinate	* a,swga_Coordinate* b,float dist); 

//搜寻在线段line偏移dist位置的点，并返回这个点所在线段的首尾点坐标
swga_Coordinate	swga_interceptPoint(swga_line* line,float dist,OUT swga_Coordinate* start,OUT swga_Coordinate* end); 



#endif
