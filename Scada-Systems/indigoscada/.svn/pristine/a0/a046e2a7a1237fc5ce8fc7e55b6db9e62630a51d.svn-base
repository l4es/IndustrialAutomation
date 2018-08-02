//-< DATEIME.H >-----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Date-time field type
//-------------------------------------------------------------------*--------*

#ifndef __DATETIME_H__
#define __DATETIME_H__

#include "stdtp.h"
#include "class.h"

BEGIN_GIGABASE_NAMESPACE


/**
 * Date and time class
 */
class GIGABASE_DLL_ENTRY dbDateTime {
    int4 stamp;
  public:
    bool operator == (dbDateTime const& dt) {
        return stamp == dt.stamp;
    }
    bool operator != (dbDateTime const& dt) {
        return stamp != dt.stamp;
    }
    bool operator > (dbDateTime const& dt) {
        return stamp > dt.stamp;
    }
    bool operator >= (dbDateTime const& dt) {
        return stamp >= dt.stamp;
    }
    bool operator < (dbDateTime const& dt) {
        return stamp < dt.stamp;
    }
    bool operator <= (dbDateTime const& dt) {
        return stamp <= dt.stamp;
    }
    int operator - (dbDateTime const& dt) {
        return stamp - dt.stamp;
    }
    int operator + (dbDateTime const& dt) {
        return stamp + dt.stamp;
    }

    /**
     * Get current timestmap
     */
    static dbDateTime current() {
        return dbDateTime(time(NULL));
    }
    
    /**
     * Date constructor from time in seconds since midnight, January 1, 1970 UTC.
     */     
    dbDateTime(time_t t) {
        stamp = (int4)t;
    }
    
    /**
     * Constructor of invalid timestamp
     */
    dbDateTime() {
        stamp = -1;
    }
    
    /**
     * Check if timestamp is valid
     */
    bool isValid() const {
        return stamp != -1;
    }

    /**
     * Convert to number of seconds since midnight, January 1, 1970 UTC.
     */
    time_t asTime_t() const { return stamp; }

    /**
     * Set invalid timestamp
     */
    void clear() { stamp = -1; }

    /**
     * Construct timestamp 
     * @param year year in long format
     * @param month  month 1..12
     * @param day day of month 1..31
     * @param hour hour 0..23
     * @param min minute 0..59
     * @param sec second 0..59
     */
    dbDateTime(int year, int month, int day,
               int hour=0, int min=0, int sec = 0)
    {
        struct tm t;
        t.tm_year = year > 1900 ? year - 1900 : year;
        t.tm_mon = month-1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = min;
        t.tm_sec = sec;
        t.tm_isdst = -1;
        stamp = (int4)mktime(&t);
    }
    
