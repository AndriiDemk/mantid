# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
import mantid.simpleapi as mantid

import isis_powder.routines.common as common
from isis_powder.routines.common_enums import INPUT_BATCHING


def create_van(instrument, run_details, absorb):
    """
    Creates a splined vanadium run for the following instrument. Requires the run_details for the
    vanadium workspace we will process and whether to apply absorption corrections.
    :param instrument: The instrument object that will be used to supply various instrument specific methods
    :param run_details: The run details associated with this vanadium run
    :param absorb: Boolean flag whether to apply absorption corrections
    :return: Processed workspace group in dSpacing (but not splined)
    """
    van = run_details.vanadium_run_numbers
    # Always sum a range of inputs as its a vanadium run over multiple captures
    input_van_ws_list = common.load_current_normalised_ws_list(run_number_string=van, instrument=instrument,
                                                               input_batching=INPUT_BATCHING.Summed)
    input_van_ws = input_van_ws_list[0]  # As we asked for a summed ws there should only be one returned

    instrument.create_solid_angle_corrections(input_van_ws, run_details)

    if not (run_details.empty_inst_runs is None):
        summed_empty_inst = common.generate_summed_runs(empty_sample_ws_string=run_details.empty_inst_runs,
                                                        instrument=instrument)
        mantid.SaveNexus(Filename=run_details.summed_empty_inst_file_path, InputWorkspace=summed_empty_inst)
        corrected_van_ws = common.subtract_summed_runs(ws_to_correct=input_van_ws, empty_sample=summed_empty_inst)

    # Crop the tail end of the data on PEARL if they are not capturing slow neutrons
    corrected_van_ws = instrument._crop_raw_to_expected_tof_range(ws_to_crop=corrected_van_ws)

    if absorb:
        corrected_van_ws = instrument._apply_absorb_corrections(run_details=run_details,
                                                                ws_to_correct=corrected_van_ws)
    else:
        # Assume that create_van only uses Vanadium runs
        mantid.SetSampleMaterial(InputWorkspace=corrected_van_ws, ChemicalFormula='V')

    mantid.ApplyDiffCal(InstrumentWorkspace=corrected_van_ws,
                        CalibrationFile=run_details.offset_file_path)
    aligned_ws = mantid.ConvertUnits(InputWorkspace=corrected_van_ws, Target="dSpacing")
    solid_angle = instrument.get_solid_angle_corrections(run_details.run_number, run_details)
    if solid_angle:
        aligned_ws = mantid.Divide(LHSWorkspace=aligned_ws,RHSWorkspace=solid_angle)
        mantid.DeleteWorkspace(solid_angle)
    focused_vanadium = mantid.DiffractionFocussing(InputWorkspace=aligned_ws,
                                                   GroupingFileName=run_details.grouping_file_path)
    # convert back to TOF based on engineered detector positions
    mantid.ApplyDiffCal(InstrumentWorkspace=focused_vanadium,
                        ClearCalibration=True)
    focused_spectra = common.extract_ws_spectra(focused_vanadium)
    focused_spectra = instrument._crop_van_to_expected_tof_range(focused_spectra)

    d_spacing_group, tof_group = instrument._output_focused_ws(processed_spectra=focused_spectra,
                                                               run_details=run_details)
    _create_vanadium_splines(focused_spectra, instrument, run_details)

    common.keep_single_ws_unit(d_spacing_group=d_spacing_group, tof_group=tof_group,
                               unit_to_keep=instrument._get_unit_to_keep())

    common.remove_intermediate_workspace(corrected_van_ws)
    common.remove_intermediate_workspace(aligned_ws)
    common.remove_intermediate_workspace(focused_vanadium)
    common.remove_intermediate_workspace(focused_spectra)

    return d_spacing_group


def _create_vanadium_splines(focused_spectra, instrument, run_details):
    splined_ws_list = instrument._spline_vanadium_ws(focused_spectra)
    out_spline_van_file_path = run_details.splined_vanadium_file_path
    append = False
    for ws in splined_ws_list:
        mantid.SaveNexus(Filename=out_spline_van_file_path, InputWorkspace=ws, Append=append)
        append = True
    # Group for user convenience
    group_name = "Van_spline_data"
    tt_mode = instrument._get_current_tt_mode()
    if tt_mode:
        group_name = group_name + '_' + tt_mode

    mantid.GroupWorkspaces(InputWorkspaces=splined_ws_list, OutputWorkspace=group_name)
