#include "sem/layers/layerfactorysem.h"

#include "elm/core/exception.h"
#include "elm/core/layerconfig.h"
#include "elm/core/signal.h"
#include "elm/layers/saliencyitti.h" ///< to have layer dervied classes to test with, sem layer
#include "elm/layers/weightedsum.h"  ///< to have layer dervied classes to test with, elm layer
#include "elm/ts/ts.h"

using std::shared_ptr;
using namespace elm;

namespace {

/**
 * @brief class or testing LayerFactory's static methods
 */
class LayerFactorySEMStaticTest : public ::testing::Test
{
};

TEST_F(LayerFactorySEMStaticTest, CreateLayerPtrShared)
{
    {
        shared_ptr<base_Layer> ptr = LayerFactorySEM::CreateShared("LayerZ");
        EXPECT_TRUE(bool(ptr));
    }
    {
        shared_ptr<base_Layer> ptr = LayerFactorySEM::CreateShared("WeightedSum");
        EXPECT_TRUE(bool(ptr));
    }
}

TEST_F(LayerFactorySEMStaticTest, CreateLayerPtrShared_WrongType)
{
    EXPECT_THROW(LayerFactorySEM::CreateShared("Blahbla"), elm::ExceptionTypeError);
}

TEST_F(LayerFactorySEMStaticTest, CreateLayerPtrShared_UniqueInstancesSameType)
{
    const std::string TYPE="WeightedSum";

    shared_ptr<base_Layer> ptr1 = LayerFactorySEM::CreateShared(TYPE);
    shared_ptr<base_Layer> ptr2 = LayerFactorySEM::CreateShared(TYPE);

    EXPECT_NE(ptr1, ptr2);
}

TEST_F(LayerFactorySEMStaticTest, CreateLayerPtrShared_WithConfig)
{
    PTree params;
    params.put(WeightedSum::PARAM_A, 0.2f);
    params.put(WeightedSum::PARAM_B, 0.3f);
    LayerConfig config;
    config.Params(params);

    const std::string NAME_STIMULUS = "in";
    const std::string NAME_RESPONSE = "out";

    config.Input(WeightedSum::KEY_INPUT_STIMULUS, NAME_STIMULUS);
    config.Output(WeightedSum::KEY_OUTPUT_RESPONSE, NAME_RESPONSE);

    shared_ptr<base_Layer> ptr = LayerFactorySEM::CreateShared("WeightedSum", config, config);
    EXPECT_TRUE(bool(ptr));

    // populate signal with input feature
    Signal signal;
    signal.Append("in", cv::Mat1f::ones(1, 2));
    EXPECT_TRUE(signal.Exists(NAME_STIMULUS));
    EXPECT_FALSE(signal.Exists(NAME_RESPONSE));

    // apply signal to layer
    ptr->Activate(signal);
    ptr->Response(signal);

    // check response
    EXPECT_TRUE(signal.Exists(NAME_RESPONSE));
    EXPECT_MAT_DIMS_EQ(signal.MostRecentMat1f(NAME_RESPONSE), cv::Size2i(1, 1));
    EXPECT_FLOAT_EQ(signal.MostRecentMat1f(NAME_RESPONSE).at<float>(0), 0.5f);
}

} // annonymous namespace
