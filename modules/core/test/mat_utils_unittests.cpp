#include "core/mat_utils.h"

#include "gtest/gtest.h"
#include "ts/ts.h"
#include <string>

using namespace std;
using namespace cv;
using namespace sem;

/**
 * @brief test calculation of cumulative sum
 */
TEST(MatUtilsTest, CumSum_zeros) {

    int N = 10;
    for(int r=1; r<N; r++) {

        for(int c=1; c<N; c++) {

            Mat1f in = Mat1f::zeros(r, c);
            Mat1f out;
            sem::CumSum(in, out);
            EXPECT_MAT_EQ(in, out);
        }
    }
}

/**
 * @brief calculcate cumulative sum on matrix of ones.
 */
TEST(MatUtilsTest, CumSum_ones) {

    int N = 5;
    for(int r=1; r<N; r++) {

        for(int c=1; c<N; c++) {

            Mat1f in = Mat1f::ones(r, c);
            Mat1f out;
            sem::CumSum(in, out);

            // check output
            EXPECT_MAT_DIMS_EQ(in, out);
            for(int i=0; i<r*c; i++) {
                EXPECT_FLOAT_EQ( out(i), i+1 );
            }
            EXPECT_FLOAT_EQ( out(out.rows-1, out.cols-1), out.rows*out.cols ) << "Expectig sum value in last element.";
        }
    }
}

/**
 * @brief calculcate cumulative sum on matrix of negative ones.
 */
TEST(MatUtilsTest, CumSum_ones_neg) {

    int N = 5;
    for(int r=1; r<N; r++) {

        for(int c=1; c<N; c++) {

            Mat1f in = -Mat1f::ones(r, c);
            Mat1f out;
            sem::CumSum(in, out);

            // check output
            EXPECT_MAT_DIMS_EQ(in, out);
            for(int i=0; i<r*c; i++) {
                EXPECT_FLOAT_EQ( out(i), -(i+1) );
            }
            EXPECT_FLOAT_EQ( out(out.rows-1, out.cols-1), -out.rows*out.cols ) << "Expectig sum value in last element.";
        }
    }
}

/**
 * @brief calculcate cumulative sum on empty input matrix. Empty in, empty out.
 */
TEST(MatUtilsTest, CumSum_empty) {

    Mat1f in = Mat1f::zeros(0, 0);
    Mat1f out;
    sem::CumSum(in, out);
    EXPECT_EQ(out.total(), 0);
}

/**
 * @brief test utility function for getting mat type string representation
 */
TEST(MatUtilsTest, TypeToString)
{
    Mat a;
    a = Mat(1, 1, CV_32FC1);
    EXPECT_GT(MatTypeToString(a).size(), 0);
    EXPECT_EQ(MatTypeToString(a), "CV_32F");

    EXPECT_GT(MatTypeToString(a).size(), 0);
    EXPECT_EQ(MatTypeToString(a), MatTypeToString(Mat(1, 1, CV_32FC2)));

    a = Mat(1, 1, CV_32SC1);
    EXPECT_GT(MatTypeToString(a).size(), 0);
    EXPECT_EQ(MatTypeToString(a), MatTypeToString(Mat(1, 1, CV_32SC2)));
}

TEST(MatUtilsTest, TemplateTypeToString)
{
    Mat1f a(1, 1);
    EXPECT_GT(MatTypeToString(a).size(), 0);
    EXPECT_EQ(MatTypeToString(a), "CV_32F");

    EXPECT_GT(MatTypeToString(a).size(), 0);
    EXPECT_EQ(MatTypeToString(a), MatTypeToString(Mat(1, 1, CV_32FC2)));

    Mat1i b(1, 1);
    EXPECT_GT(MatTypeToString(b).size(), 0);
    EXPECT_EQ(MatTypeToString(b), "CV_32S");
    EXPECT_EQ(MatTypeToString(b), MatTypeToString(Mat(1, 1, CV_32SC2)));
}

/**
 * @brief test utility function for getting mat type string representation
 * with same type but different number of channels
 */
TEST(MatUtilsTest, TypeToStringChannels)
{
    Size s(1, 1);
    for(int ch=1; ch<=4; ch++) {

        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_8U, ch))),  "CV_8U");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_8S, ch))),  "CV_8S");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_16U, ch))), "CV_16U");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_16S, ch))), "CV_16S");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_32S, ch))), "CV_32S");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_32F, ch))), "CV_32F");
        EXPECT_EQ(MatTypeToString(Mat(s, CV_MAKETYPE(CV_64F, ch))), "CV_64F");
    }
}

TEST(MatARangeTest, Empty)
{
    EXPECT_TRUE(ARange(0, 0, 0).empty());
    EXPECT_TRUE(ARange(1, 3, 0).empty());
    EXPECT_TRUE(ARange(10, 3, 0).empty());
    EXPECT_TRUE(ARange(10, 30, 50).empty());
}

TEST(MatARangeTest, Invalid)
{
    EXPECT_THROW(ARange(1, 3, -1), ExceptionValueError);
    EXPECT_THROW(ARange(1, -3, 1), ExceptionValueError);
}

TEST(MatARangeTest, Increasing)
{
//    {
//        Mat1i x = ARange(1000, 1010, 1);

//        EXPECT_EQ(x.rows, 1) << "Expecting row vector.";
//        EXPECT_MAT_DIMS_EQ(x, Mat(1, 9, CV_32SC1));

//        for(int i=0; i<static_cast<int>(x.total()); i++) {
//            EXPECT_EQ(x(i), 1000+i);
//        }
//    }

    {
        Mat x = ARange<unsigned char>(0, 5, 2);

        std::cout<<x<<std::endl;
        EXPECT_EQ(x.rows, 1) << "Expecting row vector.";
        EXPECT_MAT_DIMS_EQ(x, Mat(1, 3, CV_32SC1));


        for(int i=0, j=0; i<static_cast<int>(x.total()); i++, j+=2) {
            EXPECT_EQ(x.at<unsigned char>(i), static_cast<unsigned char>(j));
        }
    }

//    {
//        Mat x = ARange<float>(-1.f, 1.f, 0.5f);

//        EXPECT_EQ(x.rows, 1) << "Expecting row vector.";
//        EXPECT_MAT_DIMS_EQ(x, Mat(1, 5, CV_32SC1));

//        float j = -1.f;
//        for(int i=0; i<static_cast<int>(x.total()); i++) {
//            EXPECT_FLOAT_EQ(x.at<float>(i), j);
//            j += 0.5f;
//        }
//    }
}



