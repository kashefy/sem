#include "sem/neuron/wtapoisson.h"

#include "elm/core/exception.h"
#include "elm/core/sampler.h"

using namespace std;
using namespace cv;
using namespace elm;

WTAPoisson::WTAPoisson(float max_frequency, float delta_t_msec)
    : base_WTA(delta_t_msec),
      lambda_(max_frequency)
{
    NextSpikeTime();
}

void WTAPoisson::NextSpikeTime()
{
    next_spike_time_sec_ = elm::randexp(lambda_);
}

Mat WTAPoisson::Compete(vector<shared_ptr<base_Learner> > &learners)
{
    // results represent which learner fired (1) and which were inhibited (0)
    Mat1i winners = Mat1i::zeros(1, static_cast<int>(learners.size()));

    // time to spike or still in refractory period
    if(next_spike_time_sec_ < delta_t_sec_) { // time to spike

        // Distribution of learner states
        Mat1f soft_max = LearnerStateDistr(learners);

        Sampler1D sampler;
        sampler.pdf(soft_max);

        winners(sampler.Sample()) = 1;

        NextSpikeTime();
    }
    else { // refractory period

        next_spike_time_sec_ -= delta_t_sec_;
    }

    return winners > 0;
}

Mat WTAPoisson::LearnerStateDistr(const vector<shared_ptr<base_Learner> > &learners) const
{
    int nb_learners = static_cast<int>(learners.size());

    if(nb_learners < 1) {

        ELM_THROW_BAD_DIMS("Learner vector is empty.");
    }

    Mat1f u(1, nb_learners);
    for(int i=0; i<nb_learners; i++) {

        u(i) = learners[i]->State().at<float>(0);
    }

    // normalize learner state distribution
    Mat mean, s;
    meanStdDev(u, mean, s);

    u -= static_cast<float>(mean.at<double>(0));
    Mat1f soft_max;
    exp(u, soft_max);
    soft_max /= sum(soft_max)(0);

    return soft_max;
}


