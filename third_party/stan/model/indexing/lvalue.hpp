#ifndef STAN_MODEL_INDEXING_LVALUE_HPP
#define STAN_MODEL_INDEXING_LVALUE_HPP

#include <stan/math/prim.hpp>
#include <stan/model/indexing/index.hpp>
#include <stan/model/indexing/index_list.hpp>
#include <stan/model/indexing/rvalue_at.hpp>
#include <stan/model/indexing/rvalue_index_size.hpp>
#include <type_traits>
#include <vector>

namespace stan {

namespace model {

/**
 * Indexing Notes:
 * The different index types:
 * index_uni - A single cell.
 * index_multi - Access multiple cells.
 * index_omni - A no-op for all indices along a dimension.
 * index_min - index from min:N
 * index_max - index from 1:max
 * index_min_max - index from min:max
 * nil_index_list - no-op
 * The order of the overloads are
 * vector / row_vector:
 *  - all index overloads
 * matrix:
 *  - all row index overloads
 *    - Assigns a subset of rows.
 *  - column/row overloads
 *    - overload on both the row and column indices.
 *  - column overloads
 *    - These take a subset of columns and then call the row slice assignment
 *       over the column subset.
 * Std vector:
 *  - single element and elementwise overloads
 *  - General overload for nested std vectors.
 */

/**
 * Assign one object to another.
 *
 * @tparam T lvalue variable type
 * @tparam U rvalue variable type, which must be assignable to `T`
 * @param[in,out] x lvalue
 * @param[in] y rvalue
 * @param[in] name Name of lvalue variable (default "ANON"); ignored
 * @param[in] depth Indexing depth (default 0; ignored
 */
template <
    typename T, typename U,
    require_t<std::is_assignable<std::decay_t<T>&, std::decay_t<U>>>* = nullptr>
inline void assign(T&& x, const nil_index_list& /* idxs */, U&& y,
                   const char* name = "ANON", int depth = 0) {
  x = std::forward<U>(y);
}

/**
 * Assign to a single element of an Eigen Vector.
 *
 * Types: vector[uni] <- scalar
 *
 * @tparam Vec Eigen type with either dynamic rows or columns, but not both.
 * @tparam U Type of value (must be assignable to T).
 * @param[in] x Vector variable to be assigned.
 * @param[in] idxs index holding which cell to assign to.
 * @param[in] y Value to assign.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If the index is out of bounds.
 */
template <typename Vec, typename U, require_eigen_vector_t<Vec>* = nullptr,
          require_stan_scalar_t<U>* = nullptr>
inline void assign(Vec&& x,
                   const cons_index_list<index_uni, nil_index_list>& idxs,
                   const U& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[uni] assign range", name, x.size(),
                          idxs.head_.n_);
  x.coeffRef(idxs.head_.n_ - 1) = y;
}

/**
 * Assign to a non-contiguous subset of elements in a vector.
 *
 * Types:  vector[multi] <- vector
 *
 * @tparam Vec1 Eigen type with either dynamic rows or columns, but not both.
 * @tparam Vec2 Eigen type with either dynamic rows or columns, but not both.
 * @param[in] x Vector to be assigned.
 * @param[in] idxs Index holding an `std::vector` of cells to assign to.
 * @param[in] y Value vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the value size isn't the same as
 * the indexed size.
 */
template <typename Vec1, typename Vec2,
          require_all_eigen_vector_t<Vec1, Vec2>* = nullptr>
inline void assign(Vec1&& x,
                   const cons_index_list<index_multi, nil_index_list>& idxs,
                   const Vec2& y, const char* name = "ANON", int depth = 0) {
  const auto& y_ref = stan::math::to_ref(y);
  stan::math::check_size_match("vector[multi] assign sizes", "lhs",
                               idxs.head_.ns_.size(), name, y_ref.size());
  const auto x_size = x.size();
  for (int n = 0; n < y_ref.size(); ++n) {
    stan::math::check_range("vector[multi] assign range", name, x_size,
                            idxs.head_.ns_[n]);
    x.coeffRef(idxs.head_.ns_[n] - 1) = y_ref.coeff(n);
  }
}

/**
 * Assign to a range of an Eigen vector
 *
 * Types:  vector[min_max] <- vector
 *
 * @tparam Vec1 Eigen type with either dynamic rows or columns, but not both.
 * @tparam Vec2 Eigen type with either dynamic rows or columns, but not both.
 * @param[in] x vector variable to be assigned.
 * @param[in] idxs List holding a single `index_min_max`.
 * @param[in] y Value vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the value size isn't the same as
 * the indexed size.
 */
template <typename Vec1, typename Vec2,
          require_all_eigen_vector_t<Vec1, Vec2>* = nullptr>
inline void assign(Vec1&& x,
                   const cons_index_list<index_min_max, nil_index_list>& idxs,
                   const Vec2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[min_max] min assign", name, x.size(),
                          idxs.head_.min_);
  stan::math::check_range("vector[min_max] max assign", name, x.size(),
                          idxs.head_.max_);
  if (idxs.head_.is_ascending()) {
    const auto slice_start = idxs.head_.min_ - 1;
    const auto slice_size = idxs.head_.max_ - slice_start;
    stan::math::check_size_match("vector[min_max] assign sizes", "lhs and rhs",
                                 slice_size, name, y.size());
    x.segment(slice_start, slice_size) = y;
    return;
  } else {
    const auto slice_start = idxs.head_.max_ - 1;
    const auto slice_size = idxs.head_.min_ - slice_start;
    stan::math::check_size_match("vector[reverse_min_max] assign sizes",
                                 "lhs and rhs", slice_size, name, y.size());
    x.segment(slice_start, slice_size) = y.reverse();
    return;
  }
}

