//-< WWWAPI.CPP >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1997  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     27-Mar-99    K.A. Knizhnik  * / [] \ *
//                          Last update:  1-Jul-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Implementation of WWWapi class
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "wwwapi.h"

BEGIN_GIGABASE_NAMESPACE

const size_t init_reply_buffer_size = 1024;

#define hhex(ch) (((ch) >> 4) >= 10 ? ((ch) >> 4) + 'A' - 10 : ((ch) >> 4) + '0')
#define lhex(ch)  (((ch) & 0xF) >= 10 ? ((ch) & 0xF) + 'A' - 10 : ((ch) & 0xF) + '0')

inline unsigned string_hash_function(const char* name)
{
    unsigned h = 0, g;
    while(*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xF0000000) != 0) {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

#define ERROR_TEXT(x) \
"HTTP/1.1 " x "\r\n\
Connection: close\r\n\r\n\
<HTML><HEAD><TITLE>Invalid request to the database</TITLE>\r\n\
</HEAD><BODY>\n\r\
<H1>" x "</H1>\n\r\
</BODY></HTML>\r\n\r\n"


WWWconnection::WWWconnection()
{
    memset(hash_table, 0, sizeof hash_table);
    sock = NULL;
    reply_buf = new char[init_reply_buffer_size];
    reply_buf_size = init_reply_buffer_size;
    free_pairs = NULL;
    peer = NULL;
    userData = NULL;
    userDataDestructor = NULL;
}

WWWconnection::~WWWconnection()
{
    reset();
    name_value_pair *nvp, *next;
    for (nvp = free_pairs; nvp != NULL; nvp = next)
    {
        next = nvp->next;
        delete nvp;
    }
    delete[] reply_buf;
    
    delete[] peer;

    if (userDataDestructor != NULL && userData != NULL) { 
        userDataDestructor(userData);
    }
}


char* WWWconnection::extendBuffer(size_t inc)
{
    if (reply_buf_used + inc >= reply_buf_size) {
        reply_buf_size = reply_buf_size*2 > reply_buf_used + inc
            ? reply_buf_size*2 : reply_buf_used + inc;

        char* new_buf = new char[reply_buf_size+1];
        memcpy(new_buf, reply_buf, reply_buf_used);
        delete[] reply_buf;
        reply_buf = new_buf;
    }
    reply_buf_used += inc;
    return reply_buf;
}

bool WWWconnection::terminatedBy(char const* str) const
{
    size_t len = strlen(str);
    if (len > reply_buf_used - 4) {
        return false;
    }
    return memcmp(reply_buf + reply_buf_used - len, str, len) == 0;
}

WWWconnection& WWWconnection::append(const void *buf, int len) {
    int pos = reply_buf_used;
    char *dst = extendBuffer(len);
    memcpy(dst + pos, buf, len);
    return *this;
}


WWWconnection& WWWconnection::append(char const* str)
{
    int pos = reply_buf_used;
    char* dst = extendBuffer(strlen(str));
    unsigned char ch;
    switch (encoding) {
      case TAG:
        strcpy(dst + pos, str);
        encoding = HTML;
        break;
      case HTML:
        encoding = TAG;
#if 1 // MS-Explorer handle "&nbsp;" in HTML string literals in very strange way
        if (str[0] == ' ' && str[1] == '\0') { 
            strcpy(extendBuffer(5) + pos, "&nbsp;");
            return *this;
        }
#endif            
        while (true) {
            switch(ch = *str++) {
              case '<':
                dst = extendBuffer(3);
                dst[pos++] = '&';
                dst[pos++] = 'l';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case '>':
                dst = extendBuffer(3);
                dst[pos++] = '&';
                dst[pos++] = 'g';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case '&':
                dst = extendBuffer(4);
                dst[pos++] = '&';
                dst[pos++] = 'a';
                dst[pos++] = 'm';
                dst[pos++] = 'p';
                dst[pos++] = ';';
                break;
              case '"':
                dst = extendBuffer(5);
                dst[pos++] = '&';
                dst[pos++] = 'q';
                dst[pos++] = 'u';
                dst[pos++] = 'o';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case '\0':
                dst[pos] = '\0';
                return *this;
#if 0 // MS-Explorer handle "&nbsp;" in HTML string literals in very strange way
              case ' ':
                dst = extendBuffer(5);
                dst[pos++] = '&';
                dst[pos++] = 'n';
                dst[pos++] = 'b';
                dst[pos++] = 's';
                dst[pos++] = 'p';
                dst[pos++] = ';';
#endif
                break;                
              default:
                dst[pos++] = ch;
            }
        }
        break;
      case ENTITY:
        // This is to be used for XML Text.
        // Encoding must be correct.
        encoding = TAG;
        while (true) {
            switch (ch = *str++) { 
              case '<':
              case '>':
              case '&':
              case '"':
                {
                    char buf[32];
                    sprintf(buf, "&#%d;", ch);
                    dst = extendBuffer(strlen(buf)-1); // one already done
                    strcpy(dst + pos, buf);
                    pos+=strlen(buf);
                }
                break;
              case '\0':          
                dst[pos] = '\0';
                return *this;
              default: 
                dst[pos++] = ch;
            }
        }
        break;
      case URL:
        encoding = TAG;
        while (true) {
            ch = *str++;
            if (ch == '\0') {
                dst[pos] = '\0';
                return *this;
            } else if (ch == ' ') {
                dst[pos++] = '+';
            } else if (!isalnum(ch)) {
                dst = extendBuffer(2);
                dst[pos++] = '%';
                dst[pos++] = hhex(ch);
                dst[pos++] = lhex(ch);
            } else {
                dst[pos++] = ch;
            }
        }
    }
    return *this;
}

#ifdef UNICODE
WWWconnection& WWWconnection::append(wchar_t const* str)
{
    // Send unicode string.
    // UTF8 encoding is used because it's the usual way to do it
    // To send other encoding, used one byte string way...

    int pos = reply_buf_used;

  // Use size in character and not byte in case encoding is one byte long (most common case ?)
    char* dst = extendBuffer(wcslen(str));
    wchar_t ch;
    switch (encoding) {
      case TAG:
        while(true)
        {
            ch = *str++;
            if (ch== L'\0')
            {
                dst[pos] = '\0';
                return *this;
            }
            if (ch < 128)
                dst[pos++] = (char)ch;
            else if (ch <= 0x7FF)
            {
                dst = extendBuffer(1);
                dst[pos++] =  0xC0 | (ch >> 6);
                dst[pos++] =  0x80 | (ch & 0x3F);
            }
            else if (ch <= 0xFFFF)
            {
                dst = extendBuffer(2);
                dst[pos++] = 0xE0 | (ch >> 12);
                dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                dst[pos++] = 0x80 | (ch & 0x3F);
            }
            else if (ch <= 0x1FFFFF)
            {
                dst = extendBuffer(3);
                dst[pos++] = 0xF0 | (ch >> 18);
                dst[pos++] = 0x80 | ((ch >> 12) & 0x3F);
                dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                dst[pos++] = 0x80 | (ch & 0x3F);
            }
            else
            {
                dst = extendBuffer(5);
                dst[pos++] = 0xF8 | (ch >> 24);
                dst[pos++] = 0x80 | ((ch >> 18) & 0x3F);
                dst[pos++] = 0x80 | ((ch >> 12) & 0x3F);
                dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                dst[pos++] = 0x80 | (ch & 0x3F);
            }          
        }      
        encoding = HTML;
        break;

      case HTML:
        encoding = TAG;
        while (true) {
            switch(ch = *str++) {
              case L'<':
                dst = extendBuffer(3);
                dst[pos++] = '&';
                dst[pos++] = 'l';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case L'>':
                dst = extendBuffer(3);
                dst[pos++] = '&';
                dst[pos++] = 'g';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case L'&':
                dst = extendBuffer(4);
                dst[pos++] = '&';
                dst[pos++] = 'a';
                dst[pos++] = 'm';
                dst[pos++] = 'p';
                dst[pos++] = ';';
                break;
              case L'"':
                dst = extendBuffer(5);
                dst[pos++] = '&';
                dst[pos++] = 'q';
                dst[pos++] = 'u';
                dst[pos++] = 'o';
                dst[pos++] = 't';
                dst[pos++] = ';';
                break;
              case L'\0':
                dst[pos] = '\0';
                return *this;
              default:
                if (ch < 128)
                    dst[pos++] = (char)ch;
                else if (ch <= 0x7FF)
                {
                    dst = extendBuffer(1);
                    dst[pos++] =  0xC0 | (ch >> 6);
                    dst[pos++] =  0x80 | (ch & 0x3F);
                }
                else if (ch <= 0xFFFF)
                {
                    dst = extendBuffer(2);
                    dst[pos++] = 0xE0 | (ch >> 12);
                    dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                    dst[pos++] = 0x80 | (ch & 0x3F);
                }
                else if (ch <= 0x1FFFFF)
                {
                    dst = extendBuffer(3);
                    dst[pos++] = 0xF0 | (ch >> 18);
                    dst[pos++] = 0x80 | ((ch >> 12) & 0x3F);
                    dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                    dst[pos++] = 0x80 | (ch & 0x3F);
                }
                else
                {
                    dst = extendBuffer(5);
                    dst[pos++] = 0xF8 | (ch >> 24);
                    dst[pos++] = 0x80 | ((ch >> 18) & 0x3F);
                    dst[pos++] = 0x80 | ((ch >> 12) & 0x3F);
                    dst[pos++] = 0x80 | ((ch >> 6) & 0x3F);
                    dst[pos++] = 0x80 | (ch & 0x3F);
                }
            }
        }
        break;

      case ENTITY:
        encoding = TAG;
        while (true) {
            switch(ch = *str++) {
              case L'<':
              case L'>':
              case L'&':
              case L'"':
                  {
                      char buf[32];
                      sprintf(buf, "&#%d;", ch);
                      dst = extendBuffer(strlen(buf)-1); // one already done
                      strcpy(dst + pos, buf);
                      pos+=strlen(buf);
                  }
                  break;
              case L'\0':
                dst[pos] = '\0';
                return *this;
              default:
                if (ch < 128)
                    dst[pos++] = (char)ch;
                else
                {
                    char buf[32];
                    sprintf(buf, "&#%d;", ch);
                    dst = extendBuffer(strlen(buf)-1);
                    strcpy(dst + pos, buf);
                    pos+=strlen(buf);
                }
            }
        }
        break;
        
      case URL:
        encoding = TAG;
        while (true) {
            ch = *str++;
            if (ch == L'\0') {
                dst[pos] = '\0';
                return *this;
            } else if (ch == L' ') {
                dst[pos++] = '+';
            } else if (!isalnum(ch)) {
                dst = extendBuffer(2);
                dst[pos++] = '%';
            
                if (ch < 128)
                {
                    dst[pos++] = hhex(ch);
                    dst[pos++] = lhex(ch);
                }
                else if (ch <= 0x7FF)
                {
                    dst[pos++] = hhex(0xC0 | (ch >> 6));
                    dst[pos++] = lhex(0xC0 | (ch >> 6));
                    dst = extendBuffer(3);
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | (ch & 0x3F));
                    dst[pos++] = lhex(0x80 | (ch & 0x3F));
                }
                else if (ch <= 0xFFFF)
                {
                    dst[pos++] = hhex(0xE0 | (ch >> 12));
                    dst[pos++] = lhex(0xE0 | (ch >> 12));
                    dst = extendBuffer(6);
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 6) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 6) & 0x3F));              
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | (ch & 0x3F));
                    dst[pos++] = lhex(0x80 | (ch & 0x3F));              
                }
                else if (ch <= 0x1FFFFF)
                {
                    dst[pos++] = hhex(0xF0 | (ch >> 18));
                    dst[pos++] = lhex(0xF0 | (ch >> 18));  
                    dst = extendBuffer(9);              
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 12) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 12) & 0x3F));      
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 6) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 6) & 0x3F));              
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | (ch & 0x3F));
                    dst[pos++] = lhex(0x80 | (ch & 0x3F));    
                }
                else
                {
                    dst[pos++] = hhex(0xF8 | (ch >> 24));
                    dst[pos++] = lhex(0xF8 | (ch >> 24));  
                    dst = extendBuffer(12);
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 18) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 18) & 0x3F));    
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 12) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 12) & 0x3F));      
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | ((ch >> 6) & 0x3F));
                    dst[pos++] = lhex(0x80 | ((ch >> 6) & 0x3F));              
                    dst[pos++] = '%';
                    dst[pos++] = hhex(0x80 | (ch & 0x3F));
                    dst[pos++] = lhex(0x80 | (ch & 0x3F));   
                }
            } else {
                dst[pos++] = (char)ch;
            }
        }
    }
    return *this;
}
#endif

