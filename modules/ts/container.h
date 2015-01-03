/** Assertions around common container classes
  */
#ifndef SEM_TS_CONTAINER_H_
#define SEM_TS_CONTAINER_H_

#include "gtest/gtest.h"

#include <vector>

#include <opencv2/core.hpp>

/**
  Assert that container is empty
  */
template <class T>
::testing::AssertionResult Empty(const std::vector< T > &container)
{
    if(container.empty()) { return ::testing::AssertionSuccess(); }
    else {
        return ::testing::AssertionFailure()
                << "Container is not empty and contains"
                << container.size() << " elements.";
    }
}
#define EXPECT_EMPTY(x) EXPECT_TRUE( Empty(x) )

#define EXPECT_SIZE(sz, container) EXPECT_EQ(size_t(sz), container.size())

#endif // SEM_TS_CONTAINER_H_