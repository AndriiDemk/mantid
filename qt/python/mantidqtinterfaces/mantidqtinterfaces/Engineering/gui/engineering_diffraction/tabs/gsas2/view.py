# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2022 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from qtpy import QtWidgets, QtCore

from mantidqt.utils.qt import load_ui

Ui_calib, _ = load_ui(__file__, "gsas2_tab.ui")


class GSAS2View(QtWidgets.QWidget, Ui_calib):
    sig_enable_controls = QtCore.Signal(bool)
    sig_update_sample_field = QtCore.Signal()

    def __init__(self, parent=None, instrument="ENGINX"):
        super(GSAS2View, self).__init__(parent)
        self.setupUi(self)

    def set_refine_clicked(self, slot):
        self.Refine.clicked.connect(slot)
