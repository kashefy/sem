#ifndef SEM_ENCODING_ORIENTATION_H_
#define SEM_ENCODING_ORIENTATION_H_

#include <core/typedefs.h>

namespace sem {

/**
 * @brief Compute single gabor kernel
 * Appropriate model for orientation selective V1 simple cells and edge detectors
 *
 * @param kernel radius, yields kernel of size (r*2+1, r*2+1)
 * @param sigma Std dev of gaussian envelope
 * @param theta orientation in radians
 * @param lambd wavelength of sinusoid
 * @param gamma spatial aspect ratio
 * @param ps phase offset in radians
 * @return kernel in 32-bit floats
 */
cv::Mat1f GaborKernel(int radius, float sigma, float theta_rad, float lambd, float gamma, float ps);


} // namespace sem

#endif // SEM_ENCODING_ORIENTATION_H_
