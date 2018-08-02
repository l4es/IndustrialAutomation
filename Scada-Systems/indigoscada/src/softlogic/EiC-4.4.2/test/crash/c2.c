int g(char c);

int f (char *p)
{
  char c;

  c = *++p;
  if (c != ' ')
    return 0;
  for (;;)
    {
      c = *p;
      if (g (c))
	p++;
      else
	{
	  if (c == ' ')
	    break;
	  else
	    return 0;
	}
    }
  return 1;
}

int main()
{
    return 0;
}
