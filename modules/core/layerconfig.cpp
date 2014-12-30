#include "core/layerconfig.h"

#include "core/exception.h"
#include "core/stl.h"

using namespace std;
using namespace sem;

void LayerIONames::Input(const string &key, const string &name)
{
   inputs_[key] = name;
}

void LayerIONames::Output(const string &key, const string &name)
{
    outputs_[key] = name;
}

string LayerIONames::Input(const string &key) const
{
    string name;
    if(!Find<string>(inputs_, key, name)) {

        SEM_THROW_KEY_ERROR("No name found for input key \'" + key + "\'");
    }
    return name;
}

OptS LayerIONames::InputOpt(const string &key) const
{
    OptS o;
    string name;
    return Find<string>(outputs_, key, name)? name : o;
}

string LayerIONames::Output(const string &key) const
{
    string name;
    if(!Find<string>(outputs_, key, name)) {

        SEM_THROW_KEY_ERROR("No name found for input key \'" + key + "\'");
    }
    return name;
}

OptS LayerIONames::OutputOpt(const string &key) const
{
    OptS o;
    string name;
    return Find<string>(outputs_, key, name)? name : o;
}


LayerConfig::LayerConfig()
    : LayerIONames()
{
}

void LayerConfig::Params(const PTree &params)
{
    params_ = params;
}

PTree LayerConfig::Params() const
{
    return params_;
}
