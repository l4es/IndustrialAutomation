//-< FILE.CPP >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// System dependent implementation of mapped on memory file
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#define _LARGEFILE64_SOURCE 1 // access to files greater than 2Gb in Solaris
#define _LARGE_FILE_API     1 // access to files greater than 2Gb in AIX
#define _GNU_SOURCE           // for definition of O_DIRECT on Linux

#include "gigabase.h"

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

BEGIN_GIGABASE_NAMESPACE

dbFile::~dbFile()
{
}

#if defined(_WIN32)
#if defined(SPARSE_FILE_OPTIMIZATION) && (_WIN32_WINNT >= 0x0500)
#include <Winioctl.h>
#endif

class OS_info : public OSVERSIONINFO {
  public:
    OS_info() {
        dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(this);
    }
};

static OS_info osinfo;

#define BAD_POS 0xFFFFFFFF // returned by SetFilePointer and GetFileSize


dbOSFile::dbOSFile()
{
    fh = INVALID_HANDLE_VALUE;
}

int dbOSFile::open(char_t const* fileName, int attr)
{
    directIO = false;
    fh = CreateFile(fileName, (attr & read_only)
                    ? GENERIC_READ : GENERIC_READ|GENERIC_WRITE, 
                    (attr & read_only) ? FILE_SHARE_READ : 0, NULL,
                    (attr & read_only) ? OPEN_EXISTING : (attr & truncate) ? CREATE_ALWAYS : OPEN_ALWAYS,
#ifdef _WINCE
                        FILE_ATTRIBUTE_NORMAL,
#else
                    ((attr & no_buffering) ? FILE_FLAG_NO_BUFFERING : 0) 
                    | ((attr & sequential) ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS),
#endif
                    NULL);
    if (fh == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }
#ifndef _WINCE
    if (attr & no_buffering) { 
         directIO = true;
    }
#endif
#if defined(SPARSE_FILE_OPTIMIZATION) && (_WIN32_WINNT >= 0x0500)
    if (!(attr & read_only)) {
        DWORD bytes_returned;
        if (!DeviceIoControl(fh, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &bytes_returned, NULL)) {
            return GetLastError();
        }
    }
#endif
    return ok;
}

int dbOSFile::read(offs_t pos, void* buf, size_t size)
{
    DWORD readBytes;
    if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        OVERLAPPED Overlapped;
        Overlapped.Offset = nat8_low_part(pos);
        Overlapped.OffsetHigh = nat8_high_part(pos);
        Overlapped.hEvent = NULL;
        if (ReadFile(fh, buf, size, &readBytes, &Overlapped)) {
            return readBytes == size ? ok : eof;
        } else {
            int rc = GetLastError();
            return (rc == ERROR_HANDLE_EOF) ? eof : rc;
        }
    } else {
        LONG high_pos = nat8_high_part(pos);
        LONG low_pos = nat8_low_part(pos);
        dbCriticalSection cs(mutex);
        if (SetFilePointer(fh, low_pos,
                           &high_pos, FILE_BEGIN) != BAD_POS
            && ReadFile(fh, buf, size, &readBytes, NULL))
        {
            return (readBytes == size) ? ok : eof;
        } else {
            int rc = GetLastError();
            return rc == ERROR_HANDLE_EOF ? eof : rc;
        }
    }
}

int dbOSFile::read(void* buf, size_t size)
{
    DWORD readBytes;
    if (ReadFile(fh, buf, size, &readBytes, NULL)) {
        return (readBytes == size) ? ok : eof;
    } else {
        int rc = GetLastError();
        return rc == ERROR_HANDLE_EOF ? eof : rc;
    }
}

int dbOSFile::setSize(offs_t size)
{
    LONG low_pos = nat8_low_part(size);
    LONG high_pos = nat8_high_part(size);
    if (SetFilePointer(fh, low_pos,
                       &high_pos, FILE_BEGIN) == BAD_POS
        || !SetEndOfFile(fh))
    {
        return GetLastError();
    }
    return ok;    
}


int dbOSFile::write(void const* buf, size_t size)
{
    DWORD writtenBytes;
    return !WriteFile(fh, buf, size, &writtenBytes, NULL)
        ? GetLastError() : (writtenBytes == size) ? ok : eof;
}