/**
 * Assign to a tail slice of a vector
 *
 * Types:  vector[min:N] <- vector
 *
 * @tparam Vec1 Eigen type with either dynamic rows or columns, but not both.
 * @tparam Vec2 Eigen type with either dynamic rows or columns, but not both.
 * @param[in] x vector to be assigned to.
 * @param[in] idxs An index.
 * @param[in] y Value vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the value size isn't the same as
 * the indexed size.
 */
template <typename Vec1, typename Vec2,
          require_all_eigen_vector_t<Vec1, Vec2>* = nullptr>
inline void assign(Vec1&& x,
                   const cons_index_list<index_min, nil_index_list>& idxs,
                   const Vec2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[min] assign range", name, x.size(),
                          idxs.head_.min_);
  stan::math::check_size_match("vector[min] assign sizes", "lhs",
                               x.size() - idxs.head_.min_ + 1, name, y.size());
  x.tail(x.size() - idxs.head_.min_ + 1) = y;
}

/**
 * Assign to a head slice of the assignee vector
 *
 * Types:  vector[1:max] <- vector
 *
 * @tparam Vec1 Eigen type with either dynamic rows or columns, but not both.
 * @tparam Vec2 Eigen type with either dynamic rows or columns, but not both.
 * @param[in] x vector to be assigned to.
 * @param[in] idxs An index.
 * @param[in] y Value vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the value size isn't the same as
 * the indexed size.
 */
template <typename Vec1, typename Vec2,
          require_all_eigen_vector_t<Vec1, Vec2>* = nullptr>
inline void assign(Vec1&& x,
                   const cons_index_list<index_max, nil_index_list>& idxs,
                   const Vec2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[min] assign range", name, x.size(),
                          idxs.head_.max_);
  stan::math::check_size_match("vector[min] assign sizes", "lhs",
                               idxs.head_.max_, name, y.size());
  x.head(idxs.head_.max_) = y;
}

/**
 * Assign a vector to another vector.
 *
 * Types:  vector[omni] <- vector
 *
 * @tparam Vec1 Eigen type with either dynamic rows or columns, but not both.
 * @tparam Vec2 Eigen type with either dynamic rows or columns, but not both.
 * @param[in] x vector to be assigned to.
 * @param[in] idxs An index.
 * @param[in] y Value vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::invalid_argument If the value size isn't the same as
 * the indexed size.
 */
