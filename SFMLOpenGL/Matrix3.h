/// Matrix 3 Class
/// Name: Robin Meyler
/// Login: C00231699
/// Estimated time: 3/4 hours
/// Time taken: 4 hours coding
///				2 hours tests/ tidying
/// 
#ifndef MY_MATRIX
#define MY_MATRIX
#include "MyVector3.h"
#include<iostream>
#include<math.h>
#define PI         3.14159265358979323846

class MyMatrix3
{
public:
	
	/// Default destructor, run if nothing is assigned
	/// Normal contructor, if members are assigned, constructs
	MyMatrix3();
	MyMatrix3(
		double t_a11, double t_a12, double t_a13,
		double t_a21, double t_a22, double t_a23,
		double t_a31, double t_a32, double t_a33);

	/// Takes in 3 vectors, and creates a 3x3 matrix
	/// Using the vectors are rows
	MyMatrix3(MyVector3 t_row1, MyVector3 t_row2, MyVector3 t_row3);

	/// Decontructor
	~MyMatrix3();

	/// Returns a the matrix members in string form, for output
	std::string toString()const;

	/// Equality check, to return whether true if two matrices are the same
	/// Inequality check to return whether true if two matrices are different
	bool operator ==(const MyMatrix3 t_other)const;
	bool operator !=(const MyMatrix3 t_other)const;

	/// The addition of two matrices by adding each member to its equivalent
	MyMatrix3 operator +(const MyMatrix3 t_other) const;

	/// The subtraction of two matrices by subtracting one member in one matrix from its equivalent
	MyMatrix3 operator -(const MyMatrix3 t_other) const;

	/// Multiplying two matrices together using the formula
	MyMatrix3 operator *(const MyMatrix3 t_other) const;

	/// Multiplying a vector with a matrix to return a vector
	MyVector3 operator *(const MyVector3 t_vector)const;
	
	/// Multiplying a matrix with a scaler constant 
	MyMatrix3 operator *(const double t_scale)const;

	/// Transpose a matrix
	/// Essentially draw a diagonal from top left to bottom right of the matrix
	/// Rotate the top right and bottom left corners 180 degrees relative to the diagonal
	MyMatrix3 transpose()const;

	/// The determinant of a matrix
	/// Used to find the inverse
	double determinant() const;

	/// Inverse of the matrix
	/// If a matrix is multiplied by its inverse, it = 1
	MyMatrix3 inverse() const;

	/// Returns a specific row as a vector
	/// Returns a specific column as a vector
	MyVector3 row(const int t_row)const; 
	MyVector3 column(const int t_column) const; 

	
	/// Returns a degree specific formula to mulitply by a vector to rotate it about an axis
	/// Takes degrees, changes to radians 
	static MyMatrix3 rotationZ(const double t_angleRadians); 
	static MyMatrix3 rotationY(const double t_angleRadians); 
	static MyMatrix3 rotationX(const double t_angleRadians); 

	/// Moves the x and y coordinates without changing the z coordinate, moving in left or right in 2d
	static MyMatrix3 translation(const MyVector3 t_displacement); 

	/// Returns matrix based on a scaling factor that, mulitplied by a matrix, scales the matix
	static MyMatrix3 scale(const double t_scalingfactor);

private:
	double m_11;
	double m_12;
	double m_13;
	double m_21;
	double m_22;
	double m_23;
	double m_31;
	double m_32;
	double m_33;
};

#endif // !MY_MATRIX
