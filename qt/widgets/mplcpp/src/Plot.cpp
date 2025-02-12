// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidQtWidgets/MplCpp/Plot.h"

#include "MantidAPI/MatrixWorkspace_fwd.h"
#include "MantidPythonInterface/core/CallMethod.h"
#include "MantidPythonInterface/core/Converters/ToPyList.h"
#include "MantidPythonInterface/core/GlobalInterpreterLock.h"
#include "MantidQtWidgets/Common/Python/Object.h"
#include "MantidQtWidgets/Common/Python/QHashToDict.h"
#include "MantidQtWidgets/Common/Python/Sip.h"
#include "MantidQtWidgets/MplCpp/Plot.h"
#include <utility>

using Mantid::API::MatrixWorkspace_sptr;
using namespace Mantid::PythonInterface;
using namespace MantidQt::Widgets::Common;

namespace MantidQt::Widgets::MplCpp {

namespace {

/**
 * @returns The mantidqt.plotting.functions module
 */
Python::Object functionsModule() { return Python::NewRef(PyImport_ImportModule("mantidqt.plotting.functions")); }

/**
 * Construct a Python list from a vector of strings
 * @param workspaces A strings
 * @return A new Python list object
 */
Python::Object constructArgs(const std::vector<std::string> &workspaces) {
  return Python::NewRef(Py_BuildValue("(O)", Converters::ToPyList<std::string>()(workspaces).ptr()));
}

/**
 * Construct a Python list from a vector of workspace pointers
 * @param workspaces A list of MatrixWorkspace_sptr
 * @return A new Python list object
 */
Python::Object constructArgs(const std::vector<MatrixWorkspace_sptr> &workspaces) {
  return Python::NewRef(Py_BuildValue("(O)", Converters::ToPyList<MatrixWorkspace_sptr>()(workspaces).ptr()));
}

/**
 * Construct a Python list from a QStringList
 * @param workspaces A list of strings
 * @return A new Python list object
 */
Python::Object constructArgs(const QStringList &workspaces) {
  const auto sipAPI = Python::Detail::sipAPI();
  const auto copy = new QStringList(workspaces);
  const auto *sobj = sipAPI->api_convert_from_new_type(copy, sipAPI->api_find_type("QStringList"), Py_None);
  return Python::NewRef(Py_BuildValue("(O)", sobj));
}

/**
 * Construct kwargs list for the plot function
 */
Python::Object constructKwargs(boost::optional<std::vector<int>> spectrumNums,
                               boost::optional<std::vector<int>> wkspIndices, boost::optional<Python::Object> fig,
                               boost::optional<QHash<QString, QVariant>> plotKwargs,
                               boost::optional<QHash<QString, QVariant>> axProperties,
                               boost::optional<std::string> windowTitle, boost::optional<bool> errors,
                               boost::optional<bool> overplot, boost::optional<bool> tiled) {
  // Make sure to decide whether spectrum numbers or workspace indices
  Python::Dict kwargs;

  if (spectrumNums && !wkspIndices) {
    kwargs["spectrum_nums"] = Converters::ToPyList<int>()(spectrumNums.get());
  } else if (wkspIndices && !spectrumNums) {
    kwargs["wksp_indices"] = Converters::ToPyList<int>()(wkspIndices.get());
  } else {
    throw std::invalid_argument("Passed spectrum numbers and workspace indices, please only pass one, "
                                "with the other being boost::none.");
  }

  if (errors)
    kwargs["errors"] = errors.get();
  if (overplot)
    kwargs["overplot"] = overplot.get();
  if (tiled)
    kwargs["tiled"] = tiled.get();
  if (fig)
    kwargs["fig"] = fig.get();
  if (plotKwargs)
    kwargs["plot_kwargs"] = Python::qHashToDict(plotKwargs.get());
  if (axProperties)
    kwargs["ax_properties"] = Python::qHashToDict(axProperties.get());
  if (windowTitle)
    kwargs["window_title"] = windowTitle.get();

  return std::move(kwargs);
}

Python::Object plot(const Python::Object &args, boost::optional<std::vector<int>> spectrumNums,
                    boost::optional<std::vector<int>> wkspIndices, boost::optional<Python::Object> fig,
                    boost::optional<QHash<QString, QVariant>> plotKwargs,
                    boost::optional<QHash<QString, QVariant>> axProperties, boost::optional<std::string> windowTitle,
                    bool errors, bool overplot, bool tiled) {
  const auto kwargs =
      constructKwargs(std::move(spectrumNums), std::move(wkspIndices), std::move(fig), std::move(plotKwargs),
                      std::move(axProperties), std::move(windowTitle), errors, overplot, tiled);
  try {
    return functionsModule().attr("plot")(*args, **kwargs);
  } catch (Python::ErrorAlreadySet &) {
    throw PythonException();
  }
}

} // namespace

Python::Object plot(const std::vector<std::string> &workspaces, boost::optional<std::vector<int>> spectrumNums,
                    boost::optional<std::vector<int>> wkspIndices, boost::optional<Python::Object> fig,
                    boost::optional<QHash<QString, QVariant>> plotKwargs,
                    boost::optional<QHash<QString, QVariant>> axProperties, boost::optional<std::string> windowTitle,
                    bool errors, bool overplot, bool tiled) {
  GlobalInterpreterLock lock;
  return plot(constructArgs(workspaces), std::move(spectrumNums), std::move(wkspIndices), std::move(fig),
              std::move(plotKwargs), std::move(axProperties), std::move(windowTitle), errors, overplot, tiled);
}

Python::Object plot(const QStringList &workspaces, boost::optional<std::vector<int>> spectrumNums,
                    boost::optional<std::vector<int>> wkspIndices, boost::optional<Python::Object> fig,
                    boost::optional<QHash<QString, QVariant>> plotKwargs,
                    boost::optional<QHash<QString, QVariant>> axProperties, boost::optional<std::string> windowTitle,
                    bool errors, bool overplot, bool tiled) {
  GlobalInterpreterLock lock;
  return plot(constructArgs(workspaces), std::move(spectrumNums), std::move(wkspIndices), std::move(fig),
              std::move(plotKwargs), std::move(axProperties), std::move(windowTitle), errors, overplot, tiled);
}

Python::Object plot(const std::vector<MatrixWorkspace_sptr> &workspaces, boost::optional<std::vector<int>> spectrumNums,
                    boost::optional<std::vector<int>> wkspIndices, boost::optional<Python::Object> fig,
                    boost::optional<QHash<QString, QVariant>> plotKwargs,
                    boost::optional<QHash<QString, QVariant>> axProperties, boost::optional<std::string> windowTitle,
                    bool errors, bool overplot, bool tiled) {
  GlobalInterpreterLock lock;
  return plot(constructArgs(workspaces), std::move(spectrumNums), std::move(wkspIndices), std::move(fig),
              std::move(plotKwargs), std::move(axProperties), std::move(windowTitle), errors, overplot, tiled);
}

Python::Object pcolormesh(const QStringList &workspaces, boost::optional<Python::Object> fig) {
  GlobalInterpreterLock lock;
  try {
    const auto args = constructArgs(workspaces);
    Python::Dict kwargs;
    if (fig)
      kwargs["fig"] = fig.get();
    return functionsModule().attr("pcolormesh")(*args, **kwargs);
  } catch (Python::ErrorAlreadySet &) {
    throw PythonException();
  }
}

} // namespace MantidQt::Widgets::MplCpp
