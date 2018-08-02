/*
** $Id: lua.c,v 1.160.1.2 2007/12/28 15:32:23 roberto Exp $
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ENV_WINDOWS
 #include <windows.h>
#endif

#ifdef ENV_LINUX
 #include <arpa/inet.h>
#endif

#define lua_c

#ifdef __cplusplus
extern "C" 
{
#endif
   #include <lua.h>

   #include <lauxlib.h>
   #include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "liboapc.h"
#ifndef OAPC_EXT_EXPORTS
 #include "ispace.h"
 #include "globaldefs.h"
#else //OAPC_EXT_EXPORTS
 #include "../libio_luaio/libio_luaio.h"
#endif //OAPC_EXT_EXPORTS


lua_State *globalL = NULL;

#ifndef LUA_PROGNAME
 #define LUA_PROGNAME "lua"
#endif

#ifndef LUA_PROMPT
 #define LUA_PROMPT  "> "
 #define LUA_PROMPT2 ">>"
#endif

#ifndef LUA_MAXINPUT
 #define LUA_MAXINPUT 512
#endif

#ifndef LUA_INIT
 #define LUA_INIT   "LUA_INIT"
#endif

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 501 
 # define lua_rawlen lua_strlen 
#elif LUA_VERSION_NUM == 501 
 # define lua_rawlen lua_objlen 
#endif

/*
** lua_readline defines how to show a prompt and then read a line from
** the standard input.
** lua_saveline defines how to "save" a read line in a "history".
** lua_freeline defines how to free a line read by lua_readline.
*/
#if defined(LUA_USE_READLINE)

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#define lua_readline(L,b,p)	((void)L, ((b)=readline(p)) != NULL)

#ifndef lua_saveline
 #define lua_saveline(L,idx) \
         if (lua_rawlen(L,idx) > 0)  /* non-empty line? */ \
           add_history(lua_tostring(L, idx));  /* add it to history */
#endif

#define lua_freeline(L,b)	((void)L, free(b))

#elif !defined(lua_readline)

#define lua_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */
#define lua_saveline(L,idx)	{ (void)L; (void)idx; }
#define lua_freeline(L,b)	{ (void)L; (void)b; }

#endif

/*
** lua_stdin_is_tty detects whether the standard input is a 'tty' (that
** is, whether we're running lua interactively).
*/
#if defined(LUA_USE_ISATTY)
#include <unistd.h>
#define lua_stdin_is_tty()	isatty(0)
#elif defined(LUA_WIN)
#include <io.h>
#include <stdio.h>
#define lua_stdin_is_tty()	_isatty(_fileno(stdin))
#else
#define lua_stdin_is_tty()	1  /* assume stdin is a tty */
#endif

static const char *progname = LUA_PROGNAME;



void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);
  luaL_error(L, "interrupted!");
}


#ifndef OAPC_EXT_EXPORTS
static void laction (int i) {
  signal(i, SIG_DFL); /* if another SIGINT happens before lstop,
                              terminate process (default action) */
  lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}


static void print_usage (void) {
  fprintf(stderr,
  "usage: %s [options] [script [args]].\n"
  "Available options are:\n"
  "  -e stat  execute string " LUA_QL("stat") "\n"
  "  -l name  require library " LUA_QL("name") "\n"
  "  -i       enter interactive mode after executing " LUA_QL("script") "\n"
  "  -v       show version information\n"
  "  --       stop handling options\n"
  "  -        execute stdin and stop handling options\n"
  ,
  progname);
  fflush(stderr);
}
#endif //OAPC_EXT_EXPORTS


static void l_message (const char *pname, const char *msg) {
  if (pname) fprintf(stderr, "%s: ", pname);
  fprintf(stderr, "%s\n", msg);
  fflush(stderr);
}


static int report (lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
#ifndef OAPC_EXT_EXPORTS
    l_message(progname, msg);
#else
    struct instData *data;

    lua_getglobal(L, "inDt");
    data=(struct instData*)lua_touserdata(L, -1);
    oapc_thread_mutex_lock(data->m_binOUTMutex[1]);
    if (data->m_binOUT[1]) oapc_util_release_bin_data(data->m_binOUT[1]);
    data->m_binOUT[1]=oapc_util_alloc_bin_data(OAPC_BIN_TYPE_TEXT,OAPC_BIN_SUBTYPE_TEXT_PLAIN,OAPC_COMPRESS_NONE,(int)strlen(msg)+4);
    if (data->m_binOUT[1])
    {
       data->m_binOUT[1]->param1=htonl((int)strlen(msg));
       data->m_binOUT[1]->param2=htonl(-1); // mark this as debug-message
       strcpy(&data->m_binOUT[1]->data,msg);
    }
    oapc_thread_mutex_unlock(data->m_binOUTMutex[1]);
    m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
#endif
    lua_pop(L, 1);
  }
  return status;
}


