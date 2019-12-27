
export LD_LIBRARY_PATH=/opt/orcadt/dc/mysql/lib

g++ -std=c++14 -g -L/opt/orcadt/dc/lib   -L /usr/local/lib -I /usr/local/include -I/opt/orcadt/dc/include -I/opt/orcadt/dc/include/mysql++ -I/opt/orcadt/dc/mysql/include -I ../include  -I ../include/dc -I ../h -I ../core/h dclib.cc ajson.cc -o ajson  -lzmq -lpthread -lmysqlpp -L/opt/orcadt/dc/mysql/lib -lfbmysqlclient -lxlnt -L/usr/local/lib -lwebsockets
