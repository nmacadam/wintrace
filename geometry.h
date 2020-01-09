#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

namespace lm
{

}

//
// Vectors
//

template <size_t DIM, typename T> struct vec {
	vec() { for (size_t i = DIM; i--; data_[i] = T()); }
	T& operator[](const size_t i) { assert(i < DIM); return data_[i]; }
	const T& operator[](const size_t i) const { assert(i < DIM); return data_[i]; }
	//vec<DIM, T>& operator+=(const vec<DIM, T>& rhs) { for (size_t i = DIM; i--; this[i] += rhs[i]) return this; }
	//vec<DIM, T>& operator-=(const vec<DIM, T>& rhs) { for (size_t i = DIM; i--; this[i] -= rhs[i]) return this; }
	//vec<DIM, T>& operator*=(const vec<DIM, T>& rhs) { for (size_t i = DIM; i--; this[i] *= rhs[i]) return this; }
private:
	T data_[DIM];
};

typedef vec<2, float> vec2;
typedef vec<2, int  > vec2i;
typedef vec<3, float> vec3;
typedef vec<3, int  > vec3i;
typedef vec<4, float> vec4;
typedef vec<4, int  > vec4i;

template <typename T> struct vec<2, T> {
	vec() : x(T()), y(T()) {}
	vec(T S) : x(S), y(S) {}
	vec(T X, T Y) : x(X), y(Y) {}
	//template <class U> vec<2, T>(const vec<2, U> &v);
	T& operator[](const size_t i) { assert(i < 2); return i <= 0 ? x : y; }
	const T& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? x : y; }
	T x, y;
};

template <typename T> struct vec<3, T> {
	vec() : x(T()), y(T()), z(T()) {}
	vec(T S) : x(S), y(S), z(S) {}
	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
	T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
	const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
	float norm() { return std::sqrt(x*x + y * y + z * z); }
	vec<3, T> & normalize(T l = 1) { *this = (*this)*(l / norm()); return *this; }
	T x, y, z;
};

template <typename T> struct vec<4, T> {
	vec() : x(T()), y(T()), z(T()), w(T()) {}
	vec(T S) : x(S), y(S), z(S), w(S) {}
	vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
	T& operator[](const size_t i) { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }
	const T& operator[](const size_t i) const { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }
	T x, y, z, w;
};

// Arithmetic operators

template<size_t DIM, typename T> T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
	T ret = T();
	for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
	return ret;
}

template<size_t DIM, typename T>vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
	for (size_t i = DIM; i--; lhs[i] += rhs[i]);
	return lhs;
}

template<size_t DIM, typename T>vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
	for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
	return lhs;
}

template<size_t DIM, typename T, typename U> vec<DIM, T> operator*(const vec<DIM, T> &lhs, const U& rhs) {
	vec<DIM, T> ret;
	for (size_t i = DIM; i--; ret[i] = lhs[i] * rhs);
	return ret;
}

template<size_t DIM, typename T> vec<DIM, T> operator-(const vec<DIM, T> &lhs) {
	return lhs * T(-1);
}

// Comparison operators
template<size_t DIM, typename T> bool operator==(const vec<DIM, T> &lhs, const vec<DIM, T> &rhs) {
	for (size_t i = DIM; i--;)
	{
		if (lhs[i] != rhs[i]) return false;
	}
	return true;
}

template<size_t DIM, typename T> bool operator!=(const vec<DIM, T> &lhs, const vec<DIM, T> &rhs) {
	for (size_t i = DIM; i--;)
	{
		if (lhs[i] != rhs[i]) return true;
	}
	return false;
}

// Output overloading

template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, const vec<DIM, T>& v) {
	for (unsigned int i = 0; i < DIM; i++) {
		out << v[i] << " ";
	}
	return out;
}

// Vector Operations

template<size_t DIM, typename T> float squareMagnitude(vec<DIM, T> v)
{
	float magSquared = 0;
	for (size_t i = DIM; i--;)
	{
		magSquared += v[i] * v[i];
	}

	return magSquared;
}

template<size_t DIM, typename T> float magnitude(vec<DIM, T> v)
{
	return sqrt(squareMagnitude(v));
}

template<size_t DIM, typename T> vec<DIM, T> normalize(vec<DIM, T> v)
{
	float length = magnitude(v);
	vec<DIM, T> ret;
	for (size_t i = DIM; i--; ret[i] = v[i] / length);
	return ret;
}

template<size_t DIM, typename T> float dot(vec<DIM, T> lhs, vec<DIM, T> rhs)
{
	float dotProduct = 0;
	for (size_t i = DIM; i--; dotProduct += lhs[i] * rhs[i]);
	return dotProduct;
}

