# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
import unittest
import numpy as np
from unittest import mock
from mantid.simpleapi import CreateWorkspace
from mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.context import ElementalAnalysisContext, REBINNED_VARIABLE_WS_SUFFIX
from mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.data_context import DataContext
from mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.ea_group_context import EAGroupContext
from mantidqtinterfaces.Muon.GUI.Common.contexts.muon_gui_context import MuonGuiContext
from mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.ea_group import EAGroup
from mantidqtinterfaces.Muon.GUI.Common.ADSHandler.ADS_calls import retrieve_ws, remove_ws_if_present


class ElementalAnalysisContextTest(unittest.TestCase):
    def setUp(self):
        self.context = ElementalAnalysisContext(data_context=DataContext(), ea_group_context=EAGroupContext(),
                                                muon_gui_context=MuonGuiContext)

    def assert_workspace_equal(self, workspace1, workspace2):
        self.assertEqual(workspace1.getNumberHistograms(), workspace2.getNumberHistograms())
        for i in range(workspace1.getNumberHistograms()):
            self.assertTrue(np.array_equal(workspace1.readX(i), workspace2.readX(i)))
            self.assertTrue(np.array_equal(workspace1.readY(i), workspace2.readY(i)))
            self.assertTrue(np.array_equal(workspace1.readE(i), workspace2.readE(i)))

    def assert_context_empty(self):
        self.assertEqual(len(self.context.data_context.current_runs), 0)
        self.assertEqual(len(self.context.group_context.groups), 0)
        self.assertEqual(len(self.context.data_context._loaded_data.params), 0)

    # ------------------------------------------------------------------------------------------------------------------
    # TESTS
    # ------------------------------------------------------------------------------------------------------------------

    def test_name(self):
        self.assertEqual(self.context.name, "Elemental Analysis 2")

    @mock.patch('mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.context.retrieve_ws')
    @mock.patch('mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.context.remove_ws_if_present')
    @mock.patch('mantidqtinterfaces.Muon.GUI.ElementalAnalysis2.context.ea_group_context.EAGroupContext.__getitem__')
    def test_rebin(self, mock_get_item, mock_remove_ws, mock_retrieve_ws):
        mock_get_item.return_value = EAGroup("9999; Detector 1", "detector 1", "9999")
        name = '9999; Detector 1'
        rebinned_name = '9999; Detector 1' + REBINNED_VARIABLE_WS_SUFFIX
        mock_params = "0, 2, 9"

        x_data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        y_data = [1, 1, 1, 1, 1, 1, 1, 1, 1]

        CreateWorkspace(OutputWorkspace=name, DataX=x_data, DataY=y_data)
        self.context._run_rebin("9999; Detector 1", "Variable", mock_params)

        correct_data = CreateWorkspace(OutputWorkspace="correct_data", DataX=[0, 2, 4, 6, 8, 9], DataY=[2, 2, 2, 2, 1])

        # Assert Statements
        self.assert_workspace_equal(correct_data, retrieve_ws(rebinned_name))
        mock_remove_ws.assert_has_calls([mock.call(rebinned_name)])
        mock_retrieve_ws.assert_has_calls([mock.call("9999")])
        mock_get_item.assert_has_calls([mock.call(name)])

        # clean up
        remove_ws_if_present(name)
        remove_ws_if_present("correct_data")
        remove_ws_if_present(rebinned_name)

    def test_clear_group(self):
        # setup
        self.assert_context_empty()
        self.context.data_context.current_runs.append("mock_run_1")
        self.context.data_context.current_runs.append("mock_run_2")
        self.context.group_context.add_group(EAGroup("9999; Detector 1", "detector 1", "9999"))
        self.context.group_context.add_group(EAGroup("9999; Detector 2", "detector 2", "9999"))
        self.context.data_context._loaded_data.add_data(workspace="mock_workspace_1")
        self.context.data_context._loaded_data.add_data(workspace="mock_workspace_2")

        # check if context is not empty
        with self.assertRaises(AssertionError):
            self.assert_context_empty()

        # call clear function
        self.context.clear_context()

        # check context is empty
        self.assert_context_empty()

    @mock.patch("mantidqt.utils.observer_pattern.GenericObservable.notify_subscribers")
    def test_remove_workspace_with_a_string(self, mock_notify_subscirbers):
        mock_group = EAGroup("mock_workspace", "detector 1", "9999")
        self.context.group_context.add_group(mock_group)
        self.context.group_context.remove_group = mock.Mock()
        # call remove_workspace function
        self.context.remove_workspace("mock_workspace")

        # assert statement
        mock_notify_subscirbers.assert_has_calls([mock.call("mock_workspace"), mock.call("mock_workspace")])
        self.context.group_context.remove_group.assert_called_once_with("mock_workspace")

    @mock.patch("mantidqt.utils.observer_pattern.GenericObservable.notify_subscribers")
    def test_remove_workspace_with_a_workspace(self, mock_notify_subscirbers):
        # setup
        self.context.group_context.remove_group = mock.Mock()
        mock_group = EAGroup("mock_workspace", "detector 1", "9999")
        self.context.group_context.add_group(mock_group)
        mock_ws = CreateWorkspace(OutputWorkspace="mock_workspace", DataX=[0, 2, 4, 6, 8, 9], DataY=[2, 2, 2, 2, 1])

        # call remove_workspace function
        self.context.remove_workspace(mock_ws)

        # assert statement
        mock_notify_subscirbers.assert_has_calls([mock.call("mock_workspace"), mock.call(mock_ws)])
        self.context.group_context.remove_group.assert_called_once_with("mock_workspace")

    @mock.patch("mantidqt.utils.observer_pattern.GenericObservable.notify_subscribers")
    def test_remove_workspace_with_a_rebinned_workspace(self, mock_notify_subscirbers):
        self.context.group_context.remove_workspace_from_group = mock.Mock()
        # call remove_workspace function
        self.context.remove_workspace("rebinned_mock_workspace")

        # assert statement
        mock_notify_subscirbers.assert_has_calls([mock.call("rebinned_mock_workspace")])
        self.context.group_context.remove_workspace_from_group.assert_called_once_with("rebinned_mock_workspace")

    @mock.patch("mantidqt.utils.observer_pattern.GenericObservable.notify_subscribers")
    def test_remove_workspace_with_a_string_and_not_present_in_group(self, mock_notify_subscirbers):
        self.context.group_context.remove_workspace_from_group = mock.Mock()
        # call remove_workspace function
        self.context.remove_workspace("mock_workspace")

        # assert statement
        mock_notify_subscirbers.assert_has_calls([mock.call("mock_workspace"), mock.call("mock_workspace")])
        self.context.group_context.remove_workspace_from_group.assert_called_once_with("mock_workspace")

    def test_update_current_data_with_empty_data_context(self):
        # check if data context is empty
        self.assertEqual(len(self.context.data_context.current_runs), 0)
        self.context.data_context.clear = mock.Mock()

        # call update_current_data
        self.context.update_current_data()

        self.context.data_context.clear.assert_called_once()

    def test_update_current_data_with_empty_group_context(self):
        # check if group context is empty
        self.assertEqual(len(self.context.group_context.groups), 0)
        self.context.group_context.reset_group_to_default = mock.Mock()

        # add run to data context
        self.context.data_context.current_runs.append("mock_run_1")

        # call update_current_data
        self.context.update_current_data()

        # assert statement
        self.context.group_context.reset_group_to_default.assert_called_once_with(
            self.context.data_context._loaded_data)

    def test_update_current_data_with_populated_group_context(self):
        self.context.group_context.add_new_group = mock.Mock()
        # add run to data context and group to group context
        self.context.data_context.current_runs.append("mock_run_1")
        self.context.group_context.groups.append("mock_group")

        # call update_current_data
        self.context.update_current_data()

        # assert statement
        self.context.group_context.add_new_group.assert_called_once_with(self.context.group_context.groups,
                                                                         self.context.data_context._loaded_data)


if __name__ == '__main__':
    unittest.main(buffer=False, verbosity=2)
