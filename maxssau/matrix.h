#ifndef __matrix__
#define __matrix__

#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace maxssau
{

	template <typename T>
	class Matrix {
	private:
		std::vector<std::vector<T>> data;
		size_t rows;
		size_t cols;

	public:
		// Конструкторы
		Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
			if (rows == 0 || cols == 0) {
				throw std::invalid_argument("Matrix dimensions cannot be zero");
			}
			data.resize(rows, std::vector<T>(cols, T()));
		}

		Matrix(size_t rows, size_t cols, const T& init_value) : rows(rows), cols(cols) {
			if (rows == 0 || cols == 0) {
				throw std::invalid_argument("Matrix dimensions cannot be zero");
			}
			data.resize(rows, std::vector<T>(cols, init_value));
		}

		Matrix(const std::vector<std::vector<T>>& input) {
			if (input.empty() || input[0].empty()) {
				throw std::invalid_argument("Matrix cannot be empty");
			}
			rows = input.size();
			cols = input[0].size();
			for (const auto& row : input) {
				if (row.size() != cols) {
					throw std::invalid_argument("All rows must have the same size");
				}
			}
			data = input;
		}

		// Копирующий конструктор
		Matrix(const Matrix& other) : data(other.data), rows(other.rows), cols(other.cols) {}

		// Оператор присваивания
		Matrix& operator=(const Matrix& other) {
			if (this != &other) {
				data = other.data;
				rows = other.rows;
				cols = other.cols;
			}
			return *this;
		}

		// Доступ к элементам
		T& operator()(size_t row, size_t col) {
			if (row >= rows || col >= cols) {
				throw std::out_of_range("Matrix indices out of range");
			}
			return data[row][col];
		}

		const T& operator()(size_t row, size_t col) const {
			if (row >= rows || col >= cols) {
				throw std::out_of_range("Matrix indices out of range");
			}
			return data[row][col];
		}

		// Размеры матрицы
		size_t getRows() const { return rows; }
		size_t getCols() const { return cols; }

		// Основные операции
		Matrix operator+(const Matrix& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrices must have the same dimensions for addition");
			}
			Matrix result(rows, cols);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] + other(i, j);
				}
			}
			return result;
		}

		Matrix operator-(const Matrix& other) const {
			if (rows != other.rows || cols != other.cols) {
				throw std::invalid_argument("Matrices must have the same dimensions for subtraction");
			}
			Matrix result(rows, cols);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] - other(i, j);
				}
			}
			return result;
		}

		Matrix operator*(const Matrix& other) const {
			if (cols != other.rows) {
				throw std::invalid_argument("Number of columns in first matrix must equal number of rows in second matrix");
			}
			Matrix result(rows, other.cols, T());
			for (size_t i = 0; i < rows; ++i) {
				for (size_t k = 0; k < cols; ++k) {
					for (size_t j = 0; j < other.cols; ++j) {
						result(i, j) += data[i][k] * other(k, j);
					}
				}
			}
			return result;
		}

		Matrix operator*(const T& scalar) const {
			Matrix result(rows, cols);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					result(i, j) = data[i][j] * scalar;
				}
			}
			return result;
		}

		// Транспонирование
		Matrix transpose() const {
			Matrix result(cols, rows);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					result(j, i) = data[i][j];
				}
			}
			return result;
		}

		// Определитель (только для квадратных матриц)
		T determinant() const {
			if (rows != cols) {
				throw std::logic_error("Determinant can be calculated only for square matrices");
			}
			if (rows == 1) return data[0][0];
			if (rows == 2) return data[0][0] * data[1][1] - data[0][1] * data[1][0];

			T det = T();
			for (size_t i = 0; i < cols; ++i) {
				Matrix minor = getMinor(0, i);
				det += (i % 2 == 0 ? 1 : -1) * data[0][i] * minor.determinant();
			}
			return det;
		}

		// Обратная матрица (только для квадратных матриц с ненулевым определителем)
		Matrix inverse() const {
			if (rows != cols) {
				throw std::logic_error("Inverse can be calculated only for square matrices");
			}
			T det = determinant();
			if (det == T()) {
				throw std::logic_error("Matrix is singular (determinant is zero)");
			}

			if (rows == 1) {
				Matrix result(1, 1);
				result(0, 0) = T(1) / data[0][0];
				return result;
			}

			Matrix cofactors(rows, cols);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					Matrix minor = getMinor(i, j);
					cofactors(i, j) = ((i + j) % 2 == 0 ? 1 : -1) * minor.determinant();
				}
			}

			Matrix adjugate = cofactors.transpose();
			return adjugate * (T(1) / det);
		}

		// Минор матрицы
		Matrix getMinor(size_t row, size_t col) const {
			if (row >= rows || col >= cols) {
				throw std::out_of_range("Indices out of range");
			}
			Matrix minor(rows - 1, cols - 1);
			for (size_t i = 0, mi = 0; i < rows; ++i) {
				if (i == row) continue;
				for (size_t j = 0, mj = 0; j < cols; ++j) {
					if (j == col) continue;
					minor(mi, mj) = data[i][j];
					++mj;
				}
				++mi;
			}
			return minor;
		}

		// Норма матрицы (Фробениусова норма)
		T norm() const {
			T sum = T();
			for (const auto& row : data) {
				for (const auto& elem : row) {
					sum += elem * elem;
				}
			}
			return std::sqrt(sum);
		}

		// Вывод матрицы
		void print(std::ostream& os = std::cout) const {
			for (const auto& row : data) {
				for (const auto& elem : row) {
					os << elem << "\t";
				}
				os << "\n";
			}
		}

		// Единичная матрица (статический метод)
		static Matrix identity(size_t size) {
			Matrix result(size, size, T());
			for (size_t i = 0; i < size; ++i) {
				result(i, i) = T(1);
			}
			return result;
		}
	};

	// Оператор умножения скаляра на матрицу (чтобы можно было писать 2 * matrix)
	template <typename T>
	Matrix<T> operator*(const T& scalar, const Matrix<T>& matrix) {
		return matrix * scalar;
	}

	// Оператор вывода матрицы в поток
	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
		matrix.print(os);
		return os;
	}

}

#endif