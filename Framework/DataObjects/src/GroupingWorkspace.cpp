// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include <utility>

#include "MantidAPI/SpectraAxis.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/GroupingWorkspace.h"
#include "MantidKernel/IPropertyManager.h"
#include "MantidKernel/System.h"

using std::size_t;
using namespace Mantid::API;

namespace Mantid::DataObjects {
// Register the workspace
DECLARE_WORKSPACE(GroupingWorkspace)

/** Constructor, buiding with give dimensions
 * @param numvectors: input size of the vector/histogram number for this
 * workspace
 * @return created GroupingWorkspace
 */
GroupingWorkspace::GroupingWorkspace(size_t numvectors) { this->init(numvectors, 1, 1); }

//----------------------------------------------------------------------------------------------
/** Constructor, building from an instrument
 *
 * @param inst :: input instrument that is the base for this workspace
 * @return created GroupingWorkspace
 */
GroupingWorkspace::GroupingWorkspace(const Geometry::Instrument_const_sptr &inst) : SpecialWorkspace2D(inst) {}

//----------------------------------------------------------------------------------------------
/** Fill a map with key = detector ID, value = group number
 * by using the values in Y.
 * Group values of 0 are converted to -1.
 *
 * @param detIDToGroup :: ref. to map to fill
 * @param[out] ngroups :: the number of groups found (equal to the largest group
 *number found)
 */
void GroupingWorkspace::makeDetectorIDToGroupMap(std::map<detid_t, int> &detIDToGroup, int64_t &ngroups) const {
  ngroups = 0;
  for (size_t wi = 0; wi < getNumberHistograms(); ++wi) {
    // Convert the Y value to a group number
    auto group = static_cast<int>(this->readY(wi)[0]);
    if (group == 0)
      group = -1;
    auto detIDs = this->getDetectorIDs(wi);
    for (auto detID : detIDs) {
      detIDToGroup[detID] = group;
      if (group > ngroups)
        ngroups = group;
    }
  }
}

/**
 * Fill a map where the index is detector ID and the value is the
 * group number by using the values in Y.
 * Group values of 0 are converted to -1.
 *
 * @param detIDToGroup :: ref. to map to fill
 * @param[out] ngroups :: the number of groups found (equal to the largest group
 *number found)
 */
void GroupingWorkspace::makeDetectorIDToGroupVector(std::vector<int> &detIDToGroup, int64_t &ngroups) const {
  ngroups = 0;
  for (size_t wi = 0; wi < getNumberHistograms(); ++wi) {
    // Convert the Y value to a group number
    auto group = static_cast<int>(this->y(wi).front());
    if (group == 0)
      group = -1;
    auto detIDs = this->getDetectorIDs(wi);
    for (auto detID : detIDs) {
      // if you need negative detector ids, use the other function
      if (detID < 0)
        continue;
      if (detIDToGroup.size() < static_cast<size_t>(detID + 1))
        detIDToGroup.resize(detID + 1);
      detIDToGroup[detID] = group;
      if (group > ngroups)
        ngroups = group;
    }
  }
}

} // namespace Mantid::DataObjects

///\cond TEMPLATE

namespace Mantid::Kernel {

template <>
DLLExport Mantid::DataObjects::GroupingWorkspace_sptr
IPropertyManager::getValue<Mantid::DataObjects::GroupingWorkspace_sptr>(const std::string &name) const {
  auto *prop =
      dynamic_cast<PropertyWithValue<Mantid::DataObjects::GroupingWorkspace_sptr> *>(getPointerToProperty(name));
  if (prop) {
    return *prop;
  } else {
    std::string message =
        "Attempt to assign property " + name + " to incorrect type. Expected shared_ptr<GroupingWorkspace>.";
    throw std::runtime_error(message);
  }
}

template <>
DLLExport Mantid::DataObjects::GroupingWorkspace_const_sptr
IPropertyManager::getValue<Mantid::DataObjects::GroupingWorkspace_const_sptr>(const std::string &name) const {
  auto *prop =
      dynamic_cast<PropertyWithValue<Mantid::DataObjects::GroupingWorkspace_sptr> *>(getPointerToProperty(name));
  if (prop) {
    return prop->operator()();
  } else {
    std::string message =
        "Attempt to assign property " + name + " to incorrect type. Expected const shared_ptr<GroupingWorkspace>.";
    throw std::runtime_error(message);
  }
}

} // namespace Mantid::Kernel

///\endcond TEMPLATE
