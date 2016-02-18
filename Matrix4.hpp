/*
 * rover
 */


template <class T> inline TMatrix<T>::TMatrix(const TMatrix<T>& m)
{
	memcpy(m16, m.m16, sizeof(TMatrix<T>));
}


template <class T> TMatrix<T> TMatrix<T>::operator + (const TMatrix<T>& right) const
{
	TMatrix<T> r;

	for (uint i=0; i<4; ++i)
	{
		r.m44[i][0] = m44[i][0] + right.m44[i][0];
		r.m44[i][1] = m44[i][1] + right.m44[i][1];
		r.m44[i][2] = m44[i][2] + right.m44[i][2];
		r.m44[i][3] = m44[i][3] + right.m44[i][3];
	}

	return r;
}


template <class T> TMatrix<T> TMatrix<T>::operator - (const TMatrix<T>& right) const
{
	TMatrix<T> r;

	for (uint i=0; i<4; ++i)
	{
		r.m44[i][0] = m44[i][0] - right.m44[i][0];
		r.m44[i][1] = m44[i][1] - right.m44[i][1];
		r.m44[i][2] = m44[i][2] - right.m44[i][2];
		r.m44[i][3] = m44[i][3] - right.m44[i][3];
	}

	return r;
}


template <class T> TMatrix<T> TMatrix<T>::operator * (const TMatrix<T>& right) const
{
	TMatrix<T> r;

	const T* v  = m16;
	const T* rm = right.m16;

	r.m44[0][0] = v[0]*rm[0] + v[1]*rm[4] + v[2]*rm[8]  + v[3]*rm[12];
	r.m44[0][1] = v[0]*rm[1] + v[1]*rm[5] + v[2]*rm[9]  + v[3]*rm[13];
	r.m44[0][2] = v[0]*rm[2] + v[1]*rm[6] + v[2]*rm[10] + v[3]*rm[14];
	r.m44[0][3] = v[0]*rm[3] + v[1]*rm[7] + v[2]*rm[11] + v[3]*rm[15];

	r.m44[1][0] = v[4]*rm[0] + v[5]*rm[4] + v[6]*rm[8]  + v[7]*rm[12];
	r.m44[1][1] = v[4]*rm[1] + v[5]*rm[5] + v[6]*rm[9]  + v[7]*rm[13];
	r.m44[1][2] = v[4]*rm[2] + v[5]*rm[6] + v[6]*rm[10] + v[7]*rm[14];
	r.m44[1][3] = v[4]*rm[3] + v[5]*rm[7] + v[6]*rm[11] + v[7]*rm[15];

	r.m44[2][0] = v[8]*rm[0] + v[9]*rm[4] + v[10]*rm[8]  + v[11]*rm[12];
	r.m44[2][1] = v[8]*rm[1] + v[9]*rm[5] + v[10]*rm[9]  + v[11]*rm[13];
	r.m44[2][2] = v[8]*rm[2] + v[9]*rm[6] + v[10]*rm[10] + v[11]*rm[14];
	r.m44[2][3] = v[8]*rm[3] + v[9]*rm[7] + v[10]*rm[11] + v[11]*rm[15];

	r.m44[3][0] = v[12]*rm[0] + v[13]*rm[4] + v[14]*rm[8]  + v[15]*rm[12];
	r.m44[3][1] = v[12]*rm[1] + v[13]*rm[5] + v[14]*rm[9]  + v[15]*rm[13];
	r.m44[3][2] = v[12]*rm[2] + v[13]*rm[6] + v[14]*rm[10] + v[15]*rm[14];
	r.m44[3][3] = v[12]*rm[3] + v[13]*rm[7] + v[14]*rm[11] + v[15]*rm[15];

	return r;
}


template <class T> TMatrix<T>& TMatrix<T>::operator += (const TMatrix<T>& right)
{
	for (uint i=0; i<4; ++i)
	{
		m44[i][0] += right.m44[i][0];
		m44[i][1] += right.m44[i][1];
		m44[i][2] += right.m44[i][2];
		m44[i][3] += right.m44[i][3];
	}
	return *this;
}