template <typename Vec1, typename Vec2,
          require_all_eigen_vector_t<Vec1, Vec2>* = nullptr>
inline void assign(Vec1&& x,
                   const cons_index_list<index_omni, nil_index_list>& idxs,
                   Vec2&& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_size_match("vector[min] assign sizes", "lhs", x.size(),
                               name, y.size());
  x = std::forward<Vec2>(y);
}

/**
 * Assign a row vector to a row of an eigen matrix.
 *
 * Types:  mat[uni] = row_vector
 *
 * @tparam Mat Eigen type with dynamic rows and columns.
 * @tparam RowVec Eigen type with dynamic columns and a compile time rows equal
 * to 1.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs An index holding the row to be assigned to.
 * @param[in] y Value row vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the number of columns in the row
 * vector and matrix do not match.
 */
template <typename Mat, typename RowVec,
          stan::internal::require_eigen_dense_dynamic_t<Mat>* = nullptr,
          require_eigen_row_vector_t<RowVec>* = nullptr>
inline void assign(Mat&& x,
                   const cons_index_list<index_uni, nil_index_list>& idxs,
                   const RowVec& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_size_match("matrix[uni] assign sizes", "lhs", x.cols(),
                               name, y.size());
  stan::math::check_range("matrix[uni] assign range", name, x.rows(),
                          idxs.head_.n_);
  x.row(idxs.head_.n_ - 1) = y;
}

