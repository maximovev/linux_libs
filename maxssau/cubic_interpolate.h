#ifndef __cubic__interpolation__
#define __cubic__interpolation__

#include <vector>
#include <stdexcept>
#include <algorithm>

namespace maxssau
{

	template <typename T>
	class CubicInterpolator {
	private:
		std::vector<T> x_values;  // Узлы интерполяции
		std::vector<T> y_values;  // Значения функции в узлах
		
		// Коэффициенты для кубических полиномов между узлами
		std::vector<std::array<T, 4>> coefficients;
		
		// Флаг, указывающий, были ли вычислены коэффициенты
		bool coefficients_calculated;
		
	public:
		CubicInterpolator() : coefficients_calculated(false) {}
		
		// Конструктор с передачей данных
		CubicInterpolator(const std::vector<T>& x, const std::vector<T>& y) 
			: x_values(x), y_values(y), coefficients_calculated(false) 
		{
			if (x.size() != y.size()) {
				throw std::invalid_argument("x and y arrays must have the same size");
			}
			if (x.size() < 2) {
				throw std::invalid_argument("At least 2 points are required for interpolation");
			}
		}
		
		// Добавление точки
		void addPoint(T x, T y) {
			// Находим позицию для вставки, чтобы сохранить порядок по x
			auto it = std::lower_bound(x_values.begin(), x_values.end(), x);
			size_t pos = it - x_values.begin();
			
			x_values.insert(it, x);
			y_values.insert(y_values.begin() + pos, y);
			
			coefficients_calculated = false;
		}
		
		// Установка всех точек
		void setPoints(const std::vector<T>& x, const std::vector<T>& y) {
			if (x.size() != y.size()) {
				throw std::invalid_argument("x and y arrays must have the same size");
			}
			if (x.size() < 2) {
				throw std::invalid_argument("At least 2 points are required for interpolation");
			}
			
			x_values = x;
			y_values = y;
			coefficients_calculated = false;
		}
		
		// Вычисление коэффициентов кубических полиномов
		void calculateCoefficients() {
			size_t n = x_values.size();
			coefficients.resize(n - 1);
			
			// Для простоты используем естественные граничные условия (вторая производная на концах = 0)
			// В реальных реализациях можно использовать другие граничные условия
			
			// Тридиагональная матрица для нахождения вторых производных
			std::vector<T> h(n - 1);
			std::vector<T> alpha(n - 1);
			std::vector<T> l(n);
			std::vector<T> mu(n);
			std::vector<T> z(n);
			
			// Вычисляем разности
			for (size_t i = 0; i < n - 1; ++i) {
				h[i] = x_values[i + 1] - x_values[i];
			}
			
			// Заполняем alpha
			for (size_t i = 1; i < n - 1; ++i) {
				alpha[i] = 3.0 / h[i] * (y_values[i + 1] - y_values[i]) - 
						3.0 / h[i - 1] * (y_values[i] - y_values[i - 1]);
			}
			
			// Прямой ход прогонки
			l[0] = 1.0;
			mu[0] = 0.0;
			z[0] = 0.0;
			
			for (size_t i = 1; i < n - 1; ++i) {
				l[i] = 2.0 * (x_values[i + 1] - x_values[i - 1]) - h[i - 1] * mu[i - 1];
				mu[i] = h[i] / l[i];
				z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
			}
			
			l[n - 1] = 1.0;
			z[n - 1] = 0.0;
			
			// Обратный ход прогонки
			std::vector<T> c(n);
			std::vector<T> b(n - 1);
			std::vector<T> d(n - 1);
			
			c[n - 1] = 0.0;
			
			for (int i = n - 2; i >= 0; --i) {
				c[i] = z[i] - mu[i] * c[i + 1];
				b[i] = (y_values[i + 1] - y_values[i]) / h[i] - h[i] * (c[i + 1] + 2.0 * c[i]) / 3.0;
				d[i] = (c[i + 1] - c[i]) / (3.0 * h[i]);
			}
			
			// Сохраняем коэффициенты
			for (size_t i = 0; i < n - 1; ++i) {
				coefficients[i] = {y_values[i], b[i], c[i], d[i]};
			}
			
			coefficients_calculated = true;
		}
		
		// Интерполяция значения в точке x
		T interpolate(T x) {
			if (!coefficients_calculated) {
				calculateCoefficients();
			}
			
			// Проверяем границы
			if (x < x_values.front() || x > x_values.back()) {
				throw std::out_of_range("x is out of interpolation range");
			}
			
			// Находим интервал, в который попадает x
			auto it = std::upper_bound(x_values.begin(), x_values.end(), x);
			size_t idx = std::distance(x_values.begin(), it) - 1;
			
			// Если x совпадает с последним узлом
			if (idx >= x_values.size() - 1) {
				idx = x_values.size() - 2;
			}
			
			// Вычисляем значение кубического полинома
			T dx = x - x_values[idx];
			const auto& coef = coefficients[idx];
			
			return coef[0] + coef[1] * dx + coef[2] * dx * dx + coef[3] * dx * dx * dx;
		}
		
		// Очистка данных
		void clear() {
			x_values.clear();
			y_values.clear();
			coefficients.clear();
			coefficients_calculated = false;
		}
	};

}

#endif