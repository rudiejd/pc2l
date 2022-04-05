// Copyright 2021 JD Rudie
#ifndef MATRIX_CPP
#define MATRIX_CPP


#include <cassert>
#include <vector>
#include <string>
#include "pc2l.h"
#include "Matrix.h"

Matrix::Matrix(const size_t row, const size_t col, const Val initVal) :
// TODO: Add initialization with value for PC2L vectors. perhaps faster init?
        pc2l::Vector<Val>(), rows(row), cols(col){
}

// Operator to write the matrix to a given output stream
std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    // Print the number of rows and columns to ease reading
    os << matrix.height() << " " << matrix.width() << '\n';
    // Print each entry to the output stream.
    for (size_t row = 0; row < matrix.height(); row++) {
        for (size_t col = 0; col < matrix.width(); col++) {
            os << matrix.at(matrix.width() * row + col) << " ";
        }
        // Print a new line at the end of each row just to format the
        // output a bit nicely.
        os << '\n';
    }
    return os;
}

// Operator the read the matrix from a given input stream
std::istream& operator>>(std::istream& is, Matrix& matrix) {
    size_t row, col;
    is >> row >> col;
    double cur;
    matrix = Matrix(row, col);
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            is >> cur;
//            matrix[i][j] = cur;
            matrix.insert(matrix.width() * i + j, cur);
        }
    }
    return is;
}

// Performs matrix subtraction
Matrix Matrix::operator-(const Matrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    Matrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
//            ret[row][col] = this->at(row)[col] - rhs[row][col];
            ret.insert(row * this->width() + col, this->at(row*this->width()))

        }
    }
    return ret;
}

// Performs Hadamard product
Matrix Matrix::operator*(const Matrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    Matrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret[row][col] = this->at(row)[col] * rhs[row][col];
        }
    }
    return ret;
}

// Dot product with scalar
Matrix Matrix::operator*(const Val val) const {
    Matrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret[row][col] = this->at(row)[col] * val;
        }
    }
    return ret;
}

// Performs matrix addition
Matrix Matrix::operator+(const Matrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    Matrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret[row][col] = this->at(row)[col] + rhs[row][col];
        }
    }
    return ret;
}

// dot product with matrix
Matrix Matrix::dot(const Matrix& rhs) const {
    if (this->width() != rhs.height()) {
        throw std::invalid_argument(std::string("LHS height must match RHS") +
                                    std::string("width and/or LHS width must match RHS height"));
    }
    Matrix ret(this->height(), rhs.width());
    for (size_t i = 0; i < this->height(); i++) {
        for (size_t j = 0; j < rhs.width(); j++) {
            for (size_t k = 0; k < rhs.height(); k++) {
                ret[i][j] += this->at(i)[k] * rhs[k][j];
            }
        }
    }
    return ret;
}

Matrix Matrix::transpose() const {
    Matrix ret(this->width(), this->height());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret[row][col] = this->at(col)[row];
        }
    }
    return ret;
}




#endif