/**
 * Assign to a non-contiguous subset of a matrice's rows.
 *
 * Types:  mat[multi] = mat
 *
 * @tparam Mat Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs List holding a multi index.
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename EigMat1, typename EigMat2,
          stan::internal::require_all_eigen_dense_dynamic_t<EigMat1,
                                                            EigMat2>* = nullptr>
inline void assign(EigMat1&& x,
                   const cons_index_list<index_multi, nil_index_list>& idxs,
                   const EigMat2& y, const char* name = "ANON", int depth = 0) {
  const auto& y_ref = stan::math::to_ref(y);
  stan::math::check_size_match("matrix[multi] assign row sizes", "lhs",
                               idxs.head_.ns_.size(), name, y.rows());
  for (int i = 0; i < idxs.head_.ns_.size(); ++i) {
    const int n = idxs.head_.ns_[i];
    stan::math::check_range("matrix[multi] subset range", name, x.rows(), n);
    x.row(n - 1) = y_ref.row(i);
  }
}

/**
 * Assign a matrix to another matrix
 *
 * Types:  mat[omni] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs List holding an omni index.
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <
    typename Mat1, typename Mat2,
    stan::internal::require_all_eigen_dense_dynamic_t<Mat1, Mat2>* = nullptr>
inline void assign(Mat1&& x,
                   const cons_index_list<index_omni, nil_index_list>& idxs,
                   Mat2&& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_size_match("matrix[max] assign row sizes", "lhs", x.rows(),
                               name, y.rows());
  stan::math::check_size_match("matrix[max] assign column sizes", "lhs",
                               x.cols(), name, y.cols());
  x = std::forward<Mat2>(y);
}

/**
 * Assign a matrix to the bottom rows of the assignee matrix.
 *
 * Types:  mat[min] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs An indexing from a minimum index (inclusive) to
 * the end of a container.
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <
    typename Mat1, typename Mat2,
    stan::internal::require_all_eigen_dense_dynamic_t<Mat1, Mat2>* = nullptr,
    require_eigen_t<Mat2>* = nullptr>
inline void assign(Mat1&& x,
                   const cons_index_list<index_min, nil_index_list>& idxs,
                   const Mat2& y, const char* name = "ANON", int depth = 0) {
  const auto row_size = x.rows() - (idxs.head_.min_ - 1);
  stan::math::check_range("matrix[min] assign range", name, x.rows(), row_size);
  stan::math::check_size_match("matrix[min] assign row sizes", "lhs", row_size,
                               name, y.rows());
  stan::math::check_size_match("matrix[min] assign col sizes", "lhs", x.cols(),
                               name, y.cols());
  x.bottomRows(row_size) = y;
}

/**
 * Assign a matrix to the top rows of the assignee matrix.
 *
 * Types:  mat[max] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs An indexing from the start of the container up to
 * the specified maximum index (inclusive).
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <
    typename Mat1, typename Mat2,
    stan::internal::require_all_eigen_dense_dynamic_t<Mat1, Mat2>* = nullptr>
inline void assign(Mat1&& x,
                   const cons_index_list<index_max, nil_index_list>& idxs,
                   const Mat2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("matrix[max] assign range", name, x.cols(),
                          idxs.head_.max_);
  stan::math::check_size_match("matrix[max] assign row sizes", "lhs",
                               idxs.head_.max_, name, y.rows());
  x.topRows(idxs.head_.max_) = y;
}

/**
 * Assign a matrix to a range of rows of the assignee matrix.
 *
 * Types:  mat[min_max] = mat
 *
 * @tparam Mat Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs An index for a min_max range of rows
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename EigMat1, typename EigMat2,
          stan::internal::require_all_eigen_dense_dynamic_t<EigMat1,
                                                            EigMat2>* = nullptr>
inline void assign(EigMat1&& x,
                   const cons_index_list<index_min_max, nil_index_list>& idxs,
                   const EigMat2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("matrix[min_max] max row indexing", name, x.rows(),
                          idxs.head_.max_);
  stan::math::check_range("matrix[min_max] min row indexing", name, x.rows(),
                          idxs.head_.min_);
  if (idxs.head_.is_ascending()) {
    stan::math::check_size_match("matrix[min_max] assign row sizes", "lhs",
                                 idxs.head_.min_, name, y.rows());
    x.middleRows(idxs.head_.min_ - 1, idxs.head_.max_ - 1) = y;
    return;
  } else {
    stan::math::check_size_match("matrix[reverse_min_max] assign row sizes",
                                 "lhs", idxs.head_.max_, name, y.rows());
    x.middleRows(idxs.head_.max_ - 1, idxs.head_.min_ - 1)
        = y.colwise().reverse();
    return;
  }
}

/**
 * Assign to a block of an Eigen matrix.
 *
 * Types:  mat[min_max, min_max] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs An index list containing two min_max indices
 * @param[in] y Matrix variable to assign from.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename Mat1, typename Mat2,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr,
          require_eigen_t<Mat2>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<index_min_max,
                          cons_index_list<index_min_max, nil_index_list>>& idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  if (idxs.head_.is_ascending()) {
    if (idxs.tail_.head_.is_ascending()) {
      auto row_size = idxs.head_.max_ - (idxs.head_.min_ - 1);
      auto col_size = idxs.tail_.head_.max_ - (idxs.tail_.head_.min_ - 1);
      stan::math::check_range("matrix[min_max, min_max] assign col range", name,
                              x.cols(), idxs.head_.min_);
      stan::math::check_range("matrix[min_max, min_max] assign row range", name,
                              x.rows(), idxs.tail_.head_.min_);
      stan::math::check_size_match("matrix[min_max, min_max] assign row sizes",
                                   "lhs", row_size, name, y.rows());
      stan::math::check_size_match("matrix[min_max, min_max] assign col sizes",
                                   "lhs", col_size, name, y.cols());
      x.block(idxs.head_.min_ - 1, idxs.tail_.head_.min_ - 1, row_size,
              col_size)
          = y;
      return;
    } else {
      auto row_size = idxs.head_.max_ - (idxs.head_.min_ - 1);
      auto col_size = idxs.tail_.head_.min_ - (idxs.tail_.head_.max_ - 1);
      stan::math::check_range(
          "matrix[min_max, reverse_min_max] assign col range", name, x.cols(),
          idxs.head_.min_);
      stan::math::check_range(
          "matrix[min_max, reverse_min_max] assign row range", name, x.rows(),
          idxs.tail_.head_.max_);
      stan::math::check_size_match(
          "matrix[min_max, reverse_min_max] assign row sizes", "lhs", row_size,
          name, y.rows());
      stan::math::check_size_match(
          "matrix[min_max, reverse_min_max] assign col sizes", "lhs", col_size,
          name, y.cols());
      x.block(idxs.head_.min_ - 1, idxs.tail_.head_.max_ - 1, row_size,
              col_size)
          = y.rowwise().reverse();
      return;
    }
  } else {
    if (idxs.tail_.head_.is_ascending()) {
      auto row_size = idxs.head_.min_ - (idxs.head_.max_ - 1);
      auto col_size = idxs.tail_.head_.max_ - (idxs.tail_.head_.min_ - 1);
      stan::math::check_range(
          "matrix[reverse_min_max, min_max] assign col range", name, x.cols(),
          idxs.head_.max_);
      stan::math::check_range(
          "matrix[reverse_min_max, min_max] assign row range", name, x.rows(),
          idxs.tail_.head_.min_);
      stan::math::check_size_match(
          "matrix[reverse_min_max, min_max] assign row sizes", "lhs", row_size,
          name, y.rows());
      stan::math::check_size_match(
          "matrix[reverse_min_max, min_max] assign col sizes", "lhs", col_size,
          name, y.cols());
      x.block(idxs.head_.max_ - 1, idxs.tail_.head_.min_ - 1, row_size,
              col_size)
          = y.colwise().reverse();
      return;
    } else {
      auto row_size = idxs.head_.min_ - (idxs.head_.max_ - 1);
      auto col_size = idxs.tail_.head_.min_ - (idxs.tail_.head_.max_ - 1);
      stan::math::check_range(
          "matrix[reverse_min_max, reverse_min_max] assign col range", name,
          x.cols(), idxs.head_.max_);
      stan::math::check_range(
          "matrix[reverse_min_max, reverse_min_max] assign row range", name,
          x.rows(), idxs.tail_.head_.max_);
      stan::math::check_size_match(
          "matrix[reverse_min_max, reverse_min_max] assign row sizes", "lhs",
          row_size, name, y.rows());
      stan::math::check_size_match(
          "matrix[reverse_min_max, reverse_min_max] assign col sizes", "lhs",
          col_size, name, y.cols());
      x.block(idxs.head_.max_ - 1, idxs.tail_.head_.max_ - 1, row_size,
              col_size)
          = y.reverse();
      return;
    }
  }
}

/**
 * Assign to a cell of an Eigen Matrix.
 *
 * Types:  mat[single, single] = scalar
 *
 * @tparam Mat Eigen type with dynamic rows and columns.
 * @tparam U Scalar type.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Sequence of two single indexes (from 1).
 * @param[in] y Value scalar.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If either of the indices are out of bounds.
 */