void WWWconnection::reset()
{
    reply_buf_used = 0;
    encoding = TAG;
    for (int i = itemsof(hash_table); --i >= 0;) {
        name_value_pair *nvp, *next;
        for (nvp = hash_table[i]; nvp != NULL; nvp = next) {
            next = nvp->next;
            nvp->next = free_pairs;
            free_pairs = nvp;
        }
        hash_table[i] = NULL;
    }
}

void WWWconnection::addPair(char const* name, char const* value)
{
    name_value_pair* nvp;
    if (free_pairs != NULL) {
        nvp = free_pairs;
        free_pairs = nvp->next;
    } else {
        nvp = new name_value_pair;
    }
    unsigned hash_code = string_hash_function(name);
    nvp->hash_code = hash_code;
    hash_code %= hash_table_size;
    nvp->next = hash_table[hash_code];
    hash_table[hash_code] = nvp;
    nvp->value = value;
    nvp->name = name;
}

#define HEX_DIGIT(ch) ((ch) >= 'a' ? ((ch) - 'a' + 10) : (ch) >= 'A' ? (((ch) - 'A' + 10)) : ((ch) - '0'))

char* WWWconnection::unpack(char* body, size_t length)
{
    char *src = body, *end = body + length;

    while (src < end) {
        char* name = src;
        char ch;
        char* dst = src;
        while (src < end && (ch = *src++) != '=') {
            if (ch == '+') {
                ch = ' ';
            } else if (ch == '%') {
                ch = (HEX_DIGIT(src[0]) << 4) | HEX_DIGIT(src[1]);
                src += 2;
            }
            *dst++ = ch;
        }
        *dst = '\0';
        char* value = dst = src;
        while (src < end && (ch = *src++) != '&') {
            if (ch == '+') {
                ch = ' ';
            } else if (ch == '%') {
                ch = (HEX_DIGIT(src[0]) << 4) | HEX_DIGIT(src[1]);
                src += 2;
            }
            *dst++ = ch;
        }
        *dst = '\0';
        addPair(name, value);
    }
    stub = get("stub");
    return get("page");
}


