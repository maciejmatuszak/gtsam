/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    testAttitudeFactor.cpp
 * @brief   Unit test for AttitudeFactor
 * @author  Frank Dellaert
 * @date   January 22, 2014
 */

#include <gtsam/navigation/AttitudeFactor.h>
#include <gtsam/base/Testable.h>
#include <gtsam/base/numericalDerivative.h>
#include <CppUnitLite/TestHarness.h>

using namespace std;
using namespace gtsam;

// *************************************************************************
TEST( AttitudeFactor, Constructor ) {

  // Example: pitch and roll of aircraft in an ENU Cartesian frame.
  // If pitch and roll are zero for an aerospace frame,
  // that means Z is pointing down, i.e., direction of Z = (0,0,-1)
  Sphere2 bZ(0, 0, 1); // body Z axis
  Sphere2 nDown(0, 0, -1); // down, in ENU navigation frame

  // Factor
  Key key(1);
  SharedNoiseModel model = noiseModel::Isotropic::Sigma(3, 0.25);
  AttitudeFactor factor(key, bZ, nDown, model);

  // Create a linearization point at the zero-error point
  Pose3 T(Rot3(), Point3(-5.0, 8.0, -11.0));
  EXPECT(assert_equal(zero(2),factor.evaluateError(T),1e-5));

  // Calculate numerical derivatives
  Matrix expectedH = numericalDerivative11<Pose3>(
      boost::bind(&AttitudeFactor::evaluateError, &factor, _1, boost::none), T);

  // Use the factor to calculate the derivative
  Matrix actualH;
  factor.evaluateError(T, actualH);

  // Verify we get the expected error
  EXPECT(assert_equal(expectedH, actualH, 1e-8));
}

// *************************************************************************
int main() {
  TestResult tr;
  return TestRegistry::runAllTests(tr);
}
// *************************************************************************