template <typename Mat, typename U,
          stan::internal::require_eigen_dense_dynamic_t<Mat>* = nullptr>
inline void assign(
    Mat&& x,
    const cons_index_list<index_uni,
                          cons_index_list<index_uni, nil_index_list>>& idxs,
    const U& y, const char* name = "ANON", int depth = 0) {
  const int m = idxs.head_.n_;
  const int n = idxs.tail_.head_.n_;
  stan::math::check_range("matrix[uni,uni] assign range", name, x.rows(), m);
  stan::math::check_range("matrix[uni,uni] assign range", name, x.cols(), n);
  x.coeffRef(m - 1, n - 1) = y;
}

/**
 * Assign multiple possibly unordered cells of row vector to a row of an eigen
 * matrix.
 *
 * Types:  mat[uni, multi] = row_vector
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Vec Eigen type with dynamic columns and compile time rows of 1.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs A list with a uni index for rows and multi index for columns.
 * @param[in] y Row vector.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and value matrix do not match.
 */
template <typename Mat1, typename Vec,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr,
          require_eigen_row_vector_t<Vec>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<index_uni,
                          cons_index_list<index_multi, nil_index_list>>& idxs,
    const Vec& y, const char* name = "ANON", int depth = 0) {
  const auto& y_ref = stan::math::to_ref(y);
  stan::math::check_range("matrix[uni, multi] assign range", name, x.rows(),
                          idxs.head_.n_);
  stan::math::check_size_match("matrix[uni, multi] assign sizes", "lhs",
                               idxs.tail_.head_.ns_.size(), name, y_ref.size());
  for (int i = 0; i < idxs.tail_.head_.ns_.size(); ++i) {
    stan::math::check_range("matrix[uni, multi] assign range", name, x.cols(),
                            idxs.tail_.head_.ns_[i]);
    x.coeffRef(idxs.head_.n_ - 1, idxs.tail_.head_.ns_[i] - 1) = y_ref.coeff(i);
  }
}

