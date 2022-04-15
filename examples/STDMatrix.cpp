// Copyright 2021 JD Rudie
#ifndef STD_MATRIX_CPP
#define STD_MATRIX_CPP


#include <string>
#include "pc2l.h"
#include "STDMatrix.h"

STDMatrix::STDMatrix(const size_t row, const size_t col, const Val initVal) :
        pc2l::Vector<Val>(), rows(row), cols(col) {
    for (size_t i = 0; i < row; i++) {
        for(size_t j = 0; j < col; j++) {
            insert(i, j, initVal);
        }
    }
}

// Operator to write the matrix to a given output stream
std::ostream& operator<<(std::ostream& os, const STDMatrix& matrix) {
    // Print the number of rows and columns to ease reading
    os << matrix.height() << " " << matrix.width() << '\n';
    // Print each entry to the output stream.
    for (size_t row = 0; row < matrix.height(); row++) {
        for (size_t col = 0; col < matrix.width(); col++) {
            os << matrix.at(row, col) << " ";
        }
        // Print a new line at the end of each row just to format the
        // output a bit nicely.
        os << '\n';
    }
    return os;
}

// Operator the read the matrix from a given input stream
std::istream& operator>>(std::istream& is, STDMatrix& matrix) {
    size_t row, col;
    is >> row >> col;
    double cur;
    matrix = STDMatrix(row, col);
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            is >> cur;
//            matrix[i][j] = cur;
            matrix.replace(row, col, cur);
        }
    }
    return is;
}

// Performs matrix subtraction
STDMatrix STDMatrix::operator-(const STDMatrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    STDMatrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
//            ret[row][col] = this->at(row)[col] - rhs[row][col];
            ret.replace(row, col, at(row, col) - rhs.at(row, col));
        }
    }
    return ret;
}

// Performs Hadamard product
STDMatrix STDMatrix::operator*(const STDMatrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    STDMatrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret.insert(row, col, at(row, col) * rhs.at(row, col));
        }
    }
    return ret;
}

// Dot product with scalar
STDMatrix STDMatrix::operator*(const Val val) const {
    STDMatrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
            ret.insert(row, col, at(row, col) * val);
        }
    }
    return ret;
}

// Performs matrix addition
STDMatrix STDMatrix::operator+(const STDMatrix& rhs) const {
    if (this->width() != rhs.width() || this->height() != rhs.height()) {
        throw std::invalid_argument("Matrices must have same sizes");
    }
    STDMatrix ret(this->height(), this->width());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
//            ret[row][col] = this->at(row)[col] + rhs[row][col];
            ret.insert(row, col, at(row, col) + rhs.at(row, col));
        }
    }
    return ret;
}

// dot product with matrix
STDMatrix STDMatrix::dot(const STDMatrix& rhs) const {
    if (this->width() != rhs.height()) {
        throw std::invalid_argument(std::string("LHS height must match RHS") +
                                    std::string("width and/or LHS width must match RHS height"));
    }
    STDMatrix ret(this->height(), rhs.width());
    for (size_t i = 0; i < this->height(); i++) {
        for (size_t j = 0; j < rhs.width(); j++) {
            for (size_t k = 0; k < rhs.height(); k++) {
//                ret[i][j] += this->at(i)[k] * rhs[k][j];
                ret.insert(i, j, at(i, k) + rhs.at(k, j));
            }
        }
    }
    return ret;
}

STDMatrix STDMatrix::transpose() const {
    STDMatrix ret(this->width(), this->height());
    for (size_t row = 0; row < ret.height(); row++) {
        for (size_t col = 0; col < ret.width(); col++) {
//            ret[row][col] = this->at(col)[row];
            ret.insert(row, col, at(col, row));
        }
    }
    return ret;
}

#endif