template <class T> TMatrix<T>& TMatrix<T>::operator -= (const TMatrix<T>& right)
{
	for (uint i=0; i<4; ++i)
	{
		m44[i][0] -= right.m44[i][0];
		m44[i][1] -= right.m44[i][1];
		m44[i][2] -= right.m44[i][2];
		m44[i][3] -= right.m44[i][3];
	}
	return *this;
}


template <class T> TMatrix<T>& TMatrix<T>::operator *= (const TMatrix<T>& right)
{
	T v[4];
	for (uint i=0; i<4; ++i)
	{
		v[0] = m44[i][0];
		v[1] = m44[i][1];
		v[2] = m44[i][2];
		v[3] = m44[i][3];
		m44[i][0] = v[0]*right.m44[0][0] + v[1]*right.m44[1][0] + v[2]*right.m44[2][0] + v[3]*right.m44[3][0];
		m44[i][1] = v[0]*right.m44[0][1] + v[1]*right.m44[1][1] + v[2]*right.m44[2][1] + v[3]*right.m44[3][1];
		m44[i][2] = v[0]*right.m44[0][2] + v[1]*right.m44[1][2] + v[2]*right.m44[2][2] + v[3]*right.m44[3][2];
		m44[i][3] = v[0]*right.m44[0][3] + v[1]*right.m44[1][3] + v[2]*right.m44[2][3] + v[3]*right.m44[3][3];
	}
	
	return *this;
}


template <class T> inline void TMatrix<T>::operator = (const TMatrix<T>& right)
{
	memcpy(m16, right.m16, sizeof(TMatrix<T>));
}


template <class T> void TMatrix<T>::Identity()
{
	m44[0][0] = 1; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
	m44[1][0] = 0; m44[1][1] = 1; m44[1][2] = 0; m44[1][3] = 0;
	m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = 1; m44[2][3] = 0;
	m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
}


template <class T> void TMatrix<T>::SetRotationMatrixX(const T angle)
{
	const T s = Sin( angle );
	const T c = Cos( angle );
	
	m44[0][0] = 1; m44[0][1] = 0; m44[0][2] = 0; m44[0][3] = 0;
	m44[1][0] = 0; m44[1][1] = c; m44[1][2] = s; m44[1][3] = 0;
	m44[2][0] = 0; m44[2][1] =-s; m44[2][2] = c; m44[2][3] = 0;
	m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
}


template <class T> void TMatrix<T>::SetRotationMatrixY(const T angle)
{
	const T s = Sin( angle );
	const T c = Cos( angle );
	
	m44[0][0] = c; m44[0][1] = 0; m44[0][2] =-s; m44[0][3] = 0;
	m44[1][0] = 0; m44[1][1] = 1; m44[1][2] = 0; m44[1][3] = 0;
	m44[2][0] = s; m44[2][1] = 0; m44[2][2] = c; m44[2][3] = 0;
	m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
}


template <class T> void TMatrix<T>::SetRotationMatrixZ(const T angle)
{
	const T s = Sin( angle );
	const T c = Cos( angle );

	m44[0][0] = c; m44[0][1] = s; m44[0][2] = 0; m44[0][3] = 0;
	m44[1][0] =-s; m44[1][1] = c; m44[1][2] = 0; m44[1][3] = 0;
	m44[2][0] = 0; m44[2][1] = 0; m44[2][2] = 1; m44[2][3] = 0;
	m44[3][0] = 0; m44[3][1] = 0; m44[3][2] = 0; m44[3][3] = 1;
}


template <class T> void TMatrix<T>::MultMatrix(const TMatrix<T>& right)
{	
	T v[4];

	for (uint i=0; i<4; ++i)
	{
		v[0] = m44[i][0];
		v[1] = m44[i][1];
		v[2] = m44[i][2];
		v[3] = m44[i][3];
		m44[i][0] = v[0]*right.m44[0][0] + v[1]*right.m44[1][0] + v[2]*right.m44[2][0] + v[3]*right.m44[3][0];
		m44[i][1] = v[0]*right.m44[0][1] + v[1]*right.m44[1][1] + v[2]*right.m44[2][1] + v[3]*right.m44[3][1];
		m44[i][2] = v[0]*right.m44[0][2] + v[1]*right.m44[1][2] + v[2]*right.m44[2][2] + v[3]*right.m44[3][2];
		m44[i][3] = v[0]*right.m44[0][3] + v[1]*right.m44[1][3] + v[2]*right.m44[2][3] + v[3]*right.m44[3][3];
	}
}


