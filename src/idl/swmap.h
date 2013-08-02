#ifndef _swmap_H
#define _swmap_H

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <tce/tce.h>
//#include "/root/workspace/tce/code/cpp/tce/tce.h"
//#include "/Users/socketref/Documents/workspace/test2/src/tce2.h"


namespace swmap {
	
	struct HashValueSethlp {
		//# -- THIS IS DICTIONARY! --
		typedef std::map< std::string,std::string > hash_type;
		typedef std::pair< std::string,std::string > hash_pair;
		hash_type& ds;
		
		HashValueSethlp(hash_type & ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			size_t _size = 0;
			_size = this->ds.size();
			d.writeUnsignedInt( this->ds.size() );
			hash_type::iterator _itr;
			for(_itr=this->ds.begin();_itr!=this->ds.end();_itr++){
				{				
					std::string k = _itr->first;
					d.writeString(k);
				}				
				{				
					std::string v = _itr->second;
					d.writeString(v);
				}				
			}			
		}		
		
		// unmarshall()
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					std::string _k = "";
					_k = d.readString();
					std::string _v = "";
					_v = d.readString();
					this->ds[_k] = _v;
				}				
			
			r = true;
			}catch(...){
				return false;
			}			
			return true;
		}		
	};	
	//-- end Dictonary Class definations --
	
	
	struct ReturnValueThlp {
		//# -- THIS IS DICTIONARY! --
		typedef std::map< std::string,std::string > hash_type;
		typedef std::pair< std::string,std::string > hash_pair;
		hash_type& ds;
		
		ReturnValueThlp(hash_type & ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			size_t _size = 0;
			_size = this->ds.size();
			d.writeUnsignedInt( this->ds.size() );
			hash_type::iterator _itr;
			for(_itr=this->ds.begin();_itr!=this->ds.end();_itr++){
				{				
					std::string k = _itr->first;
					d.writeString(k);
				}				
				{				
					std::string v = _itr->second;
					d.writeString(v);
				}				
			}			
		}		
		
		// unmarshall()
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					std::string _k = "";
					_k = d.readString();
					std::string _v = "";
					_v = d.readString();
					this->ds[_k] = _v;
				}				
			
			r = true;
			}catch(...){
				return false;
			}			
			return true;
		}		
	};	
	//-- end Dictonary Class definations --
	
	
	struct StreamDataThlp{
		
		typedef std::vector< unsigned char >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		StreamDataThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				d.writeByte(this->ds[n]);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						unsigned char _o = 0;
						_o = d.readByte();
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct StringListThlp{
		
		typedef std::vector< std::string >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		StringListThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				d.writeString(this->ds[n]);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						std::string _o = "";
						_o = d.readString();
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct HashValueListThlp{
		
		typedef std::vector< std::map< std::string,std::string > >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		HashValueListThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				{				
					HashValueSethlp _c(this->ds[n]);
					_c.marshall(d);
				}				
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						HashValueSethlp::hash_type _o;
						HashValueSethlp container(_o);
						r = container.unmarshall(d);
						if(!r) return false;
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct IntListThlp{
		
		typedef std::vector< int >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		IntListThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				d.writeInt(this->ds[n]);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						int _o = 0;
						_o = d.readInt();
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct StringListListThlp{
		
		typedef std::vector< std::vector< std::string > >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		StringListListThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				{				
					StringListThlp _c(this->ds[n]);
					_c.marshall(d);
				}				
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						StringListThlp::sequence_type _o;
						StringListThlp container(_o);
						r = container.unmarshall(d);
						if(!r) return false;
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct PixmapDataThlp{
		
		typedef std::vector< unsigned char >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		PixmapDataThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				d.writeByte(this->ds[n]);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						unsigned char _o = 0;
						_o = d.readByte();
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct MapIdentT{
	// -- STRUCT -- 
		int 	id;
		std::string 	name;
		//构造函数
		MapIdentT(){
			this->id = 0;
			this->name = "";
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeInt(this->id);
			d.writeString(this->name);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->id = d.readInt();
				this->name = d.readString();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct GlobalGridItemId{
	// -- STRUCT -- 
		int 	x;
		int 	y;
		int 	scalelevel;
		//构造函数
		GlobalGridItemId(){
			this->x = 0;
			this->y = 0;
			this->scalelevel = 0;
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeInt(this->x);
			d.writeInt(this->y);
			d.writeInt(this->scalelevel);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->x = d.readInt();
				this->y = d.readInt();
				this->scalelevel = d.readInt();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct MapBitmapTileT{
	// -- STRUCT -- 
		GlobalGridItemId 	id;
		std::vector< unsigned char > 	pixmap;
		//构造函数
		MapBitmapTileT(){
			this->id = GlobalGridItemId();
			this->pixmap = std::vector< unsigned char >();
		}		
		
		void marshall(tce::ByteArray& d){
			{			
				this->id.marshall(d);
			}			
			{			
				PixmapDataThlp _c(this->pixmap);
				_c.marshall(d);
			}			
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				r = this->id.unmarshall(d);
				if(!r){return false;}
				{				
					PixmapDataThlp _c(this->pixmap);
					r = _c.unmarshall(d);
					if(!r){return false;}
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct MapBitmapTile2T{
	// -- STRUCT -- 
		std::vector< unsigned char > 	pixmap;
		//构造函数
		MapBitmapTile2T(){
			this->pixmap = std::vector< unsigned char >();
		}		
		
		void marshall(tce::ByteArray& d){
			{			
				PixmapDataThlp _c(this->pixmap);
				_c.marshall(d);
			}			
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				{				
					PixmapDataThlp _c(this->pixmap);
					r = _c.unmarshall(d);
					if(!r){return false;}
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct MapBitmapTileListThlp{
		
		typedef std::vector< MapBitmapTileT >  sequence_type;
		//# -- SEQUENCE --
		sequence_type &ds;
		
		MapBitmapTileListThlp(sequence_type& ds_):ds(ds_){
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeUnsignedInt((unsigned int)this->ds.size());
			for(size_t n=0; n< this->ds.size();n++){
				this->ds[n].marshall(d);
			}			
		}		
		
		bool unmarshall(tce::ByteArray& d){
			bool r = false;
			try{
				size_t _size = 0;
				_size = d.readUnsignedInt();
				for(size_t _p=0;_p < _size;_p++){
					{					
						MapBitmapTileT _o;
						r= _o.unmarshall(d);
						if(!r) return false;
						this->ds.push_back(_o);
					}					
				}				
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}			
			return true;
		}		
		
	};	
	
	
	struct MapTileSizeT{
	// -- STRUCT -- 
		int 	width;
		int 	height;
		//构造函数
		MapTileSizeT(){
			this->width = 0;
			this->height = 0;
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeInt(this->width);
			d.writeInt(this->height);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->width = d.readInt();
				this->height = d.readInt();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct GeoSizeT{
	// -- STRUCT -- 
		float 	width;
		float 	height;
		//构造函数
		GeoSizeT(){
			this->width = 0.0;
			this->height = 0.0;
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeFloat(this->width);
			d.writeFloat(this->height);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->width = d.readFloat();
				this->height = d.readFloat();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct GeoRectT{
	// -- STRUCT -- 
		float 	x;
		float 	y;
		float 	width;
		float 	height;
		//构造函数
		GeoRectT(){
			this->x = 0.0;
			this->y = 0.0;
			this->width = 0.0;
			this->height = 0.0;
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeFloat(this->x);
			d.writeFloat(this->y);
			d.writeFloat(this->width);
			d.writeFloat(this->height);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->x = d.readFloat();
				this->y = d.readFloat();
				this->width = d.readFloat();
				this->height = d.readFloat();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	
	struct GeoPointT{
	// -- STRUCT -- 
		float 	x;
		float 	y;
		//构造函数
		GeoPointT(){
			this->x = 0.0;
			this->y = 0.0;
		}		
		
		void marshall(tce::ByteArray& d){
			d.writeFloat(this->x);
			d.writeFloat(this->y);
		}		
		
		//反序列化 unmarshall()
		bool unmarshall(tce::ByteArray& d){
			try{
				bool r = false;
				this->x = d.readFloat();
				this->y = d.readFloat();
				r = true;
			}catch(const tce::RpcException& e){
				return false;
			}
			return true;
		}		
		 // --  end function -- 
		
	};	
	class IMapServer_delegate;
	
	class IMapServer:public tce::RpcServant{
		//# -- INTERFACE -- 
		public:
		
		boost::shared_ptr<IMapServer_delegate> delegate;
		IMapServer();
		
		
		virtual MapBitmapTile2T getBitmapTile(const MapIdentT& mapid_,const float& resolution_,const GeoPointT& cxy_,const MapTileSizeT& size_,const bool& realRender_,tce::RpcContext& ctx){
			return MapBitmapTile2T();
		}		
	};	
	
	class IMapServer_delegate:public tce::RpcServantDelegate {
		
		public:
		IMapServer * inst;
		
		IMapServer_delegate(IMapServer* inst){
			this->_index = 0;
			this->_optlist[0] = &IMapServer_delegate::getBitmapTile_dummy;
			this->inst = inst;
		}		
		
		static bool getBitmapTile_dummy(tce::RpcContext& ctx){
			IMapServer_delegate *pf = (IMapServer_delegate*) ctx.delegate;
			return pf->getBitmapTile(ctx);
		}		
		
		bool getBitmapTile(tce::RpcContext& ctx){
			tce::ByteArray& d = ctx.msg->paramstream;
			d.size();
			bool r = false;
			r = false;
			MapIdentT mapid_;
			r = mapid_.unmarshall(d);
			if(!r) return false;
			float resolution_;
			resolution_ = d.readFloat();
			GeoPointT cxy_;
			r = cxy_.unmarshall(d);
			if(!r) return false;
			MapTileSizeT size_;
			r = size_.unmarshall(d);
			if(!r) return false;
			bool realRender_;
			{			
				unsigned char _d = 0;
				_d = d.readByte();
				if(_d == 0){
					realRender_ = false;
				}else{
					realRender_ = true;
				}				
			}			
			MapBitmapTile2T cr;
			cr = this->inst->getBitmapTile(mapid_,resolution_,cxy_,size_,realRender_,ctx);
			if( ctx.msg->calltype & tce::RpcMessage::ONEWAY){
				return false;
			}			
			
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			boost::shared_ptr<tce::RpcMessageReturn> m(new tce::RpcMessageReturn);
			m->sequence = ctx.msg->sequence;
			m->ifidx = ctx.msg->ifidx;
			m->call_id = ctx.msg->call_id;
			m->conn = ctx.conn;
			m->callmsg = ctx.msg;
			cr.marshall( *_d );
			m->paramstream.writeBytes(*_d);
			ctx.conn->sendMessage(m);
			return true;
		}		
		
	};	
	class IMapServerPrx;
	class IMapServer_AsyncCallBack: public tce::RpcAsyncCallBackBase{
	public:
		virtual void getBitmapTile(const MapBitmapTile2T & result,IMapServerPrx* prx){
		}		
		
	};	
	
	
	typedef boost::shared_ptr<IMapServerPrx> IMapServerPrxPtr;
	class IMapServerPrx: public tce::RpcProxyBase{
		//# -- INTERFACE PROXY -- 
		public:
		
		IMapServerPrx(){}
		IMapServerPrx( boost::shared_ptr<tce::RpcConnection> & conn){
			this->conn = conn;
		}		
		~IMapServerPrx(){ if(this->conn.get()){ this->conn->close();}}
		public:
		
		
		public:
		
		MapBitmapTile2T getBitmapTile(MapIdentT mapid_,float resolution_,GeoPointT cxy_,MapTileSizeT size_,bool realRender_,unsigned int timeout = 0,const tce::RpcProperites_t& props=tce::RpcProperites_t()) throw(tce::RpcException){
			//# function index: 17
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::TWOWAY));
			m->ifidx = 0;
			m->opidx = 0;
			m->timeout = timeout;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			m->paramsize = 5;
			m->call_id = tce::RpcCommunicator::instance().localServiceId();
			m->extra.set(props);
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			tce::ByteArray& d = *_d;
			mapid_.marshall(*_d);
			(d).writeFloat(resolution_);
			cxy_.marshall(*_d);
			size_.marshall(*_d);
			if(realRender_ == true){
				(d).writeByte(1);
			}else{
				(d).writeByte(0);
			}			
			m->paramstream.writeBytes(*_d);
			m->prx =(RpcProxyBase*)this;
			m->conn = this->conn;
			if( !this->conn->sendMessage(m) ){
				throw  tce::RpcException(tce::RpcConsts::RPCERROR_SENDFAILED);
			}			
			boost::shared_ptr<tce::RpcMessage> mr = m->wait->waitObject(timeout);
			if(m->exception.get()){
				throw *m->exception;
			}			
			if(!mr.get()){
				throw tce::RpcException(tce::RpcConsts::RPCERROR_TIMEOUT);
			}			
			d = mr->paramstream;
			MapBitmapTile2T p;
			p.unmarshall(d);
			return p;
		}		
		public:
		
		void getBitmapTile(MapIdentT mapid_,float resolution_,GeoPointT cxy_,MapTileSizeT size_,bool realRender_,IMapServer_AsyncCallBack* async,const tce::RpcProperites_t& props=tce::RpcProperites_t()) throw(tce::RpcException){
			//# function index: 17
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall(tce::RpcMessage::ASYNC));
			m->ifidx = 0;
			m->opidx = 0;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			m->paramsize = 5;
			m->call_id = tce::RpcCommunicator::instance().localServiceId();
			m->extra.set(props);
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			mapid_.marshall(*_d);
			(*_d).writeFloat(resolution_);
			cxy_.marshall(*_d);
			size_.marshall(*_d);
			if(realRender_ == true){
				(*_d).writeByte(1);
			}else{
				(*_d).writeByte(0);
			}			
			m->paramstream.writeBytes(*_d);
			m->prx =(tce::RpcProxyBase*)this;
			m->async = async;
			m->callback = IMapServerPrx::getBitmapTile_asyncparser_hlp;
			m->conn = this->conn;
			if( !this->conn->sendMessage(m) ){
				throw  tce::RpcException(tce::RpcConsts::RPCERROR_SENDFAILED);
			}			
		}		
		public:
		
		void getBitmapTile_oneway(MapIdentT mapid_,float resolution_,GeoPointT cxy_,MapTileSizeT size_,bool realRender_,const tce::RpcProperites_t& props=tce::RpcProperites_t()) throw(tce::RpcException){
			//# function index: 17
			
			boost::shared_ptr<tce::RpcMessageCall> m(new tce::RpcMessageCall( tce::RpcMessage::ONEWAY) );
			m->ifidx = 0;
			m->opidx = 0;
			m->issuetime = tce::RpcCommunicator::instance().currentTimeStamp(); 
			m->paramsize = 5;
			m->call_id = tce::RpcCommunicator::instance().localServiceId();
			m->extra.set(props);
			boost::shared_ptr<tce::ByteArray> _d(new tce::ByteArray);
			mapid_.marshall(*_d);
			(*_d).writeFloat(resolution_);
			cxy_.marshall(*_d);
			size_.marshall(*_d);
			if(realRender_ == true){
				(*_d).writeByte(1);
			}else{
				(*_d).writeByte(0);
			}			
			m->paramstream.writeBytes(*_d);
			m->prx =(RpcProxyBase*)this;
			if( !this->conn->sendMessage(m) ){
				throw  tce::RpcException(tce::RpcConsts::RPCERROR_SENDFAILED);
			}			
		}		
		
		private:
		static void getBitmapTile_asyncparser_hlp(tce::RpcMessage* m,tce::RpcMessage* m2){
			IMapServerPrx* pthis = (IMapServerPrx*)m->prx;
			pthis->getBitmapTile_asyncparser(m,m2);
		}		
		
		
		private:
		void getBitmapTile_asyncparser(tce::RpcMessage* m_,tce::RpcMessage* m2_){
			//# function index: 17 , m2 - callreturn msg.
			
			tce::RpcMessage* m = m_;
			tce::RpcMessage* m2 = m2_;
			tce::ByteArray& stream = m2->paramstream;
			IMapServer_AsyncCallBack* user = (IMapServer_AsyncCallBack*)m->async;
			if(stream.size() == 0){ 
				return;
			}			
			try{
				tce::ByteArray d;
				d = stream;
				MapBitmapTile2T _p;
				_p.unmarshall(d);
				user->getBitmapTile(_p,this);
			}catch(const tce::RpcException& e){
				tce::RpcCommunicator::instance().logTrace(e.what());
			}			
		}		
		
		public:
		static IMapServerPrxPtr create(tce::RpcConnectionPtr& conn){
			return IMapServerPrxPtr( new IMapServerPrx(conn) );
		}		
		
		// async 返回的链接状态 
		static IMapServerPrxPtr create(const std::string& uri,tce::RpcConnection::Types conntype=tce::RpcConnection::SOCKET, bool sync=false,const tce::Properties_t& props = tce::Properties_t()){
			tce::RpcConnectionPtr conn ;
			conn = tce::RpcCommunicator::instance().createConnection(conntype,uri,props);
			bool r = false;
			r = conn->connect();
			if(!r) return IMapServerPrxPtr();
			return IMapServerPrxPtr( new IMapServerPrx(conn) );
		}		
	};	
	
	inline IMapServer::IMapServer(){
		this->_delegate = boost::shared_ptr<IMapServer_delegate>( new IMapServer_delegate(this) );
	}	
	
}
#endif
