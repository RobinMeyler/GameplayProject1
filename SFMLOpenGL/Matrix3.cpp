/// Matrix 3 Class
/// Name: Robin Meyler
/// Login: C00231699
/// Estimated time: 3/4 hours
/// Time taken: 4 hours coding
///				2 hours tests/ tidying
/// 
/// 
/// 
/// </summary>
#include "Matrix3.h"
#include <string>


MyMatrix3::MyMatrix3() :
	m_11{ 0.0 },
	m_12{ 0.0 },				// Default contructor
	m_13{ 0.0 },
	m_21{ 0.0 },
	m_22{ 0.0 },
	m_23{ 0.0 },
	m_31{ 0.0 },
	m_32{ 0.0 },
	m_33{ 0.0 }
{
}

MyMatrix3::MyMatrix3(
	double t_a11, double t_a12, double t_a13,
	double t_a21, double t_a22, double t_a23,
	double t_a31, double t_a32, double t_a33) :

	m_11{ t_a11 },						// Assigns inputs to members when declared
	m_12{ t_a12 },
	m_13{ t_a13 },
	m_21{ t_a21 },
	m_22{ t_a22 },
	m_23{ t_a23 },
	m_31{ t_a31 },
	m_32{ t_a32 },
	m_33{ t_a33 }
{
}
MyMatrix3::MyMatrix3(MyVector3 t_row1, MyVector3 t_row2, MyVector3 t_row3) :

	m_11{ t_row1.x },					// Creates a matrix when passed 3 vectors
	m_12{ t_row1.y },
	m_13{ t_row1.z },
	m_21{ t_row2.x },
	m_22{ t_row2.y },
	m_23{ t_row2.z },
	m_31{ t_row3.x },
	m_32{ t_row3.y },
	m_33{ t_row3.z }
{
}

MyMatrix3::~MyMatrix3()
{
}

std::string MyMatrix3::toString()	const									// Matrix to string for output
{
	const std::string output = " |" + std::to_string(m_11) + "," + std::to_string(m_12) + "," + std::to_string(m_13) + "|\n"
		+ " |" + std::to_string(m_21) + "," + std::to_string(m_22) + "," + std::to_string(m_23) + "|\n"
		+ " |" + std::to_string(m_31) + "," + std::to_string(m_32) + "," + std::to_string(m_33) + "|";

	return output;
}

MyMatrix3 MyMatrix3::operator + (const MyMatrix3 t_other) const				// Add matrices
{
	return MyMatrix3(	m_11 + t_other.m_11,   m_12 + t_other.m_12,   m_13 + t_other.m_13,
						m_21 + t_other.m_21, m_22 + t_other.m_22,   m_23 + t_other.m_23,
						m_31 + t_other.m_31,   m_32 + t_other.m_32,   m_33 + t_other.m_33	);
}

MyMatrix3 MyMatrix3::operator - (const MyMatrix3 t_other) const				// Sub matrices
{
	return MyMatrix3(	m_11 - t_other.m_11, m_12 - t_other.m_12, m_13 - t_other.m_13,
						m_21 - t_other.m_21, m_22 - t_other.m_22, m_23 - t_other.m_23,
						m_31 - t_other.m_31, m_32 - t_other.m_32, m_33 - t_other.m_33);
}

MyMatrix3 MyMatrix3::operator * (const double t_mult) const					// Multy by scalar
{
	return MyMatrix3(	m_11 * t_mult, m_12 * t_mult, m_13 * t_mult,
						m_21 * t_mult, m_22 * t_mult, m_23 * t_mult,
						m_31 * t_mult, m_32 * t_mult, m_33 * t_mult);
}

MyMatrix3 MyMatrix3::operator * (const MyMatrix3 t_mult) const				// Matrix by Matrix
{
	return MyMatrix3(	m_11*t_mult.m_11 + m_12 * t_mult.m_21 + m_13 * t_mult.m_31,
						m_11*t_mult.m_12 + m_12 * t_mult.m_22 + m_13 * t_mult.m_32, 
						m_11*t_mult.m_13 + m_12 * t_mult.m_23 + m_13 * t_mult.m_33,

						m_21*t_mult.m_11 + m_22 * t_mult.m_21 + m_23 * t_mult.m_31, 
						m_21*t_mult.m_12 + m_22 * t_mult.m_22 + m_23 * t_mult.m_32, 
						m_21*t_mult.m_13 + m_22 * t_mult.m_23 + m_23 * t_mult.m_33, 

						m_31*t_mult.m_11 + m_32 * t_mult.m_21 + m_33 * t_mult.m_31, 
						m_31*t_mult.m_12 + m_32 * t_mult.m_22 + m_33 * t_mult.m_32, 
						m_31*t_mult.m_13 + m_32 * t_mult.m_23 + m_33 * t_mult.m_33 );
}

MyVector3 MyMatrix3::operator * (const MyVector3 t_mult) const				// Matrix by Vector
{
	return MyVector3(	m_11*t_mult.x + m_12 * t_mult.y + m_13 * t_mult.z,
						m_21*t_mult.x + m_22 * t_mult.y + m_23 * t_mult.z,
						m_31*t_mult.x + m_32 * t_mult.y + m_33 * t_mult.z );
}

