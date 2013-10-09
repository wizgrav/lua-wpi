#include "lua.h"
#include "lauxlib.h"
#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ARGCHECK(n)\
	if(lua_gettop(L) != n){\
		 luaL_error(L,"Invalid number of arguments.");\
	}\
	lcd_handle *l = luaL_checkudata(L,1,"wpi.lcd");\
	if(!l) luaL_error(L,"First argument must be an lcd handle.")

typedef struct lcd_handle{
	int handle;
}lcd_handle;

static int wpi_lcd (lua_State *L) {
  if(lua_gettop(L) != 13){
	  luaL_error(L,"Incorrect number of arguments.");
  }
  int rows, cols,  bits, rs,  strb, d0,  d1,  d2,  d3,  d4, d5,  d6,  d7;
  
  rows = (int) lua_tointeger(L,1);
  cols = (int) lua_tointeger(L,2);
  bits = (int) lua_tointeger(L,3);
  rs = (int) lua_tointeger(L,4);
  strb = (int) lua_tointeger(L,5);
  d0 = (int) lua_tointeger(L,6);
  d1 = (int) lua_tointeger(L,7);
  d2 = (int) lua_tointeger(L,8);
  d3 = (int) lua_tointeger(L,9);
  d4 = (int) lua_tointeger(L,10);
  d5 = (int) lua_tointeger(L,11);
  d6 = (int) lua_tointeger(L,12);
  d7 = (int) lua_tointeger(L,13);
  
  int handle = lcdInit ( rows,  cols,  bits, rs,  strb, d0,  d1,  d2,  d3,  d4, d5,  d6,  d7) ;
  
  if(handle < 0){
	luaL_error(L,"Error initializing lcd.");
  }
  
  lua_settop(L,0);
  lcd_handle *l = lua_newuserdata(L,sizeof(lcd_handle));
  luaL_getmetatable(L,"wiringPi.lcd");
  lua_setmetatable(L,-2);
  l->handle = handle;
  return 1;
}
static int wpi_mode (lua_State *L) {
	if(lua_gettop(L) != 2) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	int mode = (int) lua_toboolean(L,2);
	pinMode (pin,mode);
	return 0;
}

static int wpi_read (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	lua_settop(L,0);
	lua_pushboolean(L,digitalRead(pin));
	return 1;
}

static int wpi_write (lua_State *L) {
	if(lua_gettop(L) != 2) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	int value = (int) lua_toboolean(L,2);
	digitalWrite(pin,value);
	return 0;
}

static int lcd_home (lua_State *L) {
	ARGCHECK(1);
	lcdHome(l->handle);
	return 0;
}

static int lcd_clear (lua_State *L) {
	ARGCHECK(1);
	lcdClear(l->handle);
	return 0;
}

static int lcd_cursor (lua_State *L) {
	ARGCHECK(2);
	lcdCursor(l->handle,lua_toboolean(L,2));
	return 0;
}

static int lcd_display (lua_State *L) {
	ARGCHECK(2);
	lcdDisplay(l->handle,lua_toboolean(L,2));
	return 0;
}

static int lcd_blink (lua_State *L) {
	ARGCHECK(2);
	lcdCursorBlink(l->handle,lua_toboolean(L,2));
	return 0;
}

static int lcd_position (lua_State *L) {
	ARGCHECK(3);
	lcdPosition(l->handle,(int)lua_tointeger(L,2),(int)lua_tointeger(L,3));
	return 0;
}


static int lcd_print (lua_State *L) {
	ARGCHECK(2);
	const char *s = (const char *) lua_tolstring(L,2,NULL);
	if(!s) luaL_error(L,"Invalid type, expected string.");
	lcdPuts(l->handle,s);
	return 0;
}

static int lcd_char (lua_State *L) {
	ARGCHECK(2);
	unsigned char c = (unsigned char) lua_tointeger(L,2);
	lcdPutchar(l->handle,c);
	return 0;
}

static int lcd_def (lua_State *L) {
	ARGCHECK(3);
	int i = lua_tointeger(L,2);
	size_t len;
	unsigned char *s = (unsigned char *)lua_tolstring(L,3,&len);
	if(!s) luaL_error(L,"Invalid type, expected string.");
	if(len != 8) luaL_error(L,"Definition string has to be exactly 8 characters.");
	lcdCharDef(l->handle,i,s);
	return 0;
}



static const luaL_reg wpilib[] = {
{"lcd",   wpi_lcd},
{"mode",   wpi_mode},
{"read",   wpi_read},
{"write",   wpi_write},

{NULL, NULL}
};

static const luaL_reg lcdlib[] = {
{"home",   lcd_home},
{"clear",   lcd_clear},
{"display",   lcd_display},
{"def",   lcd_def},
{"cursor",   lcd_cursor},
{"blink",   lcd_blink},
{"position",   lcd_position},
{"print",   lcd_print},
{"char",   lcd_char},
{NULL, NULL}
};

LUALIB_API int luaopen_wpi (lua_State *L) {
  if (wiringPiSetup () == -1) luaL_error(L,"piBoardRev: Unable to determine board revision from /proc/cpuinfo");
  luaL_newmetatable(L, "wpi.lcd");
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);  
  lua_settable(L, -3);
  luaL_openlib(L, NULL, lcdlib,0);
  luaL_register(L, "wpi", wpilib);
  return 1;
}
