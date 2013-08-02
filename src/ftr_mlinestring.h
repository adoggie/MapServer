
#ifndef _SW_FEATURE_MULTILINESTRING_H
#define _SW_FEATURE_MULTILINESTRING_H

#include "ftr_collection.h"
#include "ftr_linestring.h"

class swGeometryMultiLineString:public swGeometryCollection{
public:
	swGeometryMultiLineString( swByteStream& );
	void operator << ( swByteStream& );
private:
	std::vector<swGeometryLineString*> _lines;
};

#endif
