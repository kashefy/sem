#ifndef SIMULATIONSEM_H_
#define SIMULATIONSEM_H_

#include <iostream>
#include <memory>
#include <string>

#include <opencv2/core/core.hpp>

#include "elm/core/base_Layer.h"
#include "elm/core/typedefs.h"

class SimulationSEM
{
public:
    SimulationSEM();

    void Learn();

    void Test();

    void Eval();

protected:
    // static members
    static const std::string NAME_STIMULUS;
    static const std::string NAME_POP_CODE;
    static const std::string NAME_SPIKES_Y;
    static const std::string NAME_SPIKES_Z;
    static const std::string NAME_WEIGHTS;

    // methods
    /**
     * @brief Initialize layer for population coding
     * @return refernce to popuation code layer instance
     */
    elm::LayerShared InitPopulationCode() const;

    /**
     * @brief Initialize layer of spiking neurons
     * @return refernce to layer instance
     */
    elm::LayerShared InitLayerY() const;

    /**
     * @brief Initialize layer of spiking learners
     * @param no. of features (e.g no. of afferents)
     * @param length of spiking history
     * @return reference to layer instance
     */
    elm::LayerShared InitLearners(int nb_features, int history_length) const;

    void VisualizeOnOffWeights(const cv::Mat1f &weights);

    // members
    elm::LayerShared pop_code_;
    elm::LayerShared y_;
    elm::LayerShared z_;
    size_t nb_learners_;   ///< no. of learners (e.g. ZNeurons)

};
#endif // SIMULATIONSEM_H_
