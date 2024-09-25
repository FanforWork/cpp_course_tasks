#pragma once
#include <algorithm>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix(std::vector<std::vector<T>> matrix) { Matrix_ = matrix; }
  Matrix(T elem = T()) {
    Matrix_.resize(N);
    for (size_t i = 0; i < N; ++i) {
      Matrix_[i].resize(M);
    }
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        operator()(i, j) = elem;
      }
    }
  }

  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(j, i) = this->operator()(i, j);
      }
    }
    return result;
  }

  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& matrix) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        operator()(i, j) += matrix(i, j);
      }
    }
    return *this;
  }

  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& matrix) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        operator()(i, j) -= matrix(i, j);
      }
    }
    return *this;
  }

  bool operator==(const Matrix& matrix) const {
    return this->Matrix_ == matrix.Matrix_;
  }

  Matrix<N, M, T> operator*(T elem) {
    Matrix<N, M, T> result = *this;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result.operator()(i, j) *= elem;
      }
    }
    return result;
  }

  T operator()(size_t irow, size_t jcolumn) const {
    return Matrix_[irow][jcolumn];
  }

  T& operator()(size_t irow, size_t jcolumn) { return Matrix_[irow][jcolumn]; }

  Matrix<N, M, T> operator+(const Matrix<N, M, T>& matrix) const;
  Matrix<N, M, T> operator-(const Matrix<N, M, T>& matrix) const;
  template <size_t L>
  Matrix<N, L, T> operator*(const Matrix<M, L, T>& matrix) const;
  T Trace() const { return TraceSq(*this); }

 private:
  std::vector<std::vector<T>> Matrix_;
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(
    const Matrix<N, M, T>& matrix) const {
  Matrix<N, M, T> copy = *this;
  copy += matrix;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(
    const Matrix<N, M, T>& matrix) const {
  Matrix<N, M, T> copy = *this;
  copy -= matrix;
  return copy;
}

template <size_t N, size_t M, typename T>
template <size_t L>
Matrix<N, L, T> Matrix<N, M, T>::operator*(
    const Matrix<M, L, T>& matrix) const {
  Matrix<N, L, T> result;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < L; ++j) {
      result(i, j) = T();
      for (size_t k = 0; k < M; ++k) {
        result(i, j) += this->operator()(i, k) * matrix(k, j);
      }
    }
  }
  return result;
}

template <size_t N, typename T = int64_t>
T TraceSq(const Matrix<N, N, T>& matrix) {
  T sum;
  for (int i = N - 1; i >= 0; --i) {
    sum += matrix(i, i);
  }
  return sum;
}