template <class T> void TMatrix<T>::MultMatrix3x3(const TMatrix<T>& right)
{
	T v[3];
	for (uint i=0; i<4; ++i)
	{
		v[0] = m44[i][0];
		v[1] = m44[i][1];
		v[2] = m44[i][2];
		m44[i][0] = v[0]*right.m44[0][0] + v[1]*right.m44[1][0] + v[2]*right.m44[2][0];
		m44[i][1] = v[0]*right.m44[0][1] + v[1]*right.m44[1][1] + v[2]*right.m44[2][1];
		m44[i][2] = v[0]*right.m44[0][2] + v[1]*right.m44[1][2] + v[2]*right.m44[2][2];
	}
}


// *this = left * right
template <class T> void TMatrix<T>::MultMatrix(const TMatrix<T>& left, const TMatrix<T>& right)
{
	T v[4];
	for (uint i=0; i<4; ++i)
	{
		v[0] = left.m44[i][0];
		v[1] = left.m44[i][1];
		v[2] = left.m44[i][2];
		v[3] = left.m44[i][3];
		m44[i][0] = v[0]*right.m44[0][0] + v[1]*right.m44[1][0] + v[2]*right.m44[2][0] + v[3]*right.m44[3][0];
		m44[i][1] = v[0]*right.m44[0][1] + v[1]*right.m44[1][1] + v[2]*right.m44[2][1] + v[3]*right.m44[3][1];
		m44[i][2] = v[0]*right.m44[0][2] + v[1]*right.m44[1][2] + v[2]*right.m44[2][2] + v[3]*right.m44[3][2];
		m44[i][3] = v[0]*right.m44[0][3] + v[1]*right.m44[1][3] + v[2]*right.m44[2][3] + v[3]*right.m44[3][3];
	}
}


template <class T> void TMatrix<T>::Transpose()
{
	TMatrix<T> v;

	v.m44[0][0] = m44[0][0];
	v.m44[0][1] = m44[1][0];
	v.m44[0][2] = m44[2][0];
	v.m44[0][3] = m44[3][0];
	v.m44[1][0] = m44[0][1];
	v.m44[1][1] = m44[1][1];
	v.m44[1][2] = m44[2][1];
	v.m44[1][3] = m44[3][1];
	v.m44[2][0] = m44[0][2];
	v.m44[2][1] = m44[1][2];
	v.m44[2][2] = m44[2][2];
	v.m44[2][3] = m44[3][2];
	v.m44[3][0] = m44[0][3];
	v.m44[3][1] = m44[1][3];
	v.m44[3][2] = m44[2][3];
	v.m44[3][3] = m44[3][3];

	*this = v;
}


template <class T> inline void TMatrix<T>::Mul3x3(T* v, T* o) const
{
	o[0] = v[0]*m44[0][0] + v[1]*m44[1][0] + v[2]*m44[2][0];
	o[1] = v[0]*m44[0][1] + v[1]*m44[1][1] + v[2]*m44[2][1];
	o[2] = v[0]*m44[0][2] + v[1]*m44[1][2] + v[2]*m44[2][2];
}



template <class T> void TMatrix<T>::Print() const
{
	printf("");
	printf("(%.8f, %.8f, %.8f, %.8f)", m44[0][0], m44[0][1], m44[0][2], m44[0][3]);
	printf("(%.8f, %.8f, %.8f, %.8f)", m44[1][0], m44[1][1], m44[1][2], m44[1][3]);
	printf("(%.8f, %.8f, %.8f, %.8f)", m44[2][0], m44[2][1], m44[2][2], m44[2][3]);
	printf("(%.8f, %.8f, %.8f, %.8f)", m44[3][0], m44[3][1], m44[3][2], m44[3][3]);
	printf("");
}


