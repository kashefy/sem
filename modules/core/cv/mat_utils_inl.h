/** @file template utility definitions that have to be defined inline
  */
#ifndef SEM_CORE_MAT_UTILS_INL_H__
#define SEM_CORE_MAT_UTILS_INL_H__

#include "core/cv/typedefs_fwd.h"
#include "core/exception.h"

namespace sem {

/**
 * @brief Function for converting a Mat_ object into a vector of same type
 * The mat is flattened beforehand.
 * Involves copying elements
 * May only work with matrices of POD (e.g. int, float, uchar,...)
 * Works with N-dimensional matrices
 *
 * @param matrix
 * @return resulting vector with flattened matrix data
 * @throws ExceptionTypeError on non-continuous matrix input
 */
template <typename T>
std::vector<T> Mat_ToVec_(const cv::Mat_<T> &m) {

    if(!m.empty() && !m.isContinuous()) {

        SEM_THROW_TYPE_ERROR("Only conitnuous matrices supported.");
    }
    // syntax learned from posts here:
    // http://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords
    const T* p = m.template ptr<T>(0);
    std::vector<T> v(p, p+m.total());
    return v;
}

/** @brief Function for converting STL vector of POD type into an OpenCV Mat.
 *
 * No deep copy.
 * The data is not copied to the matrix.
 * The data remains owned by the vector
 * The resulting mat object should not be referenced if the source vector goes out of scope
 *
 * @param source vector of POD type
 * @return resulting row matrix, matrix with single row pointing to the vector's underlying data
 */
template <typename T>
cv::Mat_<T> Vec_ToRowMat_(std::vector<T> &v)
{
    return cv::Mat_<T>(1, static_cast<int>(v.size()), v.data());
}

/**
 * @brief get first index of element with a specific value in matrix
 * Inspired by this Stack Overflow post: @see http://stackoverflow.com/questions/25835587/find-element-in-opencv-mat-efficiently
 *
 * @param[in] matrix to search in
 * @param[in] value to search for
 * @param[out] first index containing this value, only meaningful if found
 * @return true if sucessfully found, always flase for empty input.
 *
 * @throws ExceptionBadDims if no. of channels != 1, ExceptionTypeError for non-continuous matrix input.
 */
static int _NA=-1;   ///< not applicable
template <class T>
bool find_first_of(const cv::Mat &m, const T &value, int &index=_NA)
{
    if(!m.empty()) {

        if(m.channels() != 1) { SEM_THROW_BAD_DIMS("Only single-channel matrices supported for now."); }
        if(!m.isContinuous()) { SEM_THROW_TYPE_ERROR("Only continuous matrices supported for now."); }
    }
    else if(m.channels() > 1) { SEM_THROW_BAD_DIMS("Only single-channel matrices supported for now."); }

    for(int r=0; r < m.rows; r++) {

        const T* row = m.ptr<T>(r);
        const T* result = std::find(row, row + m.cols, value);
        if(result != row + m.cols) {

            index = static_cast<int>(result - m.ptr<T>(0));
            return true;
        }
    }
    index = -1;
    return false;
}

/**
 * @brief Create Mat object (row vector) and fill with range
 * @param start value
 * @param stop value (exclusive)
 * @param step value
 * @return template
 */
template <class T>
cv::Mat_<T> ARange_(T start, T stop, T step)
{
    double diff = static_cast<double>(stop)-static_cast<double>(start);

    if((diff > 0 && step < 0) || (diff < 0 && step > 0)) {

        SEM_THROW_VALUE_ERROR("step incompatible with start and stop.");
    }

    int cols;
    if(step == 0) {

        cols = 0;
    }
    else {

        // Need full integer columns
        double tmp = (diff < 0)? abs(diff) : diff;
        tmp = (tmp+1)/fabs(static_cast<double>(step));
        cols = static_cast<int>(tmp);
    }

    cv::Mat_<T> m(1, cols);
    T value = start;
    int count = 0;
    bool done = (step > 0)? value >= stop : value <= stop;
    while(!done && count < cols) {

        m(count++) = value;
        value += step;
        done = (step > 0)? value >= stop : value <= stop;
    }

    // exclude stop value
    if(count < cols) { m = m.colRange(0, count); }

    return m;
}

} // sem namespace

#endif // SEM_CORE_MAT_UTILS_INL_H__
