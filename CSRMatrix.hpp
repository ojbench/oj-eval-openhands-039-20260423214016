#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

template <typename T>
class CSRMatrix {

private:
    size_t n_rows;
    size_t n_cols;
    size_t nnz;
    std::vector<size_t> indptr_;
    std::vector<size_t> indices_;
    std::vector<T> data_;
    
public:
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    CSRMatrix(size_t n, size_t m) : n_rows(n), n_cols(m), nnz(0) {
        indptr_.resize(n + 1, 0);
    }

    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr, 
        const std::vector<size_t> &indices,
        const std::vector<T> &data) 
        : n_rows(n), n_cols(m), nnz(count), 
          indptr_(indptr), indices_(indices), data_(data) {
    }

    CSRMatrix(const CSRMatrix &other) = default;

    CSRMatrix(CSRMatrix &&other) = default;

    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &data) 
        : n_rows(n), n_cols(m), nnz(0) {
        indptr_.resize(n + 1, 0);
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                if (data[i][j] != T()) {
                    indices_.push_back(j);
                    data_.push_back(data[i][j]);
                    ++nnz;
                }
            }
            indptr_[i + 1] = nnz;
        }
    }

    ~CSRMatrix() = default;

    size_t getRowSize() const {
        return n_rows;
    }

    size_t getColSize() const {
        return n_cols;
    }

    size_t getNonZeroCount() const {
        return nnz;
    }

    T get(size_t i, size_t j) const {
        if (i >= n_rows || j >= n_cols) {
            throw invalid_index();
        }
        
        size_t start = indptr_[i];
        size_t end = indptr_[i + 1];
        
        for (size_t k = start; k < end; ++k) {
            if (indices_[k] == j) {
                return data_[k];
            }
        }
        
        return T();
    }

    void set(size_t i, size_t j, const T &value) {
        if (i >= n_rows || j >= n_cols) {
            throw invalid_index();
        }
        
        size_t start = indptr_[i];
        size_t end = indptr_[i + 1];
        
        // Check if element already exists
        for (size_t k = start; k < end; ++k) {
            if (indices_[k] == j) {
                data_[k] = value;
                return;
            }
            if (indices_[k] > j) {
                // Need to insert at position k
                indices_.insert(indices_.begin() + k, j);
                data_.insert(data_.begin() + k, value);
                ++nnz;
                // Update indptr for all subsequent rows
                for (size_t r = i + 1; r <= n_rows; ++r) {
                    ++indptr_[r];
                }
                return;
            }
        }
        
        // Insert at end of row
        indices_.insert(indices_.begin() + end, j);
        data_.insert(data_.begin() + end, value);
        ++nnz;
        for (size_t r = i + 1; r <= n_rows; ++r) {
            ++indptr_[r];
        }
    }

    const std::vector<size_t> &getIndptr() const {
        return indptr_;
    }

    const std::vector<size_t> &getIndices() const {
        return indices_;
    }

    const std::vector<T> &getData() const {
        return data_;
    }

    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> result(n_rows, std::vector<T>(n_cols, T()));
        
        for (size_t i = 0; i < n_rows; ++i) {
            size_t start = indptr_[i];
            size_t end = indptr_[i + 1];
            
            for (size_t k = start; k < end; ++k) {
                result[i][indices_[k]] = data_[k];
            }
        }
        
        return result;
    }

    std::vector<T> operator*(const std::vector<T> &vec) const {
        if (vec.size() != n_cols) {
            throw size_mismatch();
        }
        
        std::vector<T> result(n_rows, T());
        
        for (size_t i = 0; i < n_rows; ++i) {
            size_t start = indptr_[i];
            size_t end = indptr_[i + 1];
            
            T sum = T();
            for (size_t k = start; k < end; ++k) {
                sum = sum + data_[k] * vec[indices_[k]];
            }
            result[i] = sum;
        }
        
        return result;
    }

    CSRMatrix getRowSlice(size_t l, size_t r) const {
        if (l > r || r > n_rows) {
            throw invalid_index();
        }
        
        size_t new_rows = r - l;
        size_t start = indptr_[l];
        size_t end = indptr_[r];
        size_t new_nnz = end - start;
        
        std::vector<size_t> new_indptr(new_rows + 1);
        std::vector<size_t> new_indices;
        std::vector<T> new_data;
        
        new_indptr[0] = 0;
        for (size_t i = 0; i < new_rows; ++i) {
            new_indptr[i + 1] = indptr_[l + i + 1] - start;
        }
        
        for (size_t k = start; k < end; ++k) {
            new_indices.push_back(indices_[k]);
            new_data.push_back(data_[k]);
        }
        
        return CSRMatrix(new_rows, n_cols, new_nnz, new_indptr, new_indices, new_data);
    }
};

}

#endif // CSR_MATRIX_HPP