int dbOSFile::write(offs_t pos, void const* buf, size_t size)
{
    DWORD writtenBytes;
    if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        OVERLAPPED Overlapped;
        Overlapped.Offset = nat8_low_part(pos);
        Overlapped.OffsetHigh = nat8_high_part(pos);
        Overlapped.hEvent = NULL;
        return WriteFile(fh, buf, size, &writtenBytes, &Overlapped)
            ? writtenBytes == size ? ok : eof
            : GetLastError();
    } else {
        LONG high_pos = nat8_high_part(pos);
        LONG low_pos = nat8_low_part(pos);
        dbCriticalSection cs(mutex);
        return SetFilePointer(fh, low_pos, &high_pos, FILE_BEGIN)
            == BAD_POS ||
            !WriteFile(fh, buf, size, &writtenBytes, NULL)
            ? GetLastError()
            : (writtenBytes == size) ? ok : eof;
    }
}


int dbOSFile::flush()
{
    if (directIO) { 
        return ok;
    }
    return FlushFileBuffers(fh) ? ok : GetLastError();
}

int dbOSFile::close()
{
    if (fh != INVALID_HANDLE_VALUE) {
        if (CloseHandle(fh)) {
            fh = INVALID_HANDLE_VALUE;
            return ok;
        } else {
            return GetLastError();
        }
    } else {
        return ok;
    }
}

void* dbOSFile::allocateBuffer(size_t size, bool lock)
{
    void* buf = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (buf != NULL) {
#ifndef _WINCE 
        if (lock) { 
            DWORD minimumWorkingSetSize, maximumWorkingSetSize;
            GetProcessWorkingSetSize(GetCurrentProcess(),
                                     &minimumWorkingSetSize, 
                                     &maximumWorkingSetSize);
            
            if (size > minimumWorkingSetSize) {
                minimumWorkingSetSize = size + 4*1024*1024;
            }
            if (maximumWorkingSetSize < minimumWorkingSetSize) { 
                maximumWorkingSetSize = minimumWorkingSetSize;
            }
            if (!SetProcessWorkingSetSize(GetCurrentProcess(),
                                          minimumWorkingSetSize,
                                          maximumWorkingSetSize)) 
            {
                printf("Failed to extend process working set: %d\n", (int)GetLastError());
            }
            if (!VirtualLock(buf, size)) { 
                printf("Virtuanl lock failed with status: %d\n", (int)GetLastError());
            }        
        }
#endif
    }
    return buf;
}

void dbOSFile::protectBuffer(void* buf, size_t size, bool readonly)
{
    DWORD oldProt;
    VirtualProtect(buf, size, readonly ? PAGE_READONLY : PAGE_READWRITE, &oldProt);
}



void  dbOSFile::deallocateBuffer(void* buffer, size_t size, bool unlock)
{
#ifndef _WINCE
    if (unlock) {
        VirtualUnlock(buffer, size);
    }
#endif
    VirtualFree(buffer, 0, MEM_RELEASE);
}

size_t dbOSFile::ramSize()
{
    MEMORYSTATUS memStat;
    GlobalMemoryStatus(&memStat);
    return memStat.dwTotalPhys;
}


char_t* dbOSFile::errorText(int code, char_t* buf, size_t bufSize)
{
    int len;

    switch (code) {
      case ok:
        STRNCPY(buf, STRLITERAL("No error"), bufSize-1);
        break;
      case eof:
        STRNCPY(buf, STRLITERAL("Transfer less bytes than specified"), bufSize-1);
        break;
      default:
        len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            code,
                            0,
                            buf,
                            bufSize-1,
                            NULL);
        if (len == 0) {
            char_t errcode[64];
            SPRINTF(errcode, STRLITERAL("unknown error %u"), code);
            STRNCPY(buf, errcode, bufSize-1);
        }
    }
    buf[bufSize-1] = '\0';
    return buf;
}

#else // Unix

END_GIGABASE_NAMESPACE

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

BEGIN_GIGABASE_NAMESPACE

#ifdef __linux__
#define lseek(fd, offs, whence) lseek64(fd, offs, whence)
#endif

dbOSFile::dbOSFile()
{
    fd = -1;
}

#if defined(__linux__)
bool directIoSupported(char const* fileName)
{
    int dummy = 0;
    char* buf = new char[strlen(fileName) + 8];
    sprintf(buf, "%s.direct", fileName);
    int fd = open(buf, O_WRONLY|O_CREAT|O_DIRECT, 0666);
    if (fd < 0) {         
        delete[] buf;
        return false;
    }
    int rc = write(fd, (char*)&dummy+1, 1);
    int error = errno;
    close(fd);
    unlink(buf);
    delete[] buf;
    return rc < 0 && error == EINVAL;
}
#endif

