#include "geom_lines.h"
#include <math.h>
#include <algorithm>
//反向线段
swga_line* swga_createReverseLine(swga_line* line ){
	swga_line* rline = NULL;
	rline  = new swga_line;
	*rline = *line;
	std::reverse(rline->begin(),rline->end());
	return rline;
}

//计算折线长度
float swga_length(swga_line* line){
	float len;
	len=0.0;
	if(line->size()>1){
		for(swUInt32 n=1;n<line->size();n++){
			len += swga_distance(&line->at(n-1),&line->at(n));
		}
	}	
	return len;
}

//从折线ml开始offstart的位置开始计算len跨域了多少线段
//如果坐标点在某一线段中，不产生新的折线哦
swga_line* swga_extraLines(swga_line* line,float offstart,float length){
	swga_line* spanline = NULL;
	swga_Coordinate rc;
	float len=0.0;
	float left;
	swUInt32 n;
	swUInt32 startnode,endnode;

	n=0;
	while(1){
		n++;
		if(n>=line->size()){
			return NULL;
		}
		float r = swga_distance(&line->at(n-1),&line->at(n));
		if( r+len == offstart){
			startnode = n;
			break;		
		}else if( r+ len > offstart){
			startnode = n-1;
			break;
		}
		len+=r;		
	}
	n=0;
	while(1){
		n++;
		if(n>=line->size()){
			return NULL;
		}
		float r = swga_distance(&line->at(n-1),&line->at(n));
		if( r+len == offstart+length){
			endnode = n;
			break;		
		}else if( r+ len > offstart+length){
			endnode = n;
			break;
		}
		len+=r;		
	}	
	
	spanline = new swga_line;
	for(n=startnode;n<=endnode;n++){
		spanline->push_back(line->at(n));
	}	
	return spanline;
}  

//计算两坐标点距离
float swga_distance(swga_Coordinate* a,swga_Coordinate* b){
	float d;
	d = (float)sqrt( pow(b->x - a->x,2)+pow(b->y - a->y,2) );	
	return d;
}

//计算弧度
float swga_radian(swga_Coordinate* a,swga_Coordinate* b){
	float r;
	r = (float)atan( (b->y - a->y)/(b->x - a->x));	
	return r;
}

//计算从a到b的角度
float swga_angle(swga_Coordinate* a, swga_Coordinate* b){
	float angle =0.0;
	if( *a == *b){
		return angle;
	}
	if( a->x == b->x){
		if(b->y >= a->y){
			angle = 90.0 ;
		}else{
			angle = 270.;
		}
	}
	if(a->y == b->y){
		if( a->x <= b->x){
			angle =0.;
		}else{
			angle = 180.;
		}		
	}
	float radian;
	radian = swga_radian(a,b);
	angle = radian/M_PI*180;
	if( angle <0){
		if( b->x < a->x){
			angle = 180 - fabs(angle);
		}else{
			angle = 360 - fabs(angle);
		}
	}else{
		if( b->x < a->x){
			angle = 180 + fabs(angle);
		}
	}
	
	return angle;
}

//计算 a-b线段上离开a距离dist的坐标点,返回坐标点和坐标点所在线段的首位端点的角度
swga_Coordinate	swga_interceptPoint(swga_Coordinate	* a,swga_Coordinate* b,float dist){
	swga_Coordinate rc;
	rc = *a;
	float angle;
	angle = swga_angle(a,b);
	if(angle == 0){
		rc.x += dist;
	}else if(angle == 180){
		rc.x -= dist;
	}else if(angle == 90){
		rc.y += dist;
	}else if(angle == 270){
		rc.y -= dist;
	}else{
		float a,b;
		b =(float) (dist * cos(M_PI/180.0*angle)) ; // 底边
		a = (float)(dist* sin(M_PI/180.0*angle)); //对边
		rc.x += b;
		rc.y += a;		
	}
	return rc;
}

//搜寻在线段line偏移dist位置的点，并返回这个点所在线段的首尾点坐标
swga_Coordinate	swga_interceptPoint(swga_line* line,float dist,OUT swga_Coordinate* start,OUT swga_Coordinate* end){
	swga_Coordinate rc;
	float len=0.0;
	float left;
	swUInt32 n;
	
	n=0;
	for(;;){
		n++;
		if(n>=line->size()){
			break;
			
			//rc.x = line->at(line->size()-1).x;
			//rc.y = line->at(line->size()-1).y;
			//return rc; //超过了线段长度	返回最后一个点
		}
		float r = swga_distance(&line->at(n-1),&line->at(n));
		if( r+ len >= dist){
			start->x = line->at(n-1).x;
			start->y = line->at(n-1).y;
			end->x = line->at(n).x;
			end->y = line->at(n).y;
			rc = swga_interceptPoint(&line->at(n-1),&line->at(n),dist-len);
			break;
		}else{
			len+=r;
		}		
	}
	/*
	n=1;
	left = dist;
	len+=swga_distance(&line->at(n-1),&line->at(n));
	while(len<dist){
		n++;
		if(n>=line->size()){
			rc.x = line->at(line->size()-1).x;
			rc.y = line->at(line->size()-1).y;
			return rc; //超过了线段长度	返回最后一个点
		}
		len+=swga_distance(&line->at(n-1),&line->at(n));
	}
	start->x = line->at(n-1).x;
	start->y = line->at(n-1).y;
	end->x = line->at(n).x;
	end->y = line->at(n).y;
	*/
	return rc;
}

