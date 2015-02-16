#include "sem/layers/layerfactorysem.h"

#include <boost/assign/list_of.hpp>

#include "elm/core/registor.h"

/** Whenever a new layer is imeplemented:
 *  1. include its header below
 *  2. Add it to the initialization of g_layerRegistry map.
 */
#include "elm/layers/attentionwindow.h"
#include "elm/layers/layer_y.h"
#include "elm/encoding/populationcode.h"
#include "elm/layers/saliencyitti.h"
#include "elm/layers/weightedsum.h"
#include "sem/layers/layer_z.h"

using boost::assign::map_list_of;

typedef Registor_<base_Layer> LayerRegistor;
typedef Registor_<base_Layer>::Registry LayerRegistry;

/** Macros for creating individual registry pair items
 *  credit: J. Turcot, T. Senechal, http://stackoverflow.com/questions/138600/initializing-a-static-stdmapint-int-in-c
 */
#define REGISTRY_PAIR(Registor, NewInstance) (#NewInstance, &Registor::DerivedInstance<NewInstance>)
#define LAYER_REGISTRY_PAIR(NewInstance) REGISTRY_PAIR(LayerRegistor, NewInstance)

LayerRegistry g_layerRegistry = map_list_of
        LAYER_REGISTRY_PAIR( AttentionWindow )
        LAYER_REGISTRY_PAIR( LayerY )
        LAYER_REGISTRY_PAIR( LayerZ )
        LAYER_REGISTRY_PAIR( MutexPopulationCode )
        LAYER_REGISTRY_PAIR( SaliencyItti )
        LAYER_REGISTRY_PAIR( WeightedSum )
        ; ///< <-- add new layer to registry here

LayerFactorySEM::LayerFactorySEM()
{
}

LayerRegistor::RegisteredTypeSharedPtr LayerFactorySEM::CreateShared(const LayerType &type)
{
    return LayerRegistor::CreatePtrShared(g_layerRegistry, type);
}

LayerRegistor::RegisteredTypeSharedPtr LayerFactorySEM::CreateShared(const LayerType &type,
                                                                          const LayerConfig &config,
                                                                          const LayerIONames &io)
{
    LayerRegistor::RegisteredTypeSharedPtr ptr = LayerFactorySEM::CreateShared(type);
    ptr->Reset(config);
    ptr->IONames(io);
    return ptr;
}