import sys,os,os.path,struct

FILE_IDX='shanghai_100000_0.tidx'
FILE_IDX='D:/d-share/mapdata/shanghai_100000_0.tidx'

#FILE_IDX='./mapdata/shanghai_100000_0.tidx'
f = open(FILE_IDX,'rb')
resolution, = struct.unpack('f',f.read(4))
scalelevel, = struct.unpack('B',f.read(1))
f.read(16)
#f.read(4)
while 1:
	#x,y = struct.unpack('!II',f.read(8))
	offset, = struct.unpack('!I',f.read(4))
	fileid, = struct.unpack('!H',f.read(2))
	size, = struct.unpack('!I',f.read(4))
	if size:
		print offset,fileid,size,offset+size
print resolution,scalelevel

