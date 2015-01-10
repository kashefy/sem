#ifndef SEM_CORE_FEATUREDATA_H_
#define SEM_CORE_FEATUREDATA_H_

#include <memory>

#include <boost/variant.hpp>

#include <opencv2/core.hpp>

#include "core/mat_utils.h"
#include "core/pcl_utils.h"

/**
 * @brief base class for caching feature data conversion
 */
class FeatDataConversionCache
{
public:
    /**
     * @brief Reset cache indicating a conversion took place
     */
    virtual void Reset();
};

/**
 * @brief template class for different static visitors
 */
template <typename T>
class FeatDataVisitor_ :
        public boost::static_visitor<T >,
        public FeatDataConversionCache
{
};

/**
 * @brief visitor class for converting to Mat of floats
 */
class FeatDataVisitorMatF :
        public FeatDataVisitor_<cv::Mat_<float> >
{
public:
    cv::Mat_<float> operator()(const cv::Mat_<float> &m) const;

    cv::Mat_<float> operator()(sem::PointCloudXYZ::Ptr &c) const;
};

/**
 * @brief visitor class for converting to pcl point cloud
 * And keeping track of when a heavy conversion (involving deep copy) occured
 */
class FeatDataVisitorCloud :
        public FeatDataVisitor_<sem::PointCloudXYZ::Ptr >
{
public:
    void Reset();

    sem::PointCloudXYZ::Ptr operator()(sem::PointCloudXYZ::Ptr &c);

    sem::PointCloudXYZ::Ptr operator()(const cv::Mat_<float> &m);

protected:
    sem::PointCloudXYZ::Ptr c_; ///< internal reference for caching most recent conversion result
};

/**
 * @brief The Feature data class.
 * Encapsulating a feature in multiple types and caching different represenations
 */
class FeatureData
{
public:
    /**
     * @brief Construct a new object with initial representation
     */
    template <class T>
    FeatureData(const T &t)
    {
        var_ = t;
    }

    /**
     * @brief Extract new representation of underlying feature data.
     * Utilizes cached conversion if applicable
     * @return new feature data representation
     */
    template <class T>
    T get();

protected:
    FeatureData();

protected:
    /**
     * @brief Initialization routine common to all constructors
     */
    void Init();

    /**
     * @brief Reset visitor cache
     */
    void Reset();

    boost::variant< cv::Mat1f, sem::PointCloudXYZ::Ptr > var_; ///< variant object to enable finite representations of a single feature data instance

    FeatDataVisitorMatF visitor_mat_;       ///< visitor for converting to Mat objects
    FeatDataVisitorCloud visitor_cloud_;    ///< visitor for converting to pcl point clouds
};

#endif // SEM_CORE_FEATUREDATA_H_
