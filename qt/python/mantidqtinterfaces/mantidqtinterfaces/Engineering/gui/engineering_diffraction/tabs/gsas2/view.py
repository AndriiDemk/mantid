# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2022 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from qtpy import QtWidgets, QtCore
from qtpy.QtGui import QRegExpValidator
from mantidqt.utils.qt import load_ui

Ui_calib, _ = load_ui(__file__, "gsas2_tab.ui")


class GSAS2View(QtWidgets.QWidget, Ui_calib):
    sig_enable_controls = QtCore.Signal(bool)
    sig_update_sample_field = QtCore.Signal()

    def __init__(self, parent=None, instrument="ENGINX"):
        super(GSAS2View, self).__init__(parent)
        self.setupUi(self)
        one_many_int_float_comma_separated = \
            QRegExpValidator(QtCore.QRegExp(r"^(?:\d+(?:\.\d*)?|\.\d+)(?:,(?:\d+(?:\.\d*)?|\.\d+))*$"),
                             self.override_unitcell_length)
        self.override_unitcell_length.setValidator(one_many_int_float_comma_separated)

    def set_refine_clicked(self, slot):
        self.refine_button.clicked.connect(slot)

    def get_refinement_parameters(self):
        return [self.refinement_method_combobox.currentText(), self.override_unitcell_length.text(),
                self.refine_microstrain_checkbox.isChecked(), self.refine_sigma_one_checkbox.isChecked(),
                self.refine_gamma_y_checkbox.isChecked()]
