#ifndef ISPACE_H
#define ISPACE_H

#define FCOMMON_NAME "luaPLC LUA Script Interpreter"
#define FCOMMON_VERSION "0.2"

extern lua_State *globalL;
extern void      *m_luaMutex;

int lua_ispace_connect(lua_State *L);
int lua_ispace_set_data(lua_State *L);
int lua_ispace_request_data(lua_State *L);
int lua_ispace_request_all_data(lua_State *L);
int lua_ispace_disconnect(lua_State *L);

int lua_ispace_set_value(lua_State *L);
int lua_ispace_get_value(lua_State *L);

#endif
