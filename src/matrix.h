
#ifndef _SW_MATRIX_H
#define _SW_MATRIX_H
#include "base.h"

/**
* cairo_matrix_t:

* @xx: xx component of the affine transformation
* @yx: yx component of the affine transformation
* @xy: xy component of the affine transformation
* @yy: yy component of the affine transformation
* @x0: X translation component of the affine transformation
* @y0: Y translation component of the affine transformation
*
* A #cairo_matrix_t holds an affine transformation, such as a scale,
* rotation, shear, or a combination of those. The transformation of
* a point (x, y) is given by:
* <programlisting>
*     x_new = xx * x + xy * y + x0;
*     y_new = yx * x + yy * y + y0;
* </programlisting>
**/

/*
typedef struct _cairo_matrix {
	double xx; double yx;
	double xy; double yy;
	double x0; double y0;
} cairo_matrix_t;

typedef cairo_matrix_t  swMatrixT;
#define cairo_bool_t bool

#define cairo_private 
#define  cairo_status_t int
#define  CAIRO_STATUS_SUCCESS 0
#define CAIRO_STATUS_INVALID_MATRIX 1

#define _cairo_error(e) -1

void cairo_matrix_init (cairo_matrix_t *matrix,
									 double xx, double yx,
									 double xy, double yy,
									 double x0, double y0);
void cairo_matrix_init_identity (cairo_matrix_t *matrix);									
void	cairo_matrix_init_rotate (cairo_matrix_t *matrix,double radians) ;
void cairo_matrix_translate (cairo_matrix_t *matrix, double tx, double ty);									 
void cairo_matrix_scale (cairo_matrix_t *matrix, double sx, double sy);
void cairo_matrix_rotate (cairo_matrix_t *matrix, double radians);
void cairo_matrix_multiply (cairo_matrix_t *result, const cairo_matrix_t *a, const cairo_matrix_t *b);
void cairo_matrix_transform_point (const cairo_matrix_t *matrix, double *x, double *y);
double _cairo_matrix_compute_determinant (const cairo_matrix_t *matrix);

*/



//typedef cairo_matrix_t  swMatrixT;
#endif
