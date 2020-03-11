#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"

/* clamp the value between 0.0 and 1.0 */
float clamp(float val)
{
	return val < 0.0 ? 0.0 : (val > 1.0 ? 1.0 : val);
}

Matrix get_rotation_matrix(double degrees, char axis)
{
	Matrix m;
	m = get_unit_matrix();

	switch (axis) {
		case 'x':
			m.e[5] = cos(degrees);
			m.e[6] = sin(degrees);
			m.e[9] = -sin(degrees);
			m.e[10] = cos(degrees);
			break;
		case 'y':
			m.e[0] = cos(degrees);
			m.e[2] = -sin(degrees);
			m.e[8] = sin(degrees);
			m.e[10] = cos(degrees);
			break;
		case 'z':
			m.e[0] = cos(degrees);
			m.e[1] = sin(degrees);
			m.e[4] = -sin(degrees);
			m.e[5] = cos(degrees);
			break;
	}

	return m;
}

Matrix get_scalar_matrix(Vector s)
{
	Matrix m;
	m = get_unit_matrix();
	m.e[0] = s.x;
	m.e[5] = s.y;
	m.e[10] = s.z;

	return m;
}

Matrix get_translation_matrix(Vector a, Vector b)
{
	Matrix m;
	m = get_unit_matrix();
	m.e[12] = b.x - a.x;
	m.e[13] = b.y - a.y;
	m.e[14] = b.z - a.z;

	return m;
}

Matrix get_unit_matrix()
{
	Matrix u;
	u.e[0] = 1.0; u.e[1] = 0.0; u.e[2] = 0.0; u.e[3] = 0.0;
	u.e[4] = 0.0; u.e[5] = 1.0; u.e[6] = 0.0; u.e[7] = 0.0;
	u.e[8] = 0.0; u.e[9] = 0.0; u.e[10] = 1.0; u.e[11] = 0.0;
	u.e[12] = 0.0; u.e[13] = 0.0; u.e[14] = 0.0; u.e[15] = 1.0;

	return u;
}

Matrix scalar_matrix(double x, double y, double z)
{
	Matrix S;
	S = get_unit_matrix();
	S.e[0] = x;
	S.e[5] = y;
	S.e[10] = z;

	return S;
}

Matrix rotation_matrix(double degrees, double x, double y, double z)
{
	/* not entirely sure about * */
	Matrix Rx, Ry, Rz, Rxyz;
	Rx = get_rotation_matrix(degrees * x, 'x');
	Ry = get_rotation_matrix(degrees * y, 'y');
	Rz = get_rotation_matrix(degrees * z, 'z');

	Rxyz = MatMatMul(Rx, Ry);
	Rxyz = MatMatMul(Rxyz, Rz);
	return Rxyz;
}

Matrix translation_matrix(double x, double y, double z)
{
	Matrix T;
	T = get_unit_matrix();
	T.e[12] = x;
	T.e[13] = y;
	T.e[14] = z;

	return T;
}

double max_val(double a, double b)
{
	return a > b ? a : b;
}

double min_val(double a, double b)
{
	return a < b ? a : b;
}

double absolute(double a)
{
	if (a < 0) return -a;
	else return a;
}

Vector get_vec_offset(Vector v_in, Vector v_offset)
{
	Vector v;
	v.x = v_in.x - v_offset.x;
	v.y = v_in.y - v_offset.y;
	v.z = v_in.z - v_offset.z;

	return v;
}

Vector get_vec_normal(Vector a, Vector b)
{
	return CrossProduct(a, b);
}

Vector get_face_normal(Vector a, Vector b, Vector c)
{
	Vector v_o1, v_o2, vn;

	v_o1 = get_vec_offset(c, b);
	v_o2 = get_vec_offset(a, b);

	vn = get_vec_normal(v_o1, v_o2);

	return Normalize(vn);
}

double degrees_to_radians(double degrees)
{
	return degrees * (PI/180);
}

double cot(double degrees)
{
	double radians = degrees_to_radians(degrees);
	return 1.0/tan(radians);
}

float round_float(float x)
{
	/* a little nasty hack */
	int xx = (int)x;
	return (float)xx + 0.5f > x ? (float)xx : (float)xx + 1.0f;
}

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

double DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

double Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	double len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

HomVector Normalize(HomVector a)
{
	double len = sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
	HomVector v = { a.x/len, a.y/len, a.z/len, a.w/len };
	return v;
}

Vector ScalarVecMul(double t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

void PrintVector(char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}
