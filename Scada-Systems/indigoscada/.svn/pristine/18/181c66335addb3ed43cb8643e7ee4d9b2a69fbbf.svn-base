//-< SET.H >---------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     17-Jan-99    K.A. Knizhnik  * / [] \ *
//                          Last update: 25-Feb-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Small set implementation. Number of elements in the set should
// not be greater than 64. Template class is used to allow static
// type checking for different sets.
//-------------------------------------------------------------------*--------*

#ifndef __SET_H__
#define __SET_H__

BEGIN_GIGABASE_NAMESPACE

/**
  * Small set implementation. Number of elements in the set should
  * not be greater than 64. Template class is used to allow static
  * type checking for different sets.
  * Template parameter is intended to be enum. 
  */
template<class T>
class dbSet {
  protected:
    typedef db_int8 set_t;
    set_t bits;
  public:
    dbSet(set_t elements = 0) : bits(elements) {}
    dbSet(T elem) : bits((set_t)1 << elem) {}
    dbSet(dbSet<T> const& s) : bits(s.bits) {}

    dbSet(T from, T till)  {
        assert(from <= till);
        bits = ((set_t)1 << till << 1) - ((set_t)1 << till);
    }

    bool operator == (dbSet<T> const& s) const {
        return bits == s.bits;
    }
    bool operator != (dbSet<T> const& s) const {
        return bits != s.bits;
    }
    bool operator == (T elem) const {
        return bits == ((set_t)1 << elem);
    }
    bool operator != (T elem) const {
        return bits != ((set_t)1 << elem);
    }
    bool operator <= (dbSet<T> const& s) const {
        return (bits & ~s.bits) == 0;
    }
    bool operator >= (dbSet<T> const& s) const {
        return (s.bits & ~bits) == 0;
    }
    bool operator < (dbSet<T> const& s) const {
        return (bits & ~s.bits) == 0 && bits != s.bits;
    }
    bool operator > (dbSet<T> const& s) const {
        return (s.bits & ~bits) == 0 && bits != s.bits;
    }
    dbSet<T> operator + (dbSet<T> const& s) const {
        dbSet<T> result(bits | s.bits);
        return result;
    }
    dbSet<T> operator + (T elem) const {
        dbSet<T> result(bits | ((set_t)1 << elem));
        return result;
    }
    dbSet<T> operator - (dbSet<T> const& s) const {
        dbSet<T> result(bits & ~s.bits);
        return result;
    }
    dbSet<T> operator - (T elem) const {
        dbSet<T> result(bits & ~((set_t)1 << elem));
        return result;
    }
    dbSet<T> operator * (dbSet<T> const& s) const {
        dbSet<T> result(bits & s.bits);
        return result;
    }
    dbSet<T> operator * (T elem) const {
        dbSet<T> result(bits & ((set_t)1 << elem));
        return result;
    }
    bool has(T elem) const {
        return (bits & ((set_t)1 << elem)) != 0;
    }
    bool empty() {
        return bits == 0;
    }
    dbSet<T> operator += (T elem) {
        bits |= (set_t)1 << elem;
        return *this;
    }
    dbSet<T> operator -= (T elem) {
        bits &= ~((set_t)1 << elem);
        return *this;
    }
    dbSet<T> operator = (dbSet<T> const& s) {
        bits = s.bits;
        return *this;
    }
    dbSet<T> operator = (T elem) {
        bits = (set_t)1 << elem;
        return *this;
    }

    /**
     * Join two sets
     */
    dbSet<T> operator, (dbSet<T> const& s) {
        dbSet<T> result(bits | s.bits);
        return result;
    }
    /**
     * Add element to the set
     */
    dbSet<T> operator, (T elem) {
        dbSet<T> result(bits | ((set_t)1 << elem));
        return result;
    }

    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator == (char const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,"bits"),"=",bits;
        return expr;
    }
    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator != (char const* field) {
        dbQueryExpression expr;
        expr = dbComponent(field,"bits"),"!=",bits;
        return expr;
    }
    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator <= (char const* field) {
        dbQueryExpression expr;
        expr = "(not",dbComponent(field,"bits"),"and",bits,")=0";
        return expr;
    }
    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator >= (char const* field) {
        dbQueryExpression expr;
        expr = "(not",bits,"and",dbComponent(field,"bits"),")=0";
        return expr;
    }
    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator < (char const* field) {
        dbQueryExpression expr;
        expr = "(not",dbComponent(field,"bits"),"and",bits,")=0 and",
            dbComponent(field,"bits"),"!=",bits;
        return expr;
    }
    /**
     * Generate query expression to compare two sets
     * @param field name of the field of dbSet type
     * @return query expression comparing the table field with this set
     */
    dbQueryExpression operator > (char const* field) {
        dbQueryExpression expr;
        expr = "(not",bits,"and",dbComponent(field,"bits"),")=0 and",
            dbComponent(field,"bits"),"!=",bits;
        return expr;
    }
    /**
     * Generate query expression to check if field is in set
     * @param field name of the field of integer type
     * @return query expression checing if table field is in this set
     */
    dbQueryExpression has(char const* field) {
        dbQueryExpression expr;
        expr = "(",bits,"and 2^",dbComponent(field),") <> 0";
        return expr;
    }

    CLASS_DESCRIPTOR(dbSet<T>, (FIELD(bits), METHOD(empty)));
};

END_GIGABASE_NAMESPACE

#endif