/**
 * Assign to multiple possibly unordered cell's of a matrix from an input
 * matrix.
 *
 * Types:  mat[multi, multi] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type with dynamic rows and columns.
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Pair of multiple indexes (from 1).
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and value matrix do not match.
 */
template <
    typename Mat1, typename Mat2,
    stan::internal::require_all_eigen_dense_dynamic_t<Mat1, Mat2>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<index_multi,
                          cons_index_list<index_multi, nil_index_list>>& idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  const auto& y_ref = stan::math::to_ref(y);
  stan::math::check_size_match("matrix[multi,multi] assign sizes", "lhs",
                               idxs.head_.ns_.size(), name, y_ref.rows());
  stan::math::check_size_match("matrix[multi,multi] assign sizes", "lhs",
                               idxs.tail_.head_.ns_.size(), name, y_ref.cols());
  for (int j = 0; j < y_ref.cols(); ++j) {
    const int n = idxs.tail_.head_.ns_[j];
    stan::math::check_range("matrix[multi,multi] assign range", name, x.cols(),
                            n);
    for (int i = 0; i < y_ref.rows(); ++i) {
      const int m = idxs.head_.ns_[i];
      stan::math::check_range("matrix[multi,multi] assign range", name,
                              x.rows(), m);
      x.coeffRef(m - 1, n - 1) = y_ref.coeff(i, j);
    }
  }
}

/**
 * Assign to any rows of a single column of a matrix.
 *
 * Types:  mat[Idx, uni] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Container holding row index and a min_max index.
 * @param[in] y Matrix variable to assign from.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_uni, nil_index_list>>&
        idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("matrix[..., uni] assign range", name, x.cols(),
                          idxs.tail_.head_.n_);
  assign(x.col(idxs.tail_.head_.n_ - 1), index_list(idxs.head_), y, name,
         depth + 1);
  return;
}

/**
 * Assign to a non-contiguous set of columns of a matrix.
 *
 * Types:  mat[Idx, multi] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Pair of multiple indexes (from 1).
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and value matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_multi, nil_index_list>>&
        idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  const auto& y_ref = stan::math::to_ref(y);
  stan::math::check_size_match("matrix[..., multi] assign sizes", "lhs",
                               idxs.tail_.head_.ns_.size(), name, y_ref.cols());
  for (int j = 0; j < y_ref.cols(); ++j) {
    const int n = idxs.tail_.head_.ns_[j];
    stan::math::check_range("matrix[..., multi] assign range", name, x.cols(),
                            n);
    assign(x.col(n - 1), index_list(idxs.head_), y_ref.col(j), name, depth + 1);
  }
}

/**
 * Assign to any rows of a matrix.
 *
 * Types:  mat[Idx, omni] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Pair of multiple indexes (from 1).
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and value matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_omni, nil_index_list>>&
        idxs,
    Mat2&& y, const char* name = "ANON", int depth = 0) {
  assign(x, index_list(idxs.head_), std::forward<Mat2>(y), name, depth + 1);
}

/**
 * Assign any rows and min:N columns of a matrix.
 *
 * Types:  mat[Idx, min] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Container holding a row index and an index from a minimum
 * index (inclusive) to the end of a container.
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_min, nil_index_list>>&
        idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  const auto start_col = idxs.tail_.head_.min_ - 1;
  const auto col_size = x.cols() - start_col;
  stan::math::check_size_match("matrix[..., min] assign col sizes", "lhs",
                               col_size, name, y.cols());
  assign(x.rightCols(col_size), index_list(idxs.head_), y, name, depth + 1);
}

/**
 * Assign to any rows and 1:max columns of a matrix.
 *
 * Types:  mat[Idx, max] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Index holding a row index and an index from the start of the
 * container up to the specified maximum index (inclusive).
 * @param[in] y Value matrix.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_max, nil_index_list>>&
        idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_size_match("matrix[..., max] assign col size", "lhs",
                               idxs.tail_.head_.max_, name, y.cols());
  assign(x.leftCols(idxs.tail_.head_.max_), index_list(idxs.head_), y, name,
         depth + 1);
}

/**
 * Assign to any rows and a range of columns.
 *
 * Types:  mat[Idx, min_max] = mat
 *
 * @tparam Mat1 Eigen type with dynamic rows and columns.
 * @tparam Mat2 Eigen type
 * @tparam Idx The row index type
 * @param[in] x Matrix variable to be assigned.
 * @param[in] idxs Container holding row index and a min_max index.
 * @param[in] y Matrix variable to assign from.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions of the indexed
 * matrix and right-hand side matrix do not match.
 */
