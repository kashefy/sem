#ifndef _SEM_LAYERS_LAYERFACTORYSEM_H_
#define _SEM_LAYERS_LAYERFACTORYSEM_H_

#include <memory>
#include <string>

#include "elm/core/typedefs_fwd.h"

#include "elm/core/base_Layer.h"

/**
 * @brief class for implementing layer related factory methods
 * Such as instantiation and sequencing of multiple layer applications (e.g. pipeline)
 * @todo enable post hoc addition of custom layer instances
 */
class LayerFactorySEM
{
public:
    typedef std::string LayerType;

    LayerFactorySEM();

    /**
     * @brief Create smart pointer to an instantiated layer
     * @param type
     * @return pointer to layer instance
     * @throws ExceptionTypeError on unrecognized layer type
     */
    static elm::LayerShared CreateShared(const LayerType &type);

    /**
     * @brief Create smart pointer to an instantiated layer
     * @param type
     * @param configuration
     * @param I/O names
     * @return pointer to layer instance
     * @throws ExceptionTypeError on unrecognized layer type
     */
    static elm::LayerShared CreateShared(const LayerType &type,
                                         const elm::LayerConfig &config,
                                         const elm::LayerIONames &io);
};

#endif // _SEM_LAYERS_LAYERFACTORYSEM_H_
