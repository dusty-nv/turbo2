/*
 * rover
 */

#ifndef __ROVER_MATRIX4_TEMPLATE_H
#define __ROVER_MATRIX4_TEMPLATE_H



inline double Sqrt(double x) { return sqrt(x); }



// TODO get rid of these Cos/Sin definitions...
inline double Cos( double angle ) 
{
	return cos(angle);
}

inline float Cos( float angle )
{
	return cosf(angle);
}

inline double Sin( double angle ) 
{
	return sin(angle);
}

inline float Sin( float angle )
{
	return sinf(angle);
}



/**
 * [00 01 02 03] // m16 offsets
 * [04 05 06 07] 
 * [08 09 10 11] 
 * [12 13 14 15] 
 * 
 * [00 01 02 03] // m44 offsets --> [row][column] 
 * [10 11 12 13] 
 * [20 21 22 23] 
 * [30 31 32 33] 
 *  
 * [Xx Xy Xz 0]	// right
 * [Yx Yy Yz 0]	// up
 * [Zx Zy Zz 0]	// forward
 * [Tx Ty Tz 1]	// translation</pre>
 *
 */
template <class T> class TMatrix
{
	public:
		/**
 		 * Default constructor.
		 * This leaves the matrix uninitialized.
		 */
		inline TMatrix<T>()  {}

		/**
 		 * Initialize to identity.
		 */
		inline TMatrix<T>(const bool identity)												{ if (identity) Identity(); }

		/**
 		 * Copy constructor.
		 */
		inline TMatrix<T>(const TMatrix<T>& m);

		/**
 		 * Sets the matrix to identity.
		 */
		void Identity();

		/**
 		 * Transpose the matrix (swap rows with columns).
		 */
		void Transpose();
		
		/**
 		 * Multiply this matrix with another matrix and stores the result in itself.
		 */
		void MultMatrix(const TMatrix<T>& right);

		/**
 		 * Multiply this matrix with the 3x3 rotation part of the other matrix.
		 */
		void MultMatrix3x3(const TMatrix<T>& right);

		/**
		 * Multiply the left matrix with the right matrix and store the result in this matrix object.
		 */
		void MultMatrix(const TMatrix<T>& left, const TMatrix<T>& right);

		/**
 		 * Multiply a vector with the 3x3 rotation part of this matrix.
		 * T* is a float[3] or double[3] 3D vector
		 */
		inline void Mul3x3(T* v, T* o) const;

		/**
 		 * Makes the matrix an rotation matrix along the x-axis (in radians).
		 */
		void SetRotationMatrixX(const T angle);

		/**
 		 * Makes the matrix a rotation matrix along the y-axis (in radians).
		 */
		void SetRotationMatrixY(const T angle);

		/**
 		 * Makes the matrix a rotation matrix along the z-axis (in radians).
		 */
		void SetRotationMatrixZ(const T angle);

		/**
 		 * Translate the matrix.
		 */
		inline void Translate(const T x, const T y, const T z)								{ m44[3][0]+=x;	m44[3][1]+=y; m44[3][2]+=z; }
		
		/**
		 * Print the matrix
		 */
		void Print() const;


		// operators
		TMatrix<T>	operator +  (const TMatrix<T>& right) const;					
		TMatrix<T>	operator -  (const TMatrix<T>& right) const;					
		TMatrix<T>	operator *  (const TMatrix<T>& right) const;
		TMatrix<T>&	operator += (const TMatrix<T>& right);
		TMatrix<T>&	operator -= (const TMatrix<T>& right);
		TMatrix<T>&	operator *= (const TMatrix<T>& right);
		inline void    operator  = (const TMatrix<T>& right);

		// attributes
		union
		{		
    			T	m16[16];
   			T	m44[4][4];
		};
};


/**
 * Matrix typedef
 */
typedef /*__ALIGN(16)*/ TMatrix<double>	Matrix4d;
typedef /*__ALIGN(16)*/ TMatrix<float>	Matrix4f;
typedef /*__ALIGN(16)*/ TMatrix<float>	Matrix4;


#include "Matrix4.hpp"

#endif

