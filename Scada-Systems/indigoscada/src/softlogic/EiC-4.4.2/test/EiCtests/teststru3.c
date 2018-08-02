/*******
The following code, pulled into EiC using a #include will
report an error with regard to incomplete declarations.   We
have a workaround, by using typedef {...} Geometry_e;, but the
bug bears fixing at some point.
********/

#include <stdio.h>

enum Geometry_e { e_SlabGeometry = 1, e_CylindricalGeometry,
e_SphericalGeometry };

struct OneD_Hydro_s {
    int     Version;
    enum Geometry_e alpha;
};
  
int    main( ) {

    struct OneD_Hydro_s a, b, c;

    a.alpha = e_SlabGeometry;
    b.alpha = e_CylindricalGeometry;
    c.alpha = e_SphericalGeometry;

    printf( "a = %d\n", a.alpha );
    printf( "b = %d\n", b.alpha );
    printf( "c = %d\n", c.alpha );
  
    printf("sizeof = %d\n",(int)sizeof(enum Geometry_e));
  return( 0 );
    }

#ifdef EiCTeStS
main();
#endif






