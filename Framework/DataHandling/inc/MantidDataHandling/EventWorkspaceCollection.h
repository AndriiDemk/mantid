// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidDataHandling/DllConfig.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidKernel/System.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include <boost/function.hpp>
#include <memory>
#include <vector>

namespace Mantid {
namespace Indexing {
class IndexInfo;
}
namespace DataHandling {

/** EventWorkspaceCollection : Collection of EventWorspaces to give
backward-forward compatibility
 around performing operations on groups. Behave similar to an EventWorkspace
with some some additional new functionality.
Original purpose to support LoadEventNexus for the MultiPeriod cases.
*/
class DLLExport EventWorkspaceCollection {

private:
  /// Vector of EventWorkspaces
  std::vector<DataObjects::EventWorkspace_sptr> m_WsVec;
  /// Create Empty EventWorkspaces
  DataObjects::EventWorkspace_sptr createEmptyEventWorkspace() const;

public:
  EventWorkspaceCollection();
  EventWorkspaceCollection(const EventWorkspaceCollection &other) = delete;
  EventWorkspaceCollection &operator=(const EventWorkspaceCollection &other) = delete;
  virtual ~EventWorkspaceCollection() = default;

  void setNPeriods(size_t nPeriods, std::unique_ptr<const Kernel::TimeSeriesProperty<int>> &periodLog);
  void reserveEventListAt(size_t wi, size_t size);
  size_t nPeriods() const;
  DataObjects::EventWorkspace_sptr getSingleHeldWorkspace();
  API::Workspace_sptr combinedWorkspace();
  const DataObjects::EventList &getSpectrum(const size_t workspace_index, const size_t periodNumber) const;
  DataObjects::EventList &getSpectrum(const size_t workspace_index, const size_t periodNumber);
  void setGeometryFlag(const int flag);
  void setThickness(const float flag);
  void setHeight(const float flag);
  void setWidth(const float flag);
  void setSpectrumNumbersFromUniqueSpectra(const std::set<int> &uniqueSpectra);
  void setSpectrumNumberForAllPeriods(const size_t spectrumNumber, const specnum_t specid);
  void setDetectorIdsForAllPeriods(const size_t spectrumNumber, const detid_t id);

  Geometry::Instrument_const_sptr getInstrument() const;
  const API::Run &run() const;
  API::Run &mutableRun();
  API::Sample &mutableSample();
  DataObjects::EventList &getSpectrum(const size_t index);
  const DataObjects::EventList &getSpectrum(const size_t index) const;
  Mantid::API::Axis *getAxis(const size_t &i) const;
  size_t getNumberHistograms() const;

  std::vector<size_t> getSpectrumToWorkspaceIndexVector(Mantid::specnum_t &offset) const;

  std::vector<size_t> getDetectorIDToWorkspaceIndexVector(Mantid::specnum_t &offset, bool dothrow) const;
  Types::Core::DateAndTime getFirstPulseTime() const;
  void setAllX(const HistogramData::BinEdges &x);
  size_t getNumberEvents() const;
  void setIndexInfo(const Indexing::IndexInfo &indexInfo);
  void setInstrument(const Geometry::Instrument_const_sptr &inst);
  void setMonitorWorkspace(const std::shared_ptr<API::MatrixWorkspace> &monitorWS);
  void updateSpectraUsing(const API::SpectrumDetectorMapping &map);
  void setTitle(const std::string &title);
  void applyFilterInPlace(const boost::function<void(API::MatrixWorkspace_sptr)> &func);
  void applyFilter(const boost::function<DataObjects::EventWorkspace_sptr(DataObjects::EventWorkspace_sptr)> &func);
  virtual bool threadSafe() const;
};

using EventWorkspaceCollection_sptr = std::shared_ptr<EventWorkspaceCollection>;
using EventWorkspaceCollection_uptr = std::unique_ptr<EventWorkspaceCollection>;

} // namespace DataHandling
} // namespace Mantid
