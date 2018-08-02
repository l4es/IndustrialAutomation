char * strrev(char *s)
{
    int j,k,c;
    for(k=0;s[k] != 0;k++);
    for(j=0,k--;j<k;j++,k--) {
	c = s[j];
	s[j] = s[k];
	s[k] = c;
    }
    return s;
}
