#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lua.h"
#include "lauxlib.h"
#include <wiringPi.h>
#include <wiringShift.h>
#include <softPwm.h>
#include <lcd.h>


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
	int n = lua_gettop(L);
  if(n != 12 && n != 8){
	  luaL_error(L,"Incorrect number of arguments.");
  }
  int rows, cols,  handle, rs,  strb, d0,  d1,  d2,  d3,  d4, d5,  d6,  d7;
  
  rows = (int) lua_tointeger(L,1);
  cols = (int) lua_tointeger(L,2);
  rs = (int) lua_tointeger(L,3);
  strb = (int) lua_tointeger(L,4);
  d0 = (int) lua_tointeger(L,5);
  d1 = (int) lua_tointeger(L,6);
  d2 = (int) lua_tointeger(L,7);
  d3 = (int) lua_tointeger(L,8);
  if(n==12){
	d4 = (int) lua_tointeger(L,9);
	d5 = (int) lua_tointeger(L,10);
	d6 = (int) lua_tointeger(L,11);
	d7 = (int) lua_tointeger(L,12);
	handle = lcdInit ( rows,  cols,  8, rs,  strb, d0,  d1,  d2,  d3,  d4, d5,  d6,  d7) ;
  }else{
	handle = lcdInit ( rows,  cols,  4, rs,  strb, d0,  d1,  d2,  d3,  0, 0,  0,  0) ;	  
  }
  
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

static int wpi_poff (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	pullUpDnControl (pin,PUD_OFF);
	return 0;
}

static int wpi_pdown (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	pullUpDnControl (pin,PUD_DOWN);
	return 0;
}

static int wpi_pup (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	pullUpDnControl (pin,PUD_UP);
	return 0;
}

static int wpi_pwm (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	if(pin == 1){
		pinMode(1,PWM_OUTPUT);
	}else{
		softPwmCreate(pin,10,100);
	}
	return 0;
}

static int wpi_fpwm (lua_State *L) {
	if(lua_gettop(L) != 2) luaL_error(L,"Invalid number of arguments.");
	int pin = (int) lua_tointeger(L,1);
	int value = (int) lua_tointeger(L,2);
	if(pin == 1){
		pwmWrite(1,value);
	}else{
		softPwmWrite(pin,value);
	}
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

static int wpi_millis (lua_State *L) {
	if(lua_gettop(L)) luaL_error(L,"Invalid number of arguments.");
	lua_pushnumber(L,(double) millis());
	return 1;
}

static int wpi_micros (lua_State *L) {
	if(lua_gettop(L)) luaL_error(L,"Invalid number of arguments.");
	lua_pushnumber(L,(double) micros());
	return 1;
}

static int wpi_delay (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	unsigned int value = (unsigned int) lua_tonumber(L,1);
	delay(value);
	return 0;
}

static int wpi_microdelay (lua_State *L) {
	if(lua_gettop(L) != 1) luaL_error(L,"Invalid number of arguments.");
	unsigned int value = (unsigned int) lua_tonumber(L,1);
	delayMicroseconds(value);
	return 0;
}

static int wpi_shiftin (lua_State *L) {
	if(lua_gettop(L) != 3) luaL_error(L,"Invalid number of arguments.");
	unsigned char pin = (unsigned char) lua_tointeger(L,1);
	unsigned char cpin = (unsigned char) lua_tointeger(L,2);
	unsigned char order = (unsigned char)( lua_toboolean(L,3)? LSBFIRST:MSBFIRST);
	lua_pushinteger(L,(lua_Integer) shiftIn(pin,cpin,order));
	return 1;
}

static int wpi_shiftout (lua_State *L) {
	if(lua_gettop(L) != 4) luaL_error(L,"Invalid number of arguments.");
	unsigned char pin = (unsigned char) lua_tointeger(L,1);
	unsigned char cpin = (unsigned char) lua_tointeger(L,2);
	unsigned char order = (unsigned char)( lua_toboolean(L,3)? LSBFIRST:MSBFIRST);
	unsigned char value = (unsigned char) lua_tointeger(L,4);
	shiftOut(pin,cpin,order,value);
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
{"pwm",   wpi_pwm},
{"fpwm",   wpi_fpwm},
{"poff",   wpi_poff},
{"pdown",   wpi_pdown},
{"pup",   wpi_pup},
{"read",   wpi_read},
{"write",   wpi_write},
{"millis",   wpi_millis},
{"micros",   wpi_micros},
{"delay",   wpi_delay},
{"microdelay",   wpi_microdelay},
{"shiftin",   wpi_shiftin},
{"shiftout",   wpi_shiftout},
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
