#include <curses.h>
#define LATIN	0
#define GREEK	1

#define NONE	0
#define TON	1
#define SEP	2

static int lch = 0; /* last characte exept F12 */
static int lang = LATIN;

char latin_lower[] = "abgdezhuiklmnjoprwstyfxcv";
unsigned char greek_lower[] =
{ 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249 };

char latin_caps[] = "ABGDEZHUIKLMNJOPRSWTYFXCV";
unsigned char greek_caps[] =
{ 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 211, 211, 212, 213, 214, 215, 216, 217 };

char latin_ton[] = "aehioyvAEHIOYV";
unsigned char greek_ton[] =
{ 220, 221, 222, 223, 252, 253, 254, 182, 184, 185, 186, 188, 190, 191 };

char latin_sep[] = "iy";
unsigned char greek_sep[] =
{ 250, 251 };

int gr(int i, int conversion)
{
	int p;

	if (conversion == TON)
		for (p = 0; latin_ton[p] != '\0'; ++p)
			if (i == latin_ton[p])
				return (greek_ton[p]);
	if (conversion == SEP)
		for (p = 0; latin_sep[p] != '\0'; ++p)
			if (i == latin_sep[p])
				return (greek_sep[p]);
	for (p = 0; latin_lower[p] != '\0'; ++p)
		if (i == latin_lower[p])
			return (greek_lower[p]);
	for (p = 0; latin_caps[p] != '\0'; ++p)
		if (i == latin_caps[p])
			return (greek_caps[p]);

	return (i);
}

int lastchar()
{
	return (lch);
}

int grgetch()
{
	int ch;

	ch = getch();
	if (ch == KEY_F0 + 12){
		if (lang == LATIN)
			lang = GREEK;
		else
			lang = LATIN;
		return (grgetch());
	}
	lch = ch;
	if (lang == LATIN)
		return (ch);
	else{
		if (ch == ';'){
			ch = getch();
			return (gr(ch, TON));
		}
		if (ch == ':'){
			ch = getch();
			return (gr(ch, SEP));
		}
		return (gr(ch, NONE));
	}
}
