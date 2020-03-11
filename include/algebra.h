#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_

#define PI 3.14159265

typedef struct { double x, y, z; } Vector;
typedef struct { double x, y, z, w; } HomVector;

/* Column-major order are used for the matrices here to be comnpatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
*/
typedef struct matrix { double e[16]; } Matrix;

extern Matrix model_view;
extern Matrix proj_matrix;

/* user defined functions */
float clamp(float val);
Matrix get_rotation_matrix(double degrees, char axis);
Matrix get_scalar_matrix(Vector s);
Matrix get_translation_matrix(Vector a, Vector b);
Matrix get_unit_matrix();
Matrix scalar_matrix(double x, double y, double z);
Matrix rotation_matrix(double degrees, double x, double y, double z);
Matrix translation_matrix(double x, double y, double z);
double max_val(double a, double b);
double min_val(double a, double b);
double absolute(double a);
/* for shading */
Vector get_vec_offset(Vector v_in, Vector v_offset);
Vector get_vec_normal(Vector a, Vector b);
Vector get_face_normal(Vector a, Vector b, Vector c);
/* returns the cotangent(x) */
double degrees_to_radians(double degrees);
double cot(double x);
float round_float(float x);

/* predefined functions */
Vector Add(Vector a, Vector b);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
double DotProduct(Vector a, Vector b);
double Length(Vector a);
Vector Normalize(Vector a);
HomVector Normalize(HomVector a);
Vector ScalarVecMul(double t, Vector a);
HomVector MatVecMul(Matrix a, Vector b);
Vector Homogenize(HomVector a);
Matrix MatMatMul(Matrix a, Matrix b);
void PrintMatrix(char *name, Matrix m);
void PrintVector(char *name, Vector v);
void PrintHomVector(char *name, HomVector h);

#endif