char* WWWconnection::get(char const* name, int n)
{
    unsigned hash_code = string_hash_function(name);
    name_value_pair* nvp;
    for (nvp = hash_table[hash_code % hash_table_size];
         nvp != NULL;
         nvp = nvp->next)
    {
        if (nvp->hash_code == hash_code && strcmp(nvp->name, name) == 0) {
            if (n == 0) {
                return (char*)nvp->value;
            }
            n -= 1;
        }
    }
    return NULL;
}




//--------------------------------------------------


WWWapi::WWWapi(dbDatabase& dbase, int n_handlers, dispatcher* dispatch_table, char const* rootpath)
: db(dbase)
{
    rootPath = NULL;
    if (rootpath && *rootpath)
    {
        rootPath = new char[strlen(rootpath)+1];
        strcpy(rootPath, rootpath);
    }
    memset(hash_table, 0, sizeof hash_table);
    sock = NULL;
    address = NULL;
    dispatcher* disp = dispatch_table;
    while (--n_handlers >= 0) {
        unsigned hash_code = string_hash_function(disp->page);
        disp->hash_code = hash_code;
        hash_code %= hash_table_size;
        disp->collision_chain = hash_table[hash_code];
        hash_table[hash_code] = disp;
        disp += 1;
    }
}

WWWapi::~WWWapi()
{
    delete[] rootPath;
    delete[] address;
}

bool WWWapi::open(char const* socket_address,
                  socket_t::socket_domain domain,
                  int listen_queue)
{
    if (sock != NULL) {
        close();
    }
    address = new char[strlen(socket_address) + 1];
    strcpy(address, socket_address);
    sock = domain != socket_t::sock_global_domain
        ? socket_t::create_local(socket_address, listen_queue)
        : socket_t::create_global(socket_address, listen_queue);
    canceled = false;
    if (!sock->is_ok()) {
        char_t buf[64];
        sock->get_error_text(buf, sizeof buf);
        FPRINTF(stderr, _T("WWWapi::open: create socket failed: %s\n"), buf);
        return false;
    }
    return true;
}




bool WWWapi::connect(WWWconnection& con)
{
    assert(sock != NULL);
    con.reset();
    delete con.sock;
    con.sock = sock->accept();
    con.address = address;
    if (con.sock == NULL) {
        if (!canceled) {
            char_t buf[64];
            sock->get_error_text(buf, sizeof buf);
            FPRINTF(stderr, _T("WWWapi::connect: accept failed: %s\n"), buf);
        }
        return false;
    }
    return true;
}

void WWWapi::cancel()
{
    canceled = true;
    sock->cancel_accept();
}

void WWWapi::close()
{
    delete sock;
    delete[] address;
    sock = NULL;
}



bool WWWapi::dispatch(WWWconnection& con, char* page)
{
    unsigned hash_code = string_hash_function(page);
    for (dispatcher* disp = hash_table[hash_code % hash_table_size];
         disp != NULL;
         disp = disp->collision_chain)
    {
        if (disp->hash_code == hash_code && strcmp(disp->page, page) == 0)
        {
            bool result = disp->func(con);
            db.commit();
            return result;
        }
    }
    return true;
}


void URL2ASCII(char* src)
{
    char* dst = src;
    char ch;
    while ((ch = *src++) != '\0') {
        if (ch == '%') {
            *dst++ = ((src[0] - '0') << 8) | (src[1] - '0');
        } else if (ch == '+') {
            *dst++ = ' ';
        } else if (ch == '.' && *src == '.') {
            // for security reasons do not allow access to parent directory
            break;
        } else {
            *dst++ = ch;
        }
    }
    *dst = '\0';
}


bool CGIapi::serve(WWWconnection& con)
{
    nat4 length;
    con.reset();
    if ((size_t)con.sock->read(&length, sizeof length, sizeof length)
        != sizeof(length))
    {
        return true;
    }
    int size = length - sizeof length;
    char* buf = new char[size];
    if (con.sock->read(buf, size, size) != size)
    {
        return true;
    }
    char* page = con.unpack(buf + buf[0], length - sizeof length - buf[0]);
    char* peer = con.get("peer");
    con.peer = new char[strlen(peer)+1];
    strcpy(con.peer, peer);
    bool result = true;
    if (page != NULL)
    {
        con.extendBuffer(4);
        result = dispatch(con, page);
        *(int4*)con.reply_buf = con.reply_buf_used;
        con.sock->write(con.reply_buf, con.reply_buf_used);
    }
    delete con.sock;
    con.sock = NULL; // close connection
    delete[] con.peer;
    con.peer = NULL;
    return result;
}


inline char* stristr(char const* s, char const* p) {
    while (*s != '\0') {
        int i;
        for (i = 0; (s[i] & ~('a'-'A')) == (p[i] & ~('a' - 'A')) && p[i] != '\0'; i++);
        if (p[i] == '\0') {
            return (char*)s;
        }
        s += 1;
    }
    return NULL;
}


bool HTTPapi::serve(WWWconnection& con)
{
    size_t inputBufferSize = 4*1024;
    char* buf = new char[inputBufferSize];
    bool result = false;
    size_t size = 0;
  
    con.peer = con.sock->get_peer_name();
  
    while (true) {
        con.reset();
        char* p = buf;
        char prev_ch = 0;
        do {
            if (p == buf + size) {
                int rc = con.sock->read(buf + size, 1, inputBufferSize - size - 1,
                                        connectionHoldTimeout);
                if (rc < 0) {
                    delete con.sock;
                    con.sock = NULL;
                    delete[] con.peer;      
                    con.peer = NULL;     
                    delete[] buf;      
                    return true;
                }
                if (rc < 1) {
                    con.append(ERROR_TEXT("200 OK")); // connection closed due to timeout expiration
                    break;
                }
                size += rc;
                if ((size + 1) > inputBufferSize)
                {
                    char* newbuf =  new char[inputBufferSize*2];
                    //memset(newbuf, 0, inputBufferSize*2);
                    memcpy(newbuf, buf, inputBufferSize);
                    p = newbuf + (p - buf); // new position of p
                    delete[] buf;
                    buf = newbuf;
                    inputBufferSize *= 2;
                    buf[size] = '\0';
                }
            }
            buf[size] = '\0';
            while (*p != '\0' && (prev_ch != '\n' || *p != '\r')) {
                prev_ch = *p++;
            }
        } while (*p == '\0' && p == buf + size); // p now points to the message body
        if (*p != '\r' || *(p+1) != '\n') {
            con.append(ERROR_TEXT("400 Bad Request"));
            break;
        }
        p += 2;
        int length = INT_MAX;
        char* lenptr = stristr(buf, "Content-Length: ");
        char* ptr = stristr(buf, "Connection: Keep-Alive");
        bool  persistentConnection = (ptr != NULL && (ptr == buf || (*ptr-1) == '\r' || (*ptr-1) == '\n' || (*ptr-1) == ' '));
        char* host = stristr(buf, "Host: ");
        if (host != NULL) {
            char* q = host += 6;
            while (*q != '\n' && *q != '\r' && *q != '\0') q += 1;
            *q = '\0';
        }
        if (lenptr != NULL) {
            sscanf(lenptr+15, "%d", &length);
        }
        if (strncmp(buf, "GET ", 4) == 0) {
            char* file, *uri = buf;
            file = strchr(uri, '/');
            if (file == NULL) {
                con.append(ERROR_TEXT("400 Bad Request"));
                break;
            }
            if (*++file == '/') {
                if (host == NULL) {
                    host = file+1;
                }
                file = strchr(uri, '/');
                if (file == NULL) {
                    con.append(ERROR_TEXT("400 Bad Request"));
                    break;
                }
                *file++ = '\0';
            }
            char* file_end = strchr(file, ' ');
            char index_html[] = "index.html";
            if (file_end == NULL) {
                con.append(ERROR_TEXT("400 Bad Request"));
                break;
            }
            if (file_end == file) {
                file = index_html;
            } else {
                *file_end = '\0';
            }
            char* params = strchr(file, '?');
            if (host == NULL) {
                host = (char*)"localhost";
            }
            if (params != NULL) {
                if (!handleRequest(con, params+1, file_end, host, result)) {
                    delete con.sock;
                    con.sock = NULL;
                    delete[] con.peer;      
                    con.peer = NULL;          
                    delete[] buf;      
                    return result;
                }
            } else {
                URL2ASCII(file);
                char path[4096];
                if(rootPath)
                {
                    strcpy(path,rootPath);
                    if (*file != '\\' && *file != '/'
                        && path[strlen(path)-1] != '\\' && path[strlen(path)-1] != '/')
                    {
                        strcat(path, "/");
                    }
                    strcat(path, file);
                } else { 
                    strcpy(path, file);
                }
                FILE* f = fopen(path, "rb");
                if (f == NULL) { 
                    if (strcmp(file, index_html) == 0) {
                        static char defaultPage[] = "page=defaultPage";
                        if (!handleRequest(con, defaultPage, defaultPage + strlen(defaultPage), host, result)) {
                            delete con.sock;
                            con.sock = NULL;
                            delete[] con.peer;      
                            con.peer = NULL;          
                            delete[] buf;      
                            return result;
                        } 
                    } else { 
                        con.append(ERROR_TEXT("404 File Not Found"));
                        break;
                    }
                } else { 
                    fseek(f, 0, SEEK_END);
                    size_t file_size = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    char reply[1024];
                    char* contentType;
                    char* contentSubtype;
                    char* suf = strrchr(file, '.');
                    if (suf != NULL) { 
                        if (strcmp(suf+1, "htm") == 0 || strcmp(suf+1, "html") == 0) { 
                            contentSubtype = "html";
                            contentType = "text";
                        }
                        else
                        {
                            if (strcmp(suf+1, "xml") == 0) { 
                                contentSubtype = "xml";
                                contentType = "text";
                            }
                            else
                            {
                                if (strcmp(suf+1, "xsl") == 0) { 
                                    contentSubtype = "xsl";
                                    contentType = "text";
                                }
                                else { 
                                    contentSubtype = suf+1;
                                    contentType = "image";
                                }
                            }
                        }
                    } else { 
                        contentSubtype = "binary";
                        contentType = "image";
                    }
                    sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Length: %u\r\n"
                            "Content-Type: %s/%s\r\nConnection: %s\r\n\r\n",
                            file_size, contentType, contentSubtype, 
                            keepConnectionAlive ? "Keep-Alive" : "close");
                    con.append(reply);
                    size_t pos = con.reply_buf_used;
                    char* dst = con.extendBuffer(file_size);
                    if (dst == NULL) {
                        con.reset();
                        con.append(ERROR_TEXT("413 Request Entity Too Large"));
                        break;
                    }
                    if (fread(dst + pos, 1, file_size, f) != file_size) {
                        con.reset();
                        con.append(ERROR_TEXT("500 Internal server error"));
                        break;
                    }
                    fclose(f);
                    if (!con.sock->write(dst, con.reply_buf_used)
                        || !keepConnectionAlive)
                    {
                        delete con.sock;
                        con.sock = NULL;
                        delete[] con.peer;      
                        con.peer = NULL; 
                        delete[] buf;      
                        return true;
                    }
                }
            }
        } else if (strncmp(buf, "POST ", 5) == 0) {
            char* body = p;
          ScanNextPart:
            int n = length < buf + size - p
                ? length : buf + size - p;
            while (--n >= 0 && *p && *p != '\r' && *p != '\n')
            {
                p += 1;
            }
            if (n < 0 && p - body < length) {
                if (size >= inputBufferSize - 1) {
                    con.append(ERROR_TEXT("413 Request Entity Too Large"));
                    break;
                }
                int rc = con.sock->read(p, 0, inputBufferSize - size - 1,
                                        connectionHoldTimeout);
                if (rc < 0) {
                    delete con.sock;
                    con.sock = NULL;
                    delete[] con.peer;      
                    con.peer = NULL;          
                    delete[] buf;      
                    return true;
                }
                size += rc;
                if ((size + 4) > inputBufferSize)
                {
                    char* newbuf =  new char[inputBufferSize*2];
                    //memset(newbuf, 0, inputBufferSize*2);          
                    memcpy(newbuf, buf, inputBufferSize);
                    p = newbuf + (p - buf); // new position of p          
                    body = newbuf + (body - buf); // new position of p 
                    host = newbuf + (host - buf);
                    delete[] buf;
                    buf = newbuf;
                    inputBufferSize *= 2;
                }
                buf[size]='\0';
        
                goto ScanNextPart;
            } else {
                if (host == NULL) {
                    host = (char*)"localhost";
                }
                if (!handleRequest(con, body, p, host, result)) {
                    delete con.sock;
                    con.sock = NULL;
                    delete[] con.peer;      
                    con.peer = NULL;   
                    delete[] buf;
                    return result;
                }
                while (n >= 0 && (*p == '\n' || *p == '\r')) {
                    p += 1;
                    n -= 1;
                }
            }
        } else {
            con.append(ERROR_TEXT("405 Method not allowed"));
            break;
        }
        if (!persistentConnection) {
            delete con.sock;
            con.sock = NULL;
            delete[] con.peer;      
            con.peer = NULL;
            delete[] buf;
            return true;
        }
        if (p - buf < (long)size) {
            size -= p - buf;
            memcpy(buf, p, size);
        } else {
            size = 0;
        }
    }
    if (con.sock != NULL) {
        con.sock->write(con.reply_buf, con.reply_buf_used);
        delete con.sock;
        con.sock = NULL;
    }
    delete[] con.peer;
    con.peer = NULL;
    delete[] buf;      
    
    return true;
}


bool HTTPapi::handleRequest(WWWconnection& con, char* begin, char* end,
                            char* host, bool& result)
{
    char buf[64];
    char ch = *end;
  
    *buf ='\0';
    char* page = con.unpack(begin, end - begin);
    if (page != NULL)  {
        con.append("HTTP/1.1 200 OK\r\nContent-Length:           \r\n");
        int length_pos = con.reply_buf_used - 12;
        con.append(keepConnectionAlive
                   ? "Connection: Keep-Alive\r\n"
                   : "Connection: close\r\n");
        sprintf(buf, "http://%s/", host);
        con.stub = buf;
        result = dispatch(con, page);
        char* body = con.reply_buf + length_pos;
        char prev_ch = 0;
        con.reply_buf[con.reply_buf_used] = '\0';
        while ((*body != '\n' || prev_ch != '\n') &&
               (*body != '\r' || prev_ch != '\n') &&
               *body != '\0')
        {
            prev_ch = *body++;
        }
        if (*body == '\0') {
            con.reset();
            con.append(ERROR_TEXT("404 Not found"));
            con.sock->write(con.reply_buf, con.reply_buf_used);
            return false;
        }
        body += *body == '\n' ? 1 : 2;
        sprintf(buf, "%u", (unsigned)(con.reply_buf_used - (body - con.reply_buf)));
        memcpy(con.reply_buf + length_pos,
               buf, strlen(buf));    
        if (!con.sock->write(con.reply_buf, con.reply_buf_used)) {
            return false;
        }
        *end = ch;
        return result && keepConnectionAlive;
    } else {
        con.append(ERROR_TEXT("406 Not acceptable"));
        con.sock->write(con.reply_buf, con.reply_buf_used);
        result = true;
        *end = ch;
        return false;
    }
}


//----------------------------------------------------

void thread_proc QueueManager::handleThread(void* arg)
{
    ((QueueManager*)arg)->handle();
}


QueueManager::QueueManager(WWWapi&     api,
                           dbDatabase& dbase,
                           int         nThreads,
                           int         connectionQueueLen
                           )
: db(dbase)
{
    assert(nThreads >= 1 && connectionQueueLen >= 1);
    this->nThreads = nThreads;
    go.open();
    done.open();
    threads = new dbThread[nThreads];
    while (--nThreads >= 0) {
        threads[nThreads].create(handleThread, this);
        threads[nThreads].detach();
    }
    connectionPool = new WWWconnection[connectionQueueLen];
    connectionPool[--connectionQueueLen].next = NULL;
    while (--connectionQueueLen >= 0) {
        connectionPool[connectionQueueLen].next =
            &connectionPool[connectionQueueLen+1];
    }
    freeList = connectionPool;
    waitList = NULL;
    server = &api;
}

void QueueManager::cleanup(WWWconnection::handler cleanupfunc)
{
    if (cleanupfunc)
    {
        mutex.lock();
        WWWconnection* cur = freeList;
        while (cur)
        {
            cleanupfunc(*cur);
            cur = cur->next;
        }
        cur = waitList;
        while (cur)
        {
            cleanupfunc(*cur);
            cur = cur->next;
        }
        mutex.unlock();
    }
}

void QueueManager::start()
{
    mutex.lock();
    while (server != NULL) {
        if (freeList == NULL) {
            done.reset();
            done.wait(mutex);
            if (server == NULL) {
                break;
            }
            assert(freeList != NULL);
        }
        WWWconnection* con = freeList;
        freeList = con->next;
        WWWapi* srv = server;
        mutex.unlock();
        if (!srv->connect(*con) || server == NULL) {
            return;
        }
        mutex.lock();
        con->next = waitList;
        waitList = con;
        go.signal();
    }
    mutex.unlock();
}


void QueueManager::handle()
{
    db.attach();
    mutex.lock();
    while (true) {
        go.wait(mutex);
        WWWapi* api = server;
        if (api == NULL) {
            break;
        }
        WWWconnection* con = waitList;
        assert(con != NULL);
        waitList = con->next;
        mutex.unlock();
        if (!api->serve(*con)) {
            stop();
        }
        mutex.lock();
        if (freeList == NULL) {
            done.signal();
        }
        con->next = freeList;
        freeList = con;
    }
    mutex.unlock();
    db.detach();
}


void QueueManager::stop()
{
    mutex.lock();
    WWWapi* server = this->server;
    this->server = NULL;
    server->cancel();
    while (--nThreads >= 0) {
        go.signal();
    }
    done.signal();
    mutex.unlock();
}


QueueManager::~QueueManager()
{
    go.close();
    done.close();
    delete[] threads;
    delete[] connectionPool;
}

END_GIGABASE_NAMESPACE

