void regline(float *x,
             float *y,
             int n,
             float *m,
             float *b
             )
{
    /* In a least square sense, find the equation:
     * y = mx + b; Returns the slope in `m' and
     * the offset in `b', from the data given in
     * `y' and `x'. `n' being the size of the
     * arrays `y' and `x'.
     */
    double m00, m11, m10, m01,m20;
    int i;
    m01 = m10 = m11 = m20 = 0;
    m00 = n;
    for(i=0;i<n;++i,++x,++y) {
        m10 += *x;
        m20 += *x * *x;
        m01 += *y;
        m11 += *x * *y;
    }
    /* determine slope */
    *m = (m00*m11 - m10*m01)/(m00*m20- m10*m10);
    /* determine offset */
    *b = (m01 - m10* *m)/m00;
}
