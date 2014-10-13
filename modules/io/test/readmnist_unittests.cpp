#include "io/readmnist.h"

#include "ts/ts.h"          ///< custom assertions

#include <boost/filesystem.hpp>

#include "core/exception.h"
#include "io/binary.h"      ///< determine and swap endianness

using namespace std;
namespace bfs=boost::filesystem; // use alias

class FakeMNISTLabelsWriter {

public:

    static const int NB_ITEMS = 10;

    virtual ~FakeMNISTLabelsWriter()
    {
    }

    FakeMNISTLabelsWriter(const bfs::path& p)
        : path_(p)
    {
    }

    /**
     * @brief Save fake data to file
     * magic number
     * total no. of items
     * series label values [0,10)
     */
    virtual void Save(int magic_number)
    {
        SaveHeader(magic_number);
        SaveItems();
        out_.close();
    }

    void WriteInt(int n)
    {
        if(IS_32_LITTLE_ENDIAN) { sem::SwapEndian(&n); }
        out_.write(reinterpret_cast<char*>(&n), sizeof(int32_t));
    }

protected:

    virtual void SaveHeader(int magic_number)
    {
        if(out_.is_open())
        {
            out_.close();
        }
        out_.open(path_.string().c_str(), ios::out | ios::binary);
        if(!out_.is_open()) {

            stringstream s;
            s << "Faied to write test file (" << path_ << ").";
            SEM_THROW_FILEIO_ERROR(s.str());
        }

        WriteInt(magic_number);
        WriteInt(NB_ITEMS);
    }

    virtual void SaveItems()
    {
        for(int i=0; i<NB_ITEMS; i++) {

            unsigned char next_label = static_cast<unsigned char>(i%10);
            out_.write(reinterpret_cast<char*>(&next_label), sizeof(unsigned char));
        }
    }

    bfs::path path_;
    ofstream out_;
};

/**
 * @brief class for testing ReadMNISTLabels
 * Fake data is written to disk first, then removed after last test
 */
class ReadMNISTLabelsTest : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        bfs::create_directory(test_data_dir_);
        FakeMNISTLabelsWriter writer(test_data_path_);

        writer.Save(ReadMNISTLabels().MagicNumber());
    }

    static void TearDownTestCase()
    {
        if(bfs::is_directory(test_data_dir_)) {

            bfs::remove_all(test_data_dir_);
        }
    }

    static const bfs::path test_data_dir_;
    static const bfs::path test_data_path_;
};
const bfs::path ReadMNISTLabelsTest::test_data_dir_("tmp_ReadMNISTFileTest");
const bfs::path ReadMNISTLabelsTest::test_data_path_(test_data_dir_/"fake_data.tmp");

TEST_F(ReadMNISTLabelsTest, WrongPath)
{
    EXPECT_THROW(ReadMNISTLabels().ReadHeader("foo.bar"), ExceptionFileIOError);
}

TEST_F(ReadMNISTLabelsTest, ReadHeader)
{
    const int N = FakeMNISTLabelsWriter::NB_ITEMS;
    EXPECT_EQ(N, ReadMNISTLabels().ReadHeader(test_data_path_.string().c_str()));
}

TEST_F(ReadMNISTLabelsTest, Next)
{
    ReadMNISTLabels to;
    const int N = to.ReadHeader(test_data_path_.string().c_str());

    EXPECT_GT(N, 0);

    for(int i=0; i<N*2; i++) {

        if(i<N) {

            EXPECT_FALSE(to.IS_EOF());
            int label = static_cast<int>(to.Next().at<unsigned char>(0));
            EXPECT_EQ(label, i%N);

            if(i<N-1) { EXPECT_FALSE(to.IS_EOF()); }
            else { EXPECT_TRUE(to.IS_EOF()); }
        }
        else { EXPECT_TRUE(to.IS_EOF()); }
    }
}

TEST_F(ReadMNISTLabelsTest, Invalid)
{
    bfs::path p = test_data_dir_/"fake_labels_data_wrong_magic.tmp";
    FakeMNISTLabelsWriter writer(p);
    ReadMNISTLabels to;
    writer.Save(to.MagicNumber()+5);
    EXPECT_THROW(to.ReadHeader(p.string().c_str()), ExceptionFileIOError);
}

class FakeMNISTImagesWriter : public FakeMNISTLabelsWriter
{

public:
    static const int NB_ITEMS = 10;
    static const int ROWS = 3;
    static const int COLS = 4;

    FakeMNISTImagesWriter(const bfs::path& p)
        : FakeMNISTLabelsWriter(p)
    {
    }

    /**
     * @brief Save fake data to file
     * magic number
     * total no. of items
     * series label values [0,10)
     */
    void SaveHeader(int magic_number)
    {
        FakeMNISTLabelsWriter::SaveHeader(magic_number);
        WriteInt(ROWS);
        WriteInt(COLS);
    }

    void SaveItems()
    {
        for(int i=0; i<NB_ITEMS; i++) {

            unsigned char c = static_cast<unsigned char>(i % 255);
            cv::Mat next_img = cv::Mat(ROWS, COLS, CV_8UC1, c);
            out_.write(reinterpret_cast<char*>(next_img.data), sizeof(unsigned char)*next_img.total());
        }
    }

protected:

    bfs::path path_;
};

class ReadMNISTImagesTest : public ReadMNISTLabelsTest
{
protected:
    static void SetUpTestCase()
    {
        ReadMNISTLabelsTest::SetUpTestCase();

        FakeMNISTImagesWriter writer(test_data_path_);
        writer.Save(ReadMNISTImages().MagicNumber());
    }
};


TEST_F(ReadMNISTImagesTest, WrongPath)
{
    EXPECT_THROW(ReadMNISTImages().ReadHeader("foo.bar"), ExceptionFileIOError);
}

TEST_F(ReadMNISTImagesTest, ReadHeader)
{
    const int N = FakeMNISTImagesWriter::NB_ITEMS;
    EXPECT_EQ(N, ReadMNISTImages().ReadHeader(test_data_path_.string().c_str()));
}

TEST_F(ReadMNISTImagesTest, Next)
{
    ReadMNISTImages to;
    const int N = to.ReadHeader(test_data_path_.string().c_str());
    const int ROWS = FakeMNISTImagesWriter::ROWS;
    const int COLS = FakeMNISTImagesWriter::COLS;

    EXPECT_GT(N, 0);

    for(int i=0; i<N*2; i++) {

        if(i<N) {

            EXPECT_FALSE(to.IS_EOF());
            cv::Mat img = to.Next();
            EXPECT_MAT_DIMS_EQ(img, cv::Mat(ROWS, COLS, CV_8UC1));
            int sum = cv::sum(img)(0);
            sum /= (ROWS*COLS);
            EXPECT_EQ(sum, static_cast<unsigned char>(i%255));

            if(i<N-1) { EXPECT_FALSE(to.IS_EOF()); }
            else { EXPECT_TRUE(to.IS_EOF()); }
        }
        else { EXPECT_TRUE(to.IS_EOF()); }
    }
}

TEST_F(ReadMNISTImagesTest, Invalid)
{
    bfs::path p = test_data_dir_/"fake_images_data_wrong_magic.tmp";
    FakeMNISTImagesWriter writer(p);
    ReadMNISTImages to;
    writer.Save(to.MagicNumber()+5);
    EXPECT_THROW(to.ReadHeader(p.string().c_str()), ExceptionFileIOError);
}