static int traceback (lua_State *L) {
  if (!lua_isstring(L, 1))  /* 'message' not a string? */
    return 1;  /* keep it intact */
  // 5.1lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  lua_getglobal(L,"debug"); // should work with all 5.x
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}


static int docall (lua_State *L, int narg, int clear) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
#ifndef OAPC_EXT_EXPORTS
  signal(SIGINT, laction);
#endif
  status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
  signal(SIGINT, SIG_DFL);
  lua_remove(L, base);  /* remove traceback function */
  /* force a complete garbage collection in case of errors */
  if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
  return status;
}


static int getargs (lua_State *L, char **argv, int n) {
  int narg;
  int i;
  int argc = 0;
  while (argv[argc]) argc++;  /* count total number of arguments */
  narg = argc - (n + 1);  /* number of arguments to the script */
  luaL_checkstack(L, narg + 3, "too many arguments to script");
  for (i=n+1; i < argc; i++)
    lua_pushstring(L, argv[i]);
  lua_createtable(L, narg, n + 1);
  for (i=0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i - n);
  }
  return narg;
}


#ifndef OAPC_EXT_EXPORTS
static void print_version (void)
{
   l_message(NULL, FCOMMON_NAME " " FCOMMON_VERSION " " FCOMMON_URL " based on:");
   l_message(NULL, LUA_RELEASE "  " LUA_COPYRIGHT);
}


static int dolibrary (lua_State *L, const char *name) {
    lua_getglobal(L, "require");
    lua_pushstring(L, name);
    return report(L, docall(L, 1, 1));
}
#endif //OAPC_EXT_EXPORTS


static int dofile (lua_State *L, const char *name) {
  int status = luaL_loadfile(L, name) || docall(L, 0, 1);
  return report(L, status);
}


static int dostring (lua_State *L, const char *s, const char *name) {
  int status = luaL_loadbuffer(L, s, strlen(s), name) || docall(L, 0, 1);
  return report(L, status);
}



static const char *get_prompt (lua_State *L, int firstline) {
  const char *p;
  //5.1lua_getfield(L, LUA_GLOBALSINDEX, firstline ? "_PROMPT" : "_PROMPT2");
  lua_getglobal(L,firstline ? "_PROMPT" : "_PROMPT2"); // should work with all 5.x
  p = lua_tostring(L, -1);
  if (p == NULL) p = (firstline ? LUA_PROMPT : LUA_PROMPT2);
  lua_pop(L, 1);  /* remove global */
  return p;
}


static int incomplete (lua_State *L, int status) {
  if (status == LUA_ERRSYNTAX) {
    size_t lmsg;
    const char *msg = lua_tolstring(L, -1, &lmsg);
    const char *tp = msg + lmsg - (sizeof(LUA_QL("<eof>")) - 1);
    if (strstr(msg, LUA_QL("<eof>")) == tp) {
      lua_pop(L, 1);
      return 1;
    }
  }
  return 0;  /* else... */
}


static int pushline (lua_State *L, int firstline) {
  char buffer[LUA_MAXINPUT];
  char *b = buffer;
  size_t l;
  const char *prmt = get_prompt(L, firstline);
  if (lua_readline(L, b, prmt) == 0)
    return 0;  /* no input */
  l = strlen(b);
  if (l > 0 && b[l-1] == '\n')  /* line ends with newline? */
    b[l-1] = '\0';  /* remove it */
  if (firstline && b[0] == '=')  /* first line starts with `=' ? */
    lua_pushfstring(L, "return %s", b+1);  /* change it to `return' */
  else
    lua_pushstring(L, b);
  lua_freeline(L, b);
  return 1;
}


static int loadline (lua_State *L) {
  int status;
  lua_settop(L, 0);
  if (!pushline(L, 1))
    return -1;  /* no input */
  for (;;) {  /* repeat until gets a complete line */
    status = luaL_loadbuffer(L, lua_tostring(L, 1), lua_strlen(L, 1), "=stdin");
    if (!incomplete(L, status)) break;  /* cannot try to add lines? */
    if (!pushline(L, 0))  /* no more input? */
      return -1;
    lua_pushliteral(L, "\n");  /* add a new line... */
    lua_insert(L, -2);  /* ...between the two lines */
    lua_concat(L, 3);  /* join them */
  }
  lua_saveline(L, 1);
  lua_remove(L, 1);  /* remove line */
  return status;
}