template <typename Mat1, typename Mat2, typename Idx,
          stan::internal::require_eigen_dense_dynamic_t<Mat1>* = nullptr>
inline void assign(
    Mat1&& x,
    const cons_index_list<Idx, cons_index_list<index_min_max, nil_index_list>>&
        idxs,
    const Mat2& y, const char* name = "ANON", int depth = 0) {
  if (idxs.tail_.head_.is_ascending()) {
    const auto col_start = idxs.tail_.head_.min_ - 1;
    const auto col_size = idxs.tail_.head_.max_ - col_start;
    stan::math::check_range("matrix[..., min_max] assign range", name, x.cols(),
                            idxs.tail_.head_.min_);
    stan::math::check_range("matrix[..., min_max] assign range", name,
                            idxs.tail_.head_.max_, x.cols());
    stan::math::check_size_match("matrix[..., min_max] assign col size", "lhs",
                                 idxs.tail_.head_.max_, name, x.cols());
    assign(x.middleCols(col_start, col_size), index_list(idxs.head_), y, name,
           depth + 1);
    return;
  } else {
    const auto col_start = idxs.tail_.head_.max_ - 1;
    const auto col_size = idxs.tail_.head_.min_ - col_start;
    stan::math::check_range("matrix[..., reverse_min_max] assign range", name,
                            x.cols(), idxs.tail_.head_.max_);
    stan::math::check_range("matrix[..., reverse_min_max] assign range", name,
                            idxs.tail_.head_.min_, x.cols());
    stan::math::check_size_match("matrix[..., min_max] assign col size", "lhs",
                                 idxs.tail_.head_.min_, name, x.cols());
    assign(x.middleCols(col_start, col_size), index_list(idxs.head_),
           y.rowwise().reverse(), name, depth + 1);
    return;
  }
}

/**
 * Assign the elements of one standard vector to another.
 *
 *  std_vector = std_vector
 *
 * @tparam T Type of Std vector to be assigned to.
 * @tparam U Type of Std vector to be assigned from.
 * @param[in] x lvalue variable
 * @param[in] y rvalue variable
 * @param[in] name name of lvalue variable (default "ANON").
 * @param[in] depth indexing depth (default 0).
 */
template <typename T, typename U, require_all_std_vector_t<T, U>* = nullptr,
          require_not_t<
              std::is_assignable<std::decay_t<T>&, std::decay_t<U>>>* = nullptr>
