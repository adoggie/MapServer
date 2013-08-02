

//mapserver.ice
//------------------

dictionary<string,string>	HashValueSet;
dictionary<string,string>	ReturnValueT;
sequence<byte> 				StreamDataT;
sequence<string>			StringListT;
sequence<HashValueSet>	HashValueListT;
sequence<int>					IntListT;
sequence<StringListT>	StringListListT;
sequence<byte> PixmapDataT;


struct MapIdentT{
	int 	id;	
	string name;
};


struct GlobalGridItemId{
	int x;
	int y;
	int scalelevel;
};

struct MapBitmapTileT{
	GlobalGridItemId id;
	PixmapDataT	pixmap;	
};

struct MapBitmapTile2T{
	PixmapDataT	pixmap;
};

sequence<MapBitmapTileT>  MapBitmapTileListT;

struct MapTileSizeT{
	int width;
	int height;
};

struct GeoSizeT{
	float width;
	float height;
};

struct GeoRectT{
	float x;
	float y;
	float width;
	float height;
};

struct GeoPointT{
	float x;
	float y;
};

interface IMapServer{ 	
	MapBitmapTile2T	getBitmapTile3(MapIdentT mapid,float resolution,GeoPointT cxy,MapTileSizeT size,bool realRender);
};