static void dotty (lua_State *L) {
  int status;
  const char *oldprogname = progname;
  progname = NULL;
  while ((status = loadline(L)) != -1) {
    if (status == 0) status = docall(L, 0, 0);
    report(L, status);
    if (status == 0 && lua_gettop(L) > 0) {  /* any result to print? */
      lua_getglobal(L, "print");
      lua_insert(L, 1);
      if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != 0)
        l_message(progname, lua_pushfstring(L,
                               "error calling " LUA_QL("print") " (%s)",
                               lua_tostring(L, -1)));
    }
  }
  lua_settop(L, 0);  /* clear stack */
  fputs("\n", stdout);
  fflush(stdout);
  progname = oldprogname;
}


static int handle_script (lua_State *L, char **argv, int n) {
  int status;
  const char *fname;
  int narg = getargs(L, argv, n);  /* collect arguments */
  lua_setglobal(L, "arg");
  fname = argv[n];
  if (strcmp(fname, "-") == 0 && strcmp(argv[n-1], "--") != 0) 
    fname = NULL;  /* stdin */
  status = luaL_loadfile(L, fname);
  lua_insert(L, -(narg+1));
  if (status == 0)
    status = docall(L, narg, 0);
  else
    lua_pop(L, narg);      
  return report(L, status);
}


/* check that argument has no extra characters at the end */
#define notail(x)	{if ((x)[2] != '\0') return -1;}


#ifndef OAPC_EXT_EXPORTS
static int collectargs (char **argv, int *pi, int *pv, int *pe) {
  int i;
  for (i = 1; argv[i] != NULL; i++) {
    if (argv[i][0] != '-')  /* not an option? */
        return i;
    switch (argv[i][1]) {  /* option */
      case '-':
        notail(argv[i]);
        return (argv[i+1] != NULL ? i+1 : 0);
      case '\0':
        return i;
      case 'i':
        notail(argv[i]);
        *pi = 1;  /* go through */
      case 'v':
        notail(argv[i]);
        *pv = 1;
        break;
      case 'e':
        *pe = 1;  /* go through */
      case 'l':
        if (argv[i][2] == '\0') {
          i++;
          if (argv[i] == NULL) return -1;
        }
        break;
      default: return -1;  /* invalid option */
    }
  }
  return 0;
}


static int runargs (lua_State *L, char **argv, int n) {
  int i;
  for (i = 1; i < n; i++) {
    if (argv[i] == NULL) continue;
    lua_assert(argv[i][0] == '-');
    switch (argv[i][1]) {  /* option */
      case 'e': {
        const char *chunk = argv[i] + 2;
        if (*chunk == '\0') chunk = argv[++i];
        lua_assert(chunk != NULL);
        if (dostring(L, chunk, "=(command line)") != 0)
          return 1;
        break;
      }
      case 'l': {
        const char *filename = argv[i] + 2;
        if (*filename == '\0') filename = argv[++i];
        lua_assert(filename != NULL);
        if (dolibrary(L, filename))
          return 1;  /* stop if file fails */
        break;
      }
      default: break;
    }
  }
  return 0;
}
#endif


static int handle_luainit (lua_State *L) {
  const char *init = getenv(LUA_INIT);
  if (init == NULL) return 0;  /* status OK */
  else if (init[0] == '@')
    return dofile(L, init+1);
  else
    return dostring(L, init, "=" LUA_INIT);
}


struct Smain 
{
   int              argc;
   char           **argv;
   int              status;
};


static int pmain (lua_State *L) 
{
#ifndef OAPC_EXT_EXPORTS
   int script;
   int has_v = 0, has_e = 0;
#endif
   struct Smain *s = (struct Smain *)lua_touserdata(L, 1);
   char **argv = s->argv;
   int has_i = 0;

   globalL = L;
   if (argv[0] && argv[0][0]) progname = argv[0];
   lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
   luaL_openlibs(L);  /* open libraries */
   lua_gc(L, LUA_GCRESTART, 0);
   s->status = handle_luainit(L);
   if (s->status != 0) return 0;
#ifndef OAPC_EXT_EXPORTS
   script = collectargs(argv, &has_i, &has_v, &has_e);
   if (script < 0) 
   {  /* invalid args? */
      print_usage();
      s->status = 1;
      return 0;
   }
   if (has_v) print_version();
   s->status = runargs(L, argv, (script > 0) ? script : s->argc);
   if (s->status != 0) return 0;
   if (script) s->status = handle_script(L, argv, script);
#else
   s->status = handle_script(L, argv,1);
#endif //OAPC_EXT_EXPORTS
   if (s->status != 0) return 0;
   if (has_i) dotty(L);
#ifndef OAPC_EXT_EXPORTS
   else if (script == 0 && !has_e && !has_v) 
   {
      if (lua_stdin_is_tty()) 
      {
         print_version();
         dotty(L);
      }
      else dofile(L, NULL);  /* executes stdin as a file */
   }
#endif
   return 0;
}


