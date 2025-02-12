=========================
Indirect Geometry Changes
=========================

.. contents:: Table of Contents
   :local:

New Features
------------
- A new set of scripts has been produced to provide a method for processing OSIRIS data. :ref:`isis-powder-diffraction-osiris-ref`.
- Three new fitting functions: :ref:`FickDiffusionSQE <func-FickDiffusionSQE>`, :ref:`ChudleyElliotSQE <func-Chudley-ElliotSQE>`, and :ref:`HallRossSQE <func-Hall-RossSQE>` have been made and added to ConvFit.
- The :ref:`I(q,t) <iqt>` tab in Indirect Data Analysis can now be ran with direct data.
- Updated :ref:`SimpleShapeDiscusInelastic <algm-SimpleShapeDiscusInelastic>` workflow algorithm to add support for containers in line with the enhancements made to :ref:`DiscusMultipleScatteringCorrection <algm-DiscusMultipleScatteringCorrection>`.


Bugfixes
--------
- Fixed a bug where clicking 'Run' on the :ref:`Apply Absorption Corrections<indirect_apply_absorp_correct>` Tab in the :ref:`Corrections<interface-indirect-corrections>` GUI with no Sample or Corrections would close mantid.
- Fixed a bug where if the Corrections Workspace name entered on the :ref:`Apply Absorption Corrections<indirect_apply_absorp_correct>` Tab does not match an existing workspace, Mantid would close.
- Fixed a bug in :ref:`Indirect Data Reduction <interface-indirect-data-reduction>` where the spectra in the detector table started at 0. The spectra now start at 1.
- Fixed a bug in the Indirect Data Analysis :ref:`F(Q) fit <fqfit>` tab where, when loading a file in the workspace selector, if the parameter type was changed before the workspace was finished loading it would crash Mantid.
- Fixed a bug in the Indirect Data Analysis :ref:`F(Q) fit <fqfit>` tab where, when adding data to the interface, if the parameter type was changed when no data was selected it would crash Mantid.


Algorithms
----------

New features
############


Bugfixes
############


:ref:`Release 6.5.0 <v6.5.0>`