int dbOSFile::open(char_t const* fileName, int attr)
{
    char* name;
#ifdef UNICODE 
    char buf[1024];
    wcstombs(buf, fileName, sizeof buf);
    name = buf;
#else
    name = (char*)fileName;
#endif
    directIO = false;
#if defined(__sun)
    fd = ::open64(name, ((attr & read_only) ? O_RDONLY : O_CREAT|O_RDWR)
                  | ((attr & truncate) ? O_TRUNC : 0), 0666);
    if (fd >= 0 && (attr & no_buffering) != 0) { 
        if (directio(fd, DIRECTIO_ON) == 0) { 
            directIO = true;
        }
    }
#elif defined(_AIX)
#if defined(_AIX43)
    fd = ::open64(name, ((attr & read_only) ? O_RDONLY : O_CREAT|O_RDWR|O_LARGEFILE|O_DSYNC|
                         ((attr & no_buffering) ? O_DIRECT : 0)), 0666);
    if (attr & no_buffering) {
        directIO = true;
    }                  
#else
     fd = ::open64(name, ((attr & read_only) ? O_RDONLY : O_CREAT|O_RDWR|O_LARGEFILE
#endif /* _AIX43 */
                   | ((attr & truncate) ? O_TRUNC : 0), 0666);
#elif defined(__linux__)
    fd = ::open(name, O_LARGEFILE
               | ((attr & read_only) ? O_RDONLY : O_CREAT|O_RDWR)
               | ((attr & truncate) ? O_TRUNC : 0)
               | ((attr & no_buffering) ? O_DIRECT : 0), 0666);
    if ((attr & no_buffering) && directIoSupported(name)) {
        directIO = true;
    }                  
#else
    fd = ::open(name, O_LARGEFILE | ((attr & read_only) ? O_RDONLY : O_CREAT|O_RDWR)
                | ((attr & truncate) ? O_TRUNC : 0), 0666);
#endif
    if (fd < 0) {
        return errno;
    }
    return ok;
}

int dbOSFile::setSize(offs_t size)
{
    return ftruncate(fd, size);
}

int dbOSFile::read(offs_t pos, void* buf, size_t size)
{
    ssize_t rc;
#if defined(__sun) || defined(_AIX43)
    rc = pread64(fd, buf, size, pos);
#else
    {
        dbCriticalSection cs(mutex);
        if (offs_t(lseek(fd, pos, SEEK_SET)) != pos) {
            return errno;
        }
        rc = ::read(fd, buf, size);
    }
#endif
    if (rc == -1) {
        return errno;
    } else if (size_t(rc) != size) {
        return eof;
    } else {
        return ok;
    }
}

int dbOSFile::read(void* buf, size_t size)
{
    ssize_t rc = ::read(fd, buf, size);
    if (rc == -1) {
        return errno;
    } else if (size_t(rc) != size) {
        return eof;
    } else {
        return ok;
    }
}

int dbOSFile::write(void const* buf, size_t size)
{
    ssize_t rc = ::write(fd, buf, size);
    if (rc == -1) {
        return errno;
    } else if (size_t(rc) != size) {
        return eof;
    } else {
        return ok;
    }
}

int dbOSFile::write(offs_t pos, void const* buf, size_t size)
{
    ssize_t rc;
#if defined(__sun) || defined(_AIX43)
    rc = pwrite64(fd, buf, size, pos);
#else
    {
        dbCriticalSection cs(mutex);
        if (offs_t(lseek(fd, pos, SEEK_SET)) != pos) {
            return errno;
        }
        rc = ::write(fd, buf, size);
    }
#endif
    if (rc == -1) {
        return errno;
    } else if (size_t(rc) != size) {
        return eof;
    } else {
        return ok;
    }
}

int dbOSFile::flush()
{
    if (directIO) { 
        return ok;
    }
    return fsync(fd) != ok ? errno : ok;
}

int dbOSFile::close()
{
    if (fd != -1) {
        if (::close(fd) == ok) {
            fd = -1;
            return ok;
        } else {
            return errno;
        }
    } else {
        return ok;
    }
}

void* dbOSFile::allocateBuffer(size_t size, bool)
{
#if defined(__MINGW32__) || defined(__CYGWIN__) || (defined(__QNX__) && !defined(__QNXNTO__))
    return malloc(size);
#else
    return valloc(size);
#endif
}

void  dbOSFile::deallocateBuffer(void* buffer, size_t, bool)
{
    free(buffer);
}

#ifdef __linux__
END_GIGABASE_NAMESPACE
#include <sys/sysinfo.h>
BEGIN_GIGABASE_NAMESPACE

size_t dbOSFile::ramSize()
{
    struct sysinfo info;
    sysinfo(&info);
#ifdef SYSINFO_HAS_NO_MEM_UNIT
    return info.totalram;
#else
    return info.totalram*info.mem_unit;
#endif
}
#else
size_t dbOSFile::ramSize()
{
#if defined(__sun)
    return sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGE_SIZE);
#else
    const size_t minMemorySize = 64000000;
    return minMemorySize;
#endif
}
#endif

