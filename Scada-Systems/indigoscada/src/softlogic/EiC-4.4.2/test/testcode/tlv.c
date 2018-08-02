/* test für lokale variable */

int x=0;

/* der folgende Block enthält keine Befehle, aber er sollte x trotzdem
   hochzählen. Mal sehen ob das tut ... */
{
    int a=x++;
}

printf ("x = %d (1)\n", x);
