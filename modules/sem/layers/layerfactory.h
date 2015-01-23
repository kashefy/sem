#ifndef SEM_LAYERFACTORY_H_
#define SEM_LAYERFACTORY_H_

#include <memory>
#include <string>

#include "elm/core/base_Layer.h"
#include "elm/core/exception.h"

/**
 * @brief class for implementing layer related factory methods
 * Such as instantiation and sequencing of multiple layer applications (e.g. pipeline)
 * @todo enable post hoc addition of custom layer instances
 */
class LayerFactory
{
public:
    typedef std::string LayerType;

    LayerFactory();

    /**
     * @brief Create smart pointer to an instantiated layer
     * @param type
     * @return pointer to layer instance
     * @throws ExceptionTypeError on unrecognized layer type
     */
    static std::shared_ptr<base_Layer> CreateShared(const LayerType &type);

    /**
     * @brief Create smart pointer to an instantiated layer
     * @param type
     * @param configuration
     * @param I/O names
     * @return pointer to layer instance
     * @throws ExceptionTypeError on unrecognized layer type
     */
    static std::shared_ptr<base_Layer> CreateShared(const LayerType &type,
                                                            const LayerConfig &config,
                                                            const LayerIONames &io);
};

#endif // SEM_LAYERFACTORY_H_
