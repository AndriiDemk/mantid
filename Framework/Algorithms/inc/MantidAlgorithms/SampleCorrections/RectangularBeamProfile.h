// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidAlgorithms/DllConfig.h"
#include "MantidAlgorithms/SampleCorrections/IBeamProfile.h"
#include "MantidKernel/V3D.h"

#include <array>

namespace Mantid {
namespace Geometry {
class ReferenceFrame;
}
namespace Algorithms {

/**
  Defines a flat, rectangular beam profile that has a width, height and center
  point. The profile is assumed infinitely thin.
*/
class MANTID_ALGORITHMS_DLL RectangularBeamProfile final : public IBeamProfile {
public:
  RectangularBeamProfile(const Geometry::ReferenceFrame &frame, const Kernel::V3D &center, double width, double height);

  IBeamProfile::Ray generatePoint(Kernel::PseudoRandomNumberGenerator &rng) const override;
  IBeamProfile::Ray generatePoint(Kernel::PseudoRandomNumberGenerator &rng,
                                  const Geometry::BoundingBox &bounds) const override;
  Geometry::BoundingBox defineActiveRegion(const Geometry::BoundingBox &) const override;
  /// Returns the min point of the profile
  inline Kernel::V3D minPoint() const { return Kernel::V3D{m_min[0], m_min[1], m_min[2]}; }
  /// Returns the max point of the profile
  inline Kernel::V3D maxPoint() const {
    auto maxPt = minPoint();
    maxPt[m_horIdx] += m_width;
    maxPt[m_upIdx] += m_height;
    return maxPt;
  }

private:
  const unsigned short m_upIdx;
  const unsigned short m_beamIdx;
  const unsigned short m_horIdx;
  const double m_width;
  const double m_height;
  /*std::array<double, 3>*/ Kernel::V3D m_min;
  Kernel::V3D m_beamDir;
};

} // namespace Algorithms
} // namespace Mantid
