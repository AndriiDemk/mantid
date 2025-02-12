# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
'''
Notes:

From the command line:
$ cd ~/git/mantid/Code/Mantid/scripts/Interface
$ python reduction_application.py
It uses:
.config/Mantid/Mantid Reduction.conf
From mantidplot:
.config/Mantid/MantidPlot.conf
'''

from qtpy.QtCore import (QObject, QSettings, Signal)  # noqa

unicode = str


class GeneralSettings(QObject):
    """
        Settings class that will be passed from the main application
        to the control widgets.
    """
    # Properties to be saved in the QSettings file
    _data_path = '.'
    _debug = False
    _advanced = True
    _last_data_ws = ''
    _last_file = ''
    _instrument_name = ''
    _facility_name = ''
    _data_output_dir = None
    _catalog_data_path = '.'

    # User information for remote submission
    cluster_user = None
    cluster_pass = None
    compute_resource = "Fermi"

    # Mantid Python API version
    api2 = True
    data_updated = Signal('PyQt_PyObject','PyQt_PyObject')
    progress = Signal(int)

    def __init__(self, settings=None):
        """
            Initialization.
            @param settings: QSettings object passed by the main application
        """
        super(GeneralSettings, self).__init__()
        if settings is not None:
            self._settings = settings
        else:
            self._settings = QSettings()

    def emit_key_value(self, key, value):
        """
            Emit a signal to alert listeners of key/value update
        """
        self.data_updated.emit(key, value)

    @property
    def data_path(self):
        self._data_path = self._settings.value("general_data_path", '.', type=unicode)
        return self._data_path

    @data_path.setter
    def data_path(self, val):
        val = str(val)
        self._settings.setValue("general_data_path", val)
        self._data_path = val

    @property
    def debug(self):
        self._debug = self._settings.value("debug_mode", False, type=bool)
        return self._debug

    @debug.setter
    def debug(self, val):
        val = str(val)
        self._settings.setValue("debug_mode", val)
        self._debug = val

    @property
    def advanced(self):
        self._advanced = self._settings.value("advanced_mode", True, type=bool)
        return self._advanced

    @advanced.setter
    def advanced(self, val):
        val = str(val)
        self._settings.setValue("advanced_mode", val)
        self._advanced = val

    @property
    def instrument_name(self):
        """ Get instrument name
        :return: instrument name or False
        """
        self._instrument_name = self._settings.value("instrument_name", "", type=unicode)
        if self._instrument_name == "":
            self._instrument_name = False
        return self._instrument_name

    @instrument_name.setter
    def instrument_name(self, val):
        val = str(val)
        self._settings.setValue("instrument_name", val)
        self._instrument_name = val

    @property
    def facility_name(self):
        self._facility_name = self._settings.value("facility_name", "", type=unicode)
        if self._facility_name == "":
            self._facility_name = False
        return self._facility_name

    @facility_name.setter
    def facility_name(self, val):
        val = str(val)
        self._settings.setValue("facility_name", val)
        self._facility_name = val

    @property
    def data_output_dir(self):
        self._data_output_dir = self._settings.value("data_output_dir", True, type=bool)
        return self._data_output_dir

    @data_output_dir.setter
    def data_output_dir(self, val):
        val = str(val)
        self._settings.setValue("data_output_dir", val)
        self._data_output_dir = val

    @property
    def catalog_data_path(self):
        self._catalog_data_path = self._settings.value("catalog_data_path", self.data_path, type=unicode)
        return self._catalog_data_path

    @catalog_data_path.setter
    def catalog_data_path(self, val):
        val = str(val)
        self._settings.setValue("catalog_data_path", val)
        self._data_path = val