static int lua_thread_sleep(lua_State *L)
{
#ifdef OAPC_EXT_EXPORTS
   struct instData *data;
#endif
   int n;
   
   n = lua_gettop(L);    /* number of arguments */
   if (n!=1) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isnumber(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
#ifndef OAPC_EXT_EXPORTS
   n=(int)lua_tonumber(L,1);
   oapc_thread_mutex_unlock(m_luaMutex);
   oapc_thread_sleep(n);
   oapc_thread_mutex_lock(m_luaMutex);
#else //OAPC_EXT_EXPORTS
   lua_getglobal(L, "inDt");
   data=(struct instData*)lua_touserdata(L, -1);  //Get it from the top of the stack
   n=(int)lua_tonumber(L,1);
   oapc_thread_mutex_unlock(data->m_luaMutex);
   oapc_thread_sleep(n);
   oapc_thread_mutex_lock(data->m_luaMutex);
#endif //OAPC_EXT_EXPORTS
   return 0;                   /* number of results */
}


#ifndef OAPC_EXT_EXPORTS
int main (int argc, char **argv) 
#else //OAPC_EXT_EXPORTS
int luaMain(struct instData *data)
#endif //OAPC_EXT_EXPORTS
{
   int          status;
   struct Smain s;
   lua_State   *L;

#ifndef OAPC_EXT_EXPORTS
 #ifdef ENV_WINDOWS
   WSADATA      wsaData;

   WSAStartup((MAKEWORD(1, 1)),&wsaData);
 #endif //ENV_WINDOWS
   m_luaMutex=oapc_thread_mutex_create();
#else
   data->m_luaMutex=oapc_thread_mutex_create();
#endif //OAPC_EXT_EXPORTS
   L=luaL_newstate(); /* 5.2 create state */
//   L= lua_open();     /* 5.1 create state */
   if (L == NULL) 
   {
#ifndef OAPC_EXT_EXPORTS
      l_message(argv[0], "cannot create state: not enough memory");
      return EXIT_FAILURE;
#else //OAPC_EXT_EXPORTS
      return OAPC_ERROR_NO_MEMORY;
#endif //OAPC_EXT_EXPORTS
   }

   lua_register(L,"oapc_thread_sleep",           lua_thread_sleep);
#ifndef OAPC_EXT_EXPORTS
   lua_register(L,"oapc_ispace_connect",         lua_ispace_connect);
   lua_register(L,"oapc_ispace_disconnect",      lua_ispace_disconnect);
   lua_register(L,"oapc_ispace_set_data",        lua_ispace_set_data);
   lua_register(L,"oapc_ispace_request_data",    lua_ispace_request_data);
   lua_register(L,"oapc_ispace_request_all_data",lua_ispace_request_all_data);
  
   lua_register(L,"oapc_ispace_set_value",       lua_ispace_set_value);
   lua_register(L,"oapc_ispace_get_value",       lua_ispace_get_value);

   lua_register(L,"oapc_util_thread_sleep",      lua_thread_sleep);

   s.argc = argc;
   s.argv = argv;
   oapc_thread_mutex_lock(m_luaMutex);
#else //OAPC_EXT_EXPORTS
   lua_pushlightuserdata(L,data);
   lua_setglobal(L, "inDt");

   lua_register(L,"luaio_has_new_value",         lua_has_new_value);
   lua_register(L,"luaio_get_value",             lua_get_value);
   lua_register(L,"luaio_set_value",             lua_set_value);
   lua_register(L,"luaio_release_binary",        lua_release_binary);

   data->globalL=L;
   s.argc=2;
   s.argv=(char**)malloc(3*sizeof(char*));
   if (!s.argv) return OAPC_ERROR_NO_MEMORY;
   s.argv[0]=NULL;
   s.argv[1]=data->config.m_luafile;
   s.argv[2]=NULL;
   oapc_thread_mutex_lock(data->m_luaMutex);
#endif //OAPC_EXT_EXPORTS
   //status = lua_cpcall(L, &pmain, &s); removed since lua 5.2
   lua_pushcfunction(L, &pmain);
   lua_pushlightuserdata(L, &s);
   status = lua_pcall(L, 1, 0, 0);
   report(L, status);
   lua_close(L);
#ifndef OAPC_EXT_EXPORTS
 #ifdef ENV_WINDOWS
   WSACleanup();
 #endif //ENV_WINDOWS
   oapc_thread_mutex_release(m_luaMutex);
#else //OAPC_EXT_EXPORTS
   oapc_thread_mutex_release(data->m_luaMutex);
#endif //OAPC_EXT_EXPORTS
   return (status || s.status) ? EXIT_FAILURE : EXIT_SUCCESS;
}

