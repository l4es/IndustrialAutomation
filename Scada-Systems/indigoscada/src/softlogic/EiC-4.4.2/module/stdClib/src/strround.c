int strround(char *s,int len)
{
    /* string rounding function
     * (c) Edmond J. Breen.
     * round back the numbers in a string
     * and fill with zeros.
     * where: 'len' is the length of the string.
     * Returns 1 on success
     * and 0 if over flow has occurred.
     */
    if(len>0)
	if(s[--len] >= '5') {
	    do {
		s[len--] = '0';
	    } while(len > 0 && s[len] == '9');
	    if(s[len] == '9')
		return 0;
	    s[len]++;
	}
    return 1;
}