    /**
     * Construct timestamp with only time specified
     */
    dbDateTime(int hour, int min) {
        stamp = (hour*60+min)*60;
    }

#if HAVE_LOCALTIME_R && !defined(NO_PTHREADS)
    /**
     * Get year, for example 2002
     */
    int year() {
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_year + 1900;
    }
    /**
     * Get month: 1..12
     */
    int month() { // 1..12
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_mon + 1;
    }
    /**
     * Get day 1..31
     */
    int day() { // 1..31
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_mday;
    }
    /**
     * Get day of year 1..366
     */
    int dayOfYear() { // 1..366
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_yday+1;
    }
    /**
     * Get day of week 1..7
     */
    int dayOfWeek() { // 1..7
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_wday+1;
    }
    /**
     * Get hour: 0..23
     */
    int hour() { // 0..24
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_hour;
    }
    /**
     * Get minute: 0..59
     */
    int minute() { // 0..59
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_min;
    }
    /**
     * Get second: 0..59
     */
    int second() { // 0..59
        struct tm t;
        return localtime_r((time_t*)&stamp, &t)->tm_sec;
    }
    /**
     * Convert timestamp to string
     * @param buf buffer to receive formatted string
     * @param buf_size size of the buffer
     * @param format format as in C library <code>strftime</code> function
     * @return pointer to the buffer
     */
    char_t* asString(char* buf, int buf_size, char_t const* format = "%c") const {
        struct tm t;
        strftime(buf, buf_size, format, localtime_r((const time_t*)&stamp, &t));
        return buf;
    }
    /**
     * Get current timestamp
     */
    static dbDateTime currentDate() {
        struct tm t;
        time_t curr = time(NULL);
        localtime_r(&curr, &t);
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        return dbDateTime(mktime(&t));
    }
#else
    /**
     * Get year, for example 2002
     */
    int year() {
        return localtime((time_t*)&stamp)->tm_year + 1900;
    }
    /**
     * Get month: 1..12
     */
    int month() { // 1..12
        return localtime((time_t*)&stamp)->tm_mon + 1;
    }
    /**
     * Get day 1..31
     */
    int day() { // 1..31
        return localtime((time_t*)&stamp)->tm_mday;
    }
    /**
     * Get day of year 1..366
     */
    int dayOfYear() { // 1..366
        return localtime((time_t*)&stamp)->tm_yday+1;
    }
    /**
     * Get day of week 1..7
     */
    int dayOfWeek() { // 1..7
        return localtime((time_t*)&stamp)->tm_wday+1;
    }
    /**
     * Get hour: 0..23
     */
    int hour() { // 0..24
        return localtime((time_t*)&stamp)->tm_hour;
    }
    /**
     * Get minute: 0..59
     */
    int minute() { // 0..59
        return localtime((time_t*)&stamp)->tm_min;
    }
    /**
     * Get second: 0..59
     */
    int second() { // 0..59
        return localtime((time_t*)&stamp)->tm_sec;
    }
    /**
     * Convert timestamp to string
     * @param buf buffer to receive formatted string
     * @param buf_size size of the buffer
     * @param format format as in C library <code>strftime</code> function
     * @return pointer to the buffer
     */
    char_t* asString(char_t* buf, int buf_size, char_t const* format = _T("%c")) const {
        STRFTIME(buf, buf_size, format, localtime((time_t*)&stamp));
        return buf;
    }
    /**
     * Get current timestamp
     */
    static dbDateTime currentDate() {
        time_t curr = time(NULL);
        struct tm* tp = localtime(&curr);
        tp->tm_hour = 0;
        tp->tm_min = 0;
        tp->tm_sec = 0;
        return dbDateTime(mktime(tp));
    }
#endif

    CLASS_DESCRIPTOR(dbDateTime,
                     (KEY(stamp,INDEXED|HASHED),
                      METHOD(year), METHOD(month), METHOD(day),
                      METHOD(dayOfYear), METHOD(dayOfWeek),
                      METHOD(hour), METHOD(minute), METHOD(second)));

    /**
     * Generate query expresson for comparing timestamps for equality
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator == (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T("="),stamp;
        return expr;
    }
    /**
     * Generate query expresson for comparing timestamps for inequality
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator != (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T("<>"),stamp;
        return expr;
    }
    /**
     * Generate query expresson for comparing timestamps
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator < (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T(">"),stamp;
        return expr;
    }
    /**
     * Generate query expresson for comparing timestamps
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator <= (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T(">="),stamp;
        return expr;
    }
    /**
     * Generate query expresson for comparing timestamps
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator > (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T("<"),stamp;
        return expr;
    }
    /**
     * Generate query expresson for comparing timestamps
     * @param field name of the record of type dbDateTime
     * @return query subexpression which compare specified table field with <code>this</code> date
     */
    dbQueryExpression operator >= (char_t const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,_T("stamp")),_T("<="),stamp;
        return expr;
    }
    /**
     * Generate query expresson for checking that timestamp belongs to the specfied range
     * @param field name of the record of type dbDateTime
     * @param from timestamp specifying start of interval (inclusive)
     * @param till timestamp specifying end of interval (inclusive)
     * @return query subexpression which checks that specified table field belongs to the sepcifed interval
     */
    friend dbQueryExpression between(char_t const* field, dbDateTime& from,
                                     dbDateTime& till)
    {
        dbQueryExpression expr;
        expr=dbComponent(field,_T("stamp")),_T("between"),from.stamp,_T("and"),till.stamp;
        return expr;
    }

    /**
     * Generate query expresson for ordering search result by dbDateTime field in ascent order
     * @param field subexpression to be used in ORDER BY clause
     */
    static dbQueryExpression ascent(char_t const* field) {
        dbQueryExpression expr;
        expr=dbComponent(field,_T("stamp"));
        return expr;
    }
    /**
     * Generate query expresson for ordering search result by dbDateTime field in descent order
     * @param field subexpression to be used in ORDER BY clause
     */
    static dbQueryExpression descent(char_t const* field) {
        dbQueryExpression expr;
        expr=dbComponent(field,_T("stamp")),_T("desc");
        return expr;
    }
};

END_GIGABASE_NAMESPACE

#endif