bool MyMatrix3::operator ==(const MyMatrix3 t_other)const					// Equality
{
	const bool equality{	m_11 == t_other.m_11 && m_12 == t_other.m_12 && m_13 == t_other.m_13
						 && m_21 == t_other.m_21 && m_22 == t_other.m_22 && m_23 == t_other.m_23 
					     && m_31 == t_other.m_31 && m_32 == t_other.m_32 && m_33 == t_other.m_33 };					// Checking the equality of the coordinates
	return equality;
}
bool MyMatrix3::operator !=(const MyMatrix3 t_other)const					// Inequality
{
	const bool inequality{	 m_11 != t_other.m_11 || m_12 != t_other.m_12 || m_13 != t_other.m_13
						  || m_21 != t_other.m_21 || m_22 != t_other.m_22 || m_23 != t_other.m_23
						  || m_31 != t_other.m_31 || m_32 != t_other.m_32 || m_33 != t_other.m_33 };					// Checking the equality of the coordinates
	return inequality;
}

MyMatrix3 MyMatrix3::transpose()const										// Transpose
{
	return MyMatrix3(m_11, m_21, m_31, m_12, m_22, m_32, m_13, m_23, m_33);
}

double MyMatrix3::determinant() const										// Determinant
{
	double determinant{ (m_11*(m_22*m_33 - m_32 * m_23)) - (m_12 * (m_33*m_21 - m_31 * m_23)) + (m_13 * (m_32*m_21 - m_22 * m_31)) };

	return determinant;
}

MyMatrix3 MyMatrix3::inverse() const										// Inverse
{
	MyMatrix3 inverse{};
	double deter = determinant();
	if (deter != 0)							
	{
		inverse = { m_33*m_22 - m_32 * m_23,						// Doing the transpose while assigning to save time
					m_32*m_13 - m_33 * m_12, 
					m_23*m_12 - m_22 * m_13, 

					m_31*m_23 - m_33 * m_21, 
					m_33*m_11 - m_31 * m_13,
					m_21*m_13 - m_23 * m_11,

					m_32*m_21 - m_31 * m_22,
					m_31*m_12 - m_32 * m_11,
					m_22*m_11 - m_21 * m_12 };
		inverse =  inverse * (1.0 / deter);						
	}
	return inverse;
}

MyVector3 MyMatrix3::row(const int t_row)const				// Returns the row as a vector
{
	MyVector3 rowVec{};
	if (t_row == 0)
	{
		rowVec = {m_11, m_12, m_13};
	}
	else if (t_row == 1)
	{
		rowVec = { m_21, m_22, m_23 };
	}
	else if (t_row == 2)
	{
		rowVec = { m_31, m_32, m_33 };
	}
	return rowVec;
}

MyVector3 MyMatrix3::column(const int t_column) const			// Returns the column as a vector
{
	MyVector3 colVec{};
	if (t_column == 0)
	{
		colVec = { m_11, m_21, m_31 };
	}
	else if (t_column == 1)
	{
		colVec = { m_12, m_22, m_32 };
	}
	else if (t_column == 2)
	{
		colVec = { m_13, m_23, m_33 };
	}
	return colVec;
}

MyMatrix3 MyMatrix3::rotationZ(const double t_angleRadians)					
{																
	MyMatrix3 rotateZ{ cos(t_angleRadians), -sin(t_angleRadians),  0			// Rotate about the Z, so Z doesn't move Anticlockwise
					  ,sin(t_angleRadians),  cos(t_angleRadians),  0			// Based on trig
					  ,0		   ,  0			  ,  1 };
	return rotateZ;
}

MyMatrix3 MyMatrix3::rotationY(const double t_angleRadians)
{
	MyMatrix3 rotateY{	cos(t_angleRadians),  0,	sin(t_angleRadians),			// Rotate about the Y, so Y doesn't move Anticlockwise	
						0			,  1,    0,
					    -sin(t_angleRadians),  0,   cos(t_angleRadians) };
	return rotateY;
}

MyMatrix3 MyMatrix3::rotationX(const double t_angleRadians)
{																			
	MyMatrix3 rotateZ{   1,		0,				0,								
						 0,		cos(t_angleRadians),  -sin(t_angleRadians),			// Rotate about the X, so X doesn't move Anticlockwise
						 0,		sin(t_angleRadians),   cos(t_angleRadians) };
	return rotateZ;
}


MyMatrix3 MyMatrix3::translation(const MyVector3 t_displacement)
{
	return MyMatrix3{	1, 			0,		t_displacement.x,				// We want to move the X and Y along the Z axis, If we wanted to move all 3 points, we'd use
						0,			1,		t_displacement.y,				// a 4x4 matrix encumbering the elusive 4th dimension, and moving relative to it
						0,			0,						1	};
}

MyMatrix3 MyMatrix3::scale(const double t_scalingfactor)
{
	return MyMatrix3{	t_scalingfactor, 0,					0,					// We want only one version of the scalar for each line, like suduko, they can't align or else it will double count
						0,				 t_scalingfactor,	0,
						0,				 0,					t_scalingfactor };
}