template <typename T> vec<3, T> cross(vec<3, T> v1, vec<3, T> v2) {
	return vec<3, T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template<size_t DIM, typename T> vec<DIM, T> minimum(vec<DIM, T> lhs, vec<DIM, T> rhs) {
	vec<DIM, T> ret;
	for (size_t i = DIM; i--; ret[i] = min(lhs[i], rhs[i]));
	return ret;
}

template<size_t DIM, typename T> vec<DIM, T> maximum(vec<DIM, T> lhs, vec<DIM, T> rhs) {
	vec<DIM, T> ret;
	for (size_t i = DIM; i--; ret[i] = max(lhs[i], rhs[i]));
	return ret;
}

vec3 reflect(const vec3 &I, const vec3 &N) {
	return I - N * 2.f*(I*N);
}

//
// Matrices
//

template <size_t M, size_t N, typename T> struct mat {
	mat()
	{
		for (size_t i = M; i--;)
			for (size_t j = N; j--;) data_[i] = T();
	}

	vec<M, T>& operator[](const size_t i) { assert(i < N); return data_[i]; }
	const vec<M, T>& operator[](const size_t i) const { assert(i < N); return data_[i]; }
private:
	vec<M, T> data_[N];
};

typedef mat<2, 2, float> mat2;
typedef mat<2, 3, float> mat2x3;
typedef mat<3, 3, float> mat3;
typedef mat<4, 4, float> mat4;
//typedef vec<2, int  > vec2i;
//typedef vec<3, float> vec3;
//typedef vec<3, int  > vec3i;
//typedef vec<4, float> vec4;
//typedef vec<4, int  > vec4i;

template <typename T> struct mat<2, 2, T> {
	mat() : c0(T()), c1(T()) {}
	mat(T S) : c0(S), c1(S) {}
	mat(vec<2, T> C0, vec<2, T> C1) : c0(C0), c1(C1) {}
	vec<2, T>& operator[](const size_t i) { assert(i < 2); return i <= 0 ? c0 : c1; }
	const vec<2, T>& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? c0 : c1; }
	vec<2, T> c0, c1;
};

template <typename T> struct mat<2, 3, T> {
	mat() : c0(T()), c1(T()), c2(T()) {}
	mat(T S) : c0(S), c1(S), c2(S) {}
	mat(vec<2, T> C0, vec<2, T> C1, vec<2, T> C2) : c0(C0), c1(C1), c2(C2) {}
	vec<2, T>& operator[](const size_t i) { assert(i < 3); return i <= 0 ? c0 : (1 == i ? c1 : c2); }
	const vec<2, T>& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? c0 : (1 == i ? c1 : c2); }
	vec<2, T> c0, c1, c2;
};

template <typename T> struct mat<3, 3, T> {
	mat() : c0(T()), c1(T()), c2(T()) {}
	mat(T S) : c0(S), c1(S), c2(S) {}
	mat(vec<3, T> C0, vec<3, T> C1, vec<3, T> C2) : c0(C0), c1(C1), c2(C2) {}
	vec<3, T>& operator[](const size_t i) { assert(i < 3); return i <= 0 ? c0 : (1 == i ? c1 : c2); }
	const vec<2, T>& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? c0 : (1 == i ? c1 : c2); }
	vec<3, T> c0, c1, c2;
};

template <typename T> struct mat<4, 4, T> {
	mat() : c0(T()), c1(T()), c2(T()), c3(T()) {}
	mat(T S) : c0(S), c1(S), c2(S), c3(S) {}
	mat(vec<4, T> C0, vec<4, T> C1, vec<4, T> C2, vec<4, T> C3) : c0(C0), c1(C1), c2(C2), c3(C3) {}
	vec<4, T>& operator[](const size_t i) { assert(i < 4); return i <= 0 ? c0 : (1 == i ? c1 : (2 == i ? c2 : c3)); }
	const vec<4, T>& operator[](const size_t i) const { assert(i < 4); return i <= 0 ? c0 : (1 == i ? c1 : (2 == i ? c2 : c3)); }

	vec<3, T> c0, c1, c2, c3;
};



template <size_t M, size_t N, typename T> std::ostream& operator<<(std::ostream& out, const mat<M, N, T>& v) {
	for (unsigned int i = 0; i < M; i++) {
		for (unsigned int j = 0; j < N; j++) {
			out << v[j][i] << " ";
		}
		out << "\n";
	}
	return out;
}

//template <typename T> struct vec<2, T> {
//	vec() : x(T()), y(T()) {}
//	vec(T S) : x(S), y(S) {}
//	vec(T X, T Y) : x(X), y(Y) {}
//	//template <class U> vec<2, T>(const vec<2, U> &v);
//	T& operator[](const size_t i) { assert(i < 2); return i <= 0 ? x : y; }
//	const T& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? x : y; }
//	T x, y;
//};
//
//template <typename T> struct vec<3, T> {
//	vec() : x(T()), y(T()), z(T()) {}
//	vec(T S) : x(S), y(S), z(S) {}
//	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
//	T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
//	const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
//	float norm() { return std::sqrt(x*x + y * y + z * z); }
//	vec<3, T> & normalize(T l = 1) { *this = (*this)*(l / norm()); return *this; }
//	T x, y, z;
//};
//
//template <typename T> struct vec<4, T> {
//	vec() : x(T()), y(T()), z(T()), w(T()) {}
//	vec(T S) : x(S), y(S), z(S), w(S) {}
//	vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
//	T& operator[](const size_t i) { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }
//	const T& operator[](const size_t i) const { assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w)); }
//	T x, y, z, w;
//};

#endif //__GEOMETRY_H__