inline void assign(T&& x, const nil_index_list& /* idxs */, U&& y,
                   const char* name = "ANON", int depth = 0) {
  x.resize(y.size());
  if (std::is_rvalue_reference<U>::value) {
    for (size_t i = 0; i < y.size(); ++i) {
      assign(x[i], nil_index_list(), std::move(y[i]), name, depth + 1);
    }
  } else {
    for (size_t i = 0; i < y.size(); ++i) {
      assign(x[i], nil_index_list(), y[i], name, depth + 1);
    }
  }
}

/**
 * Assign to a single element of an std vector with additional subsetting on
 *  that element.
 *
 * Types:  std_vector<T>[uni | Idx] = T
 *
 * @tparam StdVec A standard vector
 * @tparam Idx Type of tail of index list.
 * @tparam U A type assignable to the value type of `StdVec`
 * @param[in] x Array variable to be assigned.
 * @param[in] idxs List of indexes beginning with single index
 * (from 1).
 * @param[in] y Value.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions do not match in the
 * tail assignment.
 */
template <typename StdVec, typename Idx, typename U,
          require_std_vector_t<StdVec>* = nullptr>
inline void assign(StdVec&& x, const cons_index_list<index_uni, Idx>& idxs,
                   U&& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[uni,...] assign range", name, x.size(),
                          idxs.head_.n_);
  assign(x[idxs.head_.n_ - 1], idxs.tail_, std::forward<U>(y), name, depth + 1);
}

/**
 * Assign to a single element of an std vector.
 *
 * Types:  x[uni] = y
 *
 * @tparam StdVec A standard vector
 * @tparam Idx Type of tail of index list.
 * @tparam U A type assignable to the value type of `StdVec`
 * @param[in] x Array variable to be assigned.
 * @param[in] idxs A list of indices holding a single uni index.
 * @param[in] y Value.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the dimensions do not match in the
 * tail assignment.
 */
template <typename StdVec, typename U, require_std_vector_t<StdVec>* = nullptr,
          require_t<std::is_assignable<value_type_t<StdVec>&, U>>* = nullptr>
inline void assign(StdVec&& x,
                   const cons_index_list<index_uni, nil_index_list>& idxs,
                   U&& y, const char* name = "ANON", int depth = 0) {
  stan::math::check_range("vector[uni,...] assign range", name, x.size(),
                          idxs.head_.n_);
  x[idxs.head_.n_ - 1] = std::forward<U>(y);
}

/**
 * Assign to the elements of an std vector with additional subsetting on each
 * element.
 *
 * Types:  x[Idx1 | Idx2] = y
 *
 * @tparam T A standard vector.
 * @tparam Idx1 Type of multiple index heading index list.
 * @tparam Idx2 Type of tail of index list.
 * @tparam U A standard vector
 * @param[in] x Array variable to be assigned.
 * @param[in] idxs List of indexes
 * @param[in] y Value.
 * @param[in] name Name of variable (default "ANON").
 * @param[in] depth Indexing depth (default 0).
 * @throw std::out_of_range If any of the indices are out of bounds.
 * @throw std::invalid_argument If the size of the multiple indexing
 * and size of first dimension of value do not match, or any of
 * the recursive tail assignment dimensions do not match.
 */
template <typename T, typename Idx1, typename Idx2, typename U,
          require_all_std_vector_t<T, U>* = nullptr>
inline void assign(T&& x, const cons_index_list<Idx1, Idx2>& idxs, U&& y,
                   const char* name = "ANON", int depth = 0) {
  int x_idx_size = rvalue_index_size(idxs.head_, x.size());
  stan::math::check_size_match("vector[multi,...] assign sizes", "lhs",
                               x_idx_size, name, y.size());
  for (size_t n = 0; n < y.size(); ++n) {
    int i = rvalue_at(n, idxs.head_);
    stan::math::check_range("vector[multi,...] assign range", name, x.size(),
                            i);
    if (std::is_rvalue_reference<U>::value) {
      assign(x[i - 1], idxs.tail_, std::move(y[n]), name, depth + 1);
    } else {
      assign(x[i - 1], idxs.tail_, y[n], name, depth + 1);
    }
  }
}

}  // namespace model
}  // namespace stan
#endif
