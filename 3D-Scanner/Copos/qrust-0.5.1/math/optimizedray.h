/************************************************************************
 * Fichier          : optimizedray.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef OPTIMIZEDRAY_H
#define OPTIMIZEDRAY_H

/**
 * Namespace
 */
namespace math {
  // Class OptimizedRay
  // An optimized ray to test the intersection with a aligned box
  // 
  template<class TYPE>
  class OptimizedRay {
    /**
     * Public stuff
     */
  public:
    /**
     * Fields
     */
    /**
     * The origine of the ray
     */
    TYPE origine;
    /**
     * The direction of the ray
     */
    TYPE direction;
    /**
     * The inverse of the direction (1/x)
     */
    TYPE inv_direction;
    /**
     * An array to store the sign of the direction
     */
    int sign[3];
    /**
     * Constructors
     */
    /**
     * The constructor of this ray
     * @param o The origine of the ray
     * @param d The direction of the ray
     */
    OptimizedRay (const TYPE& o, const TYPE& d) : 
      origine(o),
      direction(d),
      inv_direction(TYPE(1/d[0], 1/d[1], 1/d[2]))
    {
      sign[0] = (inv_direction[0] < 0);
      sign[1] = (inv_direction[1] < 0);
      sign[2] = (inv_direction[2] < 0);
    };
  };
}
#endif //OPTIMIZEDRAY_H

