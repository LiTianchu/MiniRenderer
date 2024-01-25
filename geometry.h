#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <cassert>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t>
struct Vec2
{
	union
	{
		struct
		{
			t u, v;
		};
		struct
		{
			t x, y;
		};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }
	inline bool operator==(const Vec2<t> &V) const { return (u == V.u && v == V.v); }
	inline bool operator!=(const Vec2<t> &V) const { return (u != V.u || v != V.v); }
	inline bool operator<(const Vec2<t> &V) const { return (u < V.u && v < V.v); }
	inline bool operator>(const Vec2<t> &V) const { return (u > V.u && v > V.v); }
	template <class>
	friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
};

template <class t>
struct Vec3
{
	union
	{
		struct
		{
			t x, y, z;
		};
		struct
		{
			t ivert, iuv, inorm;
		};
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); } // cross product operator
	inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); } // dot product operator
	inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
	inline bool operator==(const Vec3<t> &v) const { return x == v.x && y == v.y && z == v.z; }
	inline bool operator!=(const Vec3<t> &v) const { return x != v.x || y != v.y || z != v.z; }
	inline bool operator<(const Vec3<t> &v) const { return x < v.x && y < v.y && z < v.z; }
	inline bool operator>(const Vec3<t> &v) const { return x > v.x && y > v.y && z > v.z; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<t> cross(Vec3<t> other) const { return Vec3<t>{y * other.z - z * other.y, -(x * other.z - z * other.x), x * other.y - y * other.x}; }
	Vec3<t> dot(Vec3<t> other) const { return Vec3<t>{x * other.x, y * other.y, z * other.z}; }
	Vec3<t> &normalize(t l = 1)
	{
		*this = (*this) * (l / norm());
		return *this;
	}
	Vec3<t> get_inverted() const { return Vec3<t>(-x, -y, -z); }
	template <class>
	friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template <class t>
std::ostream &operator<<(std::ostream &s, Vec2<t> &v)
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t>
std::ostream &operator<<(std::ostream &s, Vec3<t> &v)
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

const int DEFAULT_ALLOC = 4;

class Matrix
{
private:
	std::vector<std::vector<float>> m;
	int rows;
	int cols;

public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC) : m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))), rows(r), cols(c){};
	int nrows() { return rows; };
	inline int ncols() { return cols; };
	static Matrix identity(int dimensions)
	{ // creates an identity matrix of the given dimensions
		Matrix E(dimensions, dimensions);
		for (int i = 0; i < dimensions; i++)
		{
			for (int j = 0; j < dimensions; j++)
			{
				if (i == j)
				{
					E[i][j] = 1.f;
				}
			}
		}
		return E;
	};

	std::vector<float> &operator[](const int i)
	{
		assert(i >= 0 && i < rows);
		return m[i];
	};

	Matrix operator*(const Matrix &other)
	{ // matrix multiplication
		assert(cols == other.rows);
		Matrix result(rows, other.cols);
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < other.cols; j++)
			{
				result.m[i][j] = 0.f;
				for (int k = 0; k < cols; k++)
				{
					result.m[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}
		return result;
	};

	Matrix operator+(const Matrix &other){
		assert(cols == other.cols && rows == other.rows);
		Matrix result(rows, cols);
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				result.m[i][j] = m[i][j] + other.m[i][j];
			}
		}
		return result;
	}

	Matrix transpose() // returns the transpose of the matrix
	{
		Matrix result(cols, rows);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				result[j][i] = m[i][j];
		return result;
	};
	Matrix inverse()
	{
		assert(rows == cols);
		// augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
		Matrix result(rows, cols * 2);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				result[i][j] = m[i][j];
		for (int i = 0; i < rows; i++)
			result[i][i + cols] = 1;
		// first pass
		for (int i = 0; i < rows - 1; i++)
		{
			// normalize the first row
			for (int j = result.cols - 1; j >= 0; j--)
				result[i][j] /= result[i][i];
			for (int k = i + 1; k < rows; k++)
			{
				float coeff = result[k][i];
				for (int j = 0; j < result.cols; j++)
				{
					result[k][j] -= result[i][j] * coeff;
				}
			}
		}
		// normalize the last row
		for (int j = result.cols - 1; j >= rows - 1; j--)
			result[rows - 1][j] /= result[rows - 1][rows - 1];
		// second pass
		for (int i = rows - 1; i > 0; i--)
		{
			for (int k = i - 1; k >= 0; k--)
			{
				float coeff = result[k][i];
				for (int j = 0; j < result.cols; j++)
				{
					result[k][j] -= result[i][j] * coeff;
				}
			}
		}
		// cut the identity matrix back
		Matrix truncate(rows, cols);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				truncate[i][j] = result[i][j + cols];
		return truncate;
	}; // returns the inverse of the matrix

	// operator overloading for printing
	friend std::ostream &operator<<(std::ostream &s, Matrix &m)
	{
		for (int i = 0; i < m.nrows(); i++)
		{
			for (int j = 0; j < m.ncols(); j++)
			{
				s << m[i][j];
				if (j < m.ncols() - 1)
					s << "\t";
			}
			s << "\n";
		}
		return s;
	};

};

#endif //__GEOMETRY_H__
