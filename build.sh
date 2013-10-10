LUA_CFLAGS=`pkg-config lua5.1 --cflags`
LIBTOOL="libtool --tag=CC --silent"
$LIBTOOL --mode=compile cc $LUA_CFLAGS -lwiringPi -lwiringPiDev -lpthread -c main.c
$LIBTOOL --mode=link cc $LUA_CFLAGS -lwiringPi -lwiringPiDev -lpthread -rpath /usr/local/lib/lua/5.1 -o libwpi.la main.lo
mv .libs/libwpi.so.0.0.0 wpi.so