char_t* dbOSFile::errorText(int code, char_t* buf, size_t bufSize)
{
    switch (code) {
      case ok:
        STRNCPY(buf, STRLITERAL("No error"), bufSize-1);
        break;
      case eof:
        STRNCPY(buf, STRLITERAL("Transfer less bytes than specified"), bufSize-1);
        break;
      default:
        STRNCPY(buf, strerror(code), bufSize-1);
    }
    buf[bufSize-1] = '\0';
    return buf;
}

#endif

int dbMultiFile::open(int n, dbSegment* seg, int attr)
{
    segment = new dbFileSegment[n];
    nSegments = n;
    while (--n >= 0) {
        segment[n].size = seg[n].size*dbPageSize;
        segment[n].offs = seg[n].offs;
        int rc = segment[n].open(seg[n].name, attr);
        if (rc != ok) {
            while (++n < nSegments) {
                segment[n].close();
            }
            return rc;
        }
    }
    return ok;
}

int dbMultiFile::close()
{
    if (segment != NULL) {
        for (int i = nSegments; --i >= 0;) {
            int rc = segment[i].close();
            if (rc != ok) {
                return rc;
            }
        }
        delete[] segment;
        segment = NULL;
    }
    return ok;
}

int dbMultiFile::setSize(offs_t)
{
    return ok;
}

int dbMultiFile::flush()
{
    for (int i = nSegments; --i >= 0;) {
        int rc = segment[i].flush();
        if (rc != ok) {
            return rc;
        }
    }
    return ok;
}


int dbMultiFile::write(offs_t pos, void const* ptr, size_t size)
{
    int n = nSegments-1;
    char const* src = (char const*)ptr;
    for (int i = 0; i < n; i++) {
        if (pos < segment[i].size) {
            if (pos + size > segment[i].size) {
                int rc = segment[i].write(segment[i].offs + pos, src, size_t(segment[i].size - pos));
                if (rc != ok) {
                    return rc;
                }
                size -= size_t(segment[i].size - pos);
                src += size_t(segment[i].size - pos);
                pos = 0;
            } else {
                return segment[i].write(segment[i].offs + pos, src, size);
            }
        } else {
            pos -= segment[i].size;
        }
    }
    return segment[n].write(segment[n].offs + pos, src, size);
}

int dbMultiFile::read(offs_t pos, void* ptr, size_t size)
{
    int n = nSegments-1;
    char* dst = (char*)ptr;
    for (int i = 0; i < n; i++) {
        if (pos < segment[i].size) {
            if (pos + size > segment[i].size) {
                int rc = segment[i].read(segment[i].offs + pos, dst, size_t(segment[i].size - pos));
                if (rc != ok) {
                    return rc;
                }
                size -= size_t(segment[i].size - pos);
                dst += size_t(segment[i].size - pos);
                pos = 0;
            } else {
                return segment[i].read(segment[i].offs + pos, dst, size);
            }
        } else {
            pos -= segment[i].size;
        }
    }
    return segment[n].read(segment[n].offs + pos, dst, size);
}


int dbRaidFile::setSize(offs_t)
{
    return ok;
}

int dbRaidFile::write(offs_t pos, void const* ptr, size_t size)
{
    char const* src = (char const*)ptr;
    while (true) { 
        int i = (int)(pos / raidBlockSize % nSegments);
        int offs = (unsigned)pos % raidBlockSize;
        size_t available = raidBlockSize - offs;
        if (available >= size) { 
            return segment[i].write(segment[i].offs + pos / (raidBlockSize*nSegments) * raidBlockSize + offs, src, size);
        }
        int rc = segment[i].write(segment[i].offs + pos / (raidBlockSize*nSegments) * raidBlockSize + offs, src, available);
        if (rc != ok) {
            return rc;
        }
        src += available;
        pos += available;
        size -= available;
    }
}
            

int dbRaidFile::read(offs_t pos, void* ptr, size_t size)
{
    char* dst = (char*)ptr;
    while (true) { 
        int i = (int)(pos / raidBlockSize % nSegments);
        int offs = (unsigned)pos % raidBlockSize;
        size_t available = raidBlockSize - offs;
        if (available >= size) { 
            return segment[i].read(segment[i].offs + pos / (raidBlockSize*nSegments) * raidBlockSize + offs, dst, size);
        }
        int rc = segment[i].read(segment[i].offs + pos / (raidBlockSize*nSegments) * raidBlockSize + offs, dst, available);
        if (rc != ok) {
            return rc;
        }
        dst += available;
        pos += available;
        size -= available;
    }
}
            
END_GIGABASE_NAMESPACE






