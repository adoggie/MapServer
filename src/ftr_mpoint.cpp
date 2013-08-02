#include "stdafx.h"
#include "ftr_mpoint.h"
#include "view.h"


swUInt16		swGeometryMultiPoint::getSize(){
	return _size;
}

/*
swGeoPoint*	swGeometryMultiPoint::getPointN(swUInt16 n){
	if(n>=_size){
		return NULL;
	}
	static swGeoPoint point; //��������̬������Ŀ���������ڻ�����ݵ�ַû��4�ֽڶ��룬��wince�»�����쳣
	swGeoPoint* r;
	if( ( (swUInt32)_points) & 0x03){ //û��4�ֽڶ���
		r = 	&point;
		memcpy(&point,_points+n,sizeof(swGeoPoint));
		
	}else{
		r =	 _points + n;
	}
	return r;
}
*/

bool	swGeometryMultiPoint::getPointN(swUInt16 n,swGeoPoint* gpt){
	if(n>=_size){
		return false;
	}
	swGeoPoint* r;
	//if( ( (ARCH_LONG)_points) & (sizeof(ARCH_LONG)-1) ){ //û��4�ֽڶ���
		memcpy(gpt, ((swByte*)_points)+ n*swGeoPoint::size(),swGeoPoint::size());
	//}else{
	//	*gpt = _points[n];
	//}
	return true;
}

swGeometryMultiPoint::swGeometryMultiPoint( swUInt16 size){
	_size = size;
}

//����֪�����ٸ�����������,��ȡ�������
bool swGeometryMultiPoint::serialIn( swByteStream& stream,swMapView* view){
// 	swUInt16 n;
// 	for(n=0;n<_size;n++){
// 		swAssertReturn(stream.readValue(_points[n])); //���ܳ������Խ��Υ�������������Ϸ��ز�ɾ��ǰ����Ķ����ڴ��
// 	}
	//memcpy( _points,stream.current(),sizeof(swGeoPoint)*_size);
	_points =(swGeoPoint*) stream.current();  //ָ�����ַ
	stream.forward(swGeoPoint::size()*_size);
	//calcMBR();
	return true;
}



void swGeometryMultiPoint::destroy(){
	// do nothing
}							

// void	swGeometryMultiPoint::getMBR(swGeoRect& rc){
// 	rc = _mbr;
// }
//����ֱ�ӽ��� _points[n]��ʽ��������ʣ��ᵼ���ֽڶ�������
//��Ϊ_pointsֱ��ָ�����ݵĻ����ַ����Щ����ǲ����ж����
/*
void	swGeometryMultiPoint::calcMBR(){	
	double dLeft	= 1.7E308;
	double dTop		= 1.7E308;
	double dRight	= -1.7E308;
	double dBottom	= -1.7E308;
	swGeoPoint gpt;
	for(swUInt32 n = 0; n < _size; n++){
		getPointN(n,&gpt);		
		if(gpt.x < dLeft){
			dLeft =gpt.x ;
		}
		if(gpt.y < dTop){
			dTop = gpt.y;
		}
		if(gpt.x > dRight){
			dRight = gpt.x;
		}
		if(gpt.y > dBottom){
			dBottom =gpt.y ;
		}
	}

	_mbr.x		= (float)dLeft;
	_mbr.y		= (float)dTop;
	_mbr.width= (float)(dRight-dLeft);
	_mbr.height= (float)(dBottom-dTop);
	 
}
*/
