// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2022 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +

#pragma once

#include "../../../ISISReflectometry/Reduction/LookupTable.h"
#include "../../../ISISReflectometry/Reduction/PreviewRow.h"
#include "TestHelpers/ModelCreationHelper.h"
#include <cxxtest/TestSuite.h>

using namespace MantidQt::CustomInterfaces::ISISReflectometry;

class LookupTableTest : public CxxTest::TestSuite {
public:
  void test_searching_by_theta_found() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    for (const auto angle : {0.5, 2.3}) {
      auto row = ModelCreationHelper::makeRow(angle);
      const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
      assertLookupRowAngle(lookupRow, angle);
    }
  }

  void test_searching_by_theta_found_for_preview_row() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    for (const auto angle : {0.5, 2.3}) {
      auto row = makePreviewRow(angle);
      const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
      assertLookupRowAngle(lookupRow, angle);
    }
  }

  void test_searching_by_theta_tolerance_found() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    const double matchTolerance = 0.01;
    for (const auto angle : {(0.5 - matchTolerance), (2.3 + matchTolerance)}) {
      auto row = ModelCreationHelper::makeRow(angle);
      const auto lookupRow = table.findLookupRow(row, matchTolerance);
      assertLookupRowAngle(lookupRow, angle, matchTolerance);
    }
  }

  void test_searching_by_theta_tolerance_found_for_preview_row() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    const double matchTolerance = 0.01;
    for (const auto angle : {(0.5 - matchTolerance), (2.3 + matchTolerance)}) {
      auto row = makePreviewRow(angle);
      const auto lookupRow = table.findLookupRow(row, matchTolerance);
      assertLookupRowAngle(lookupRow, angle, matchTolerance);
    }
  }

  void test_searching_by_theta_not_found_returns_wildcard() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    for (const auto angle : {1.2, 3.4}) {
      auto row = ModelCreationHelper::makeRow(angle);
      const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
      TS_ASSERT(lookupRow)
      const auto foundAngle = lookupRow->thetaOrWildcard();
      TS_ASSERT(!foundAngle)
      TS_ASSERT(lookupRow->isWildcard());
    }
  }

  void test_searching_by_theta_not_found_returns_wildcard_for_preview_row() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAnglesAndWildcard();

    for (const auto angle : {1.2, 3.4}) {
      auto row = makePreviewRow(angle);
      const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
      TS_ASSERT(lookupRow)
      const auto foundAngle = lookupRow->thetaOrWildcard();
      TS_ASSERT(!foundAngle)
      TS_ASSERT(lookupRow->isWildcard());
    }
  }

  void test_searching_by_theta_not_found_returns_none() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAngles();

    constexpr double notThere = 999;
    auto row = ModelCreationHelper::makeRow(notThere);
    const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
    TS_ASSERT(!lookupRow)
  }

  void test_searching_by_theta_not_found_returns_none_for_preview_row() {
    LookupTable table = ModelCreationHelper::makeLookupTableWithTwoAngles();

    constexpr double notThere = 999;
    auto row = makePreviewRow(notThere);
    const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
    TS_ASSERT(!lookupRow)
  }

  void test_searching_empty_table_returns_none() {
    LookupTable table = ModelCreationHelper::makeEmptyLookupTable();

    constexpr double notThere = 0.5;
    auto row = ModelCreationHelper::makeRow(notThere);
    const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
    TS_ASSERT(!lookupRow)
  }

  void test_searching_empty_table_returns_none_for_preview_row() {
    LookupTable table = ModelCreationHelper::makeEmptyLookupTable();

    constexpr double notThere = 0.5;
    auto row = makePreviewRow(notThere);
    const auto lookupRow = table.findLookupRow(row, m_exactMatchTolerance);
    TS_ASSERT(!lookupRow)
  }

  void test_searching_by_theta_and_title_found() {
    auto constexpr angle = 2.3;
    auto expectedLookupRow = ModelCreationHelper::makeLookupRow(angle, boost::regex("El"));
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")), expectedLookupRow};

    auto group = Group("El Em En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    if (foundLookupRow)
      TS_ASSERT_EQUALS(*foundLookupRow, expectedLookupRow)
  }

  void test_searching_by_theta_and_title_found_with_wildcard_present() {
    auto constexpr angle = 2.3;
    auto expectedLookupRow = ModelCreationHelper::makeLookupRow(angle, boost::regex("El"));
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")), expectedLookupRow,
                             ModelCreationHelper::makeWildcardLookupRow()};

    auto group = Group("El Em En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    if (foundLookupRow)
      TS_ASSERT_EQUALS(*foundLookupRow, expectedLookupRow)
  }

  void test_searching_by_theta_found_but_title_not_found_returns_none() {
    auto constexpr angle = 2.3;
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El"))};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(!foundLookupRow)
  }

  void test_searching_by_title_found_but_theta_not_found_returns_none() {
    auto constexpr angle = 2.3;
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El"))};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(!foundLookupRow)
  }

  void test_searching_by_theta_found_but_title_not_found_returns_wildcard() {
    auto constexpr angle = 2.3;
    auto wildcardRow = ModelCreationHelper::makeWildcardLookupRow();
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El")), wildcardRow};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(wildcardRow, foundLookupRow)
  }

  void test_searching_by_title_found_but_theta_not_found_returns_wildcard() {
    auto constexpr angle = 2.3;
    auto wildcardRow = ModelCreationHelper::makeWildcardLookupRow();
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El")), wildcardRow};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(wildcardRow, foundLookupRow)
  }

  void test_searching_by_title_matches_regex_over_wildcard() {
    auto constexpr angle = 2.3;
    auto wildcardRow = ModelCreationHelper::makeWildcardLookupRow();
    auto regexRow = ModelCreationHelper::makeLookupRow(angle, boost::regex(".*"));
    auto table = LookupTable{wildcardRow, regexRow};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(regexRow, foundLookupRow)
  }

  void test_searching_by_title_matches_empty_regex() {
    auto constexpr angle = 2.3;
    auto emptyRegexRow = ModelCreationHelper::makeLookupRow(angle, boost::none);
    auto table = LookupTable{emptyRegexRow};

    auto group = Group("En Oh", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(emptyRegexRow, foundLookupRow)
  }

  void test_empty_title_matches_only_empty_regex() {
    auto constexpr angle = 2.3;
    auto emptyRegexRow = ModelCreationHelper::makeLookupRow(angle, boost::none);
    auto regexRow = ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay"));
    auto table = LookupTable{emptyRegexRow, regexRow};

    auto group = Group("", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow);
    TS_ASSERT_EQUALS(emptyRegexRow, foundLookupRow);
  }

  void test_get_wildcard_row_returns_wildcard_row() {
    auto constexpr angle = 2.3;
    auto wildcardRow = ModelCreationHelper::makeWildcardLookupRow();
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El")), wildcardRow};

    const auto foundLookupRow = table.findWildcardLookupRow();
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(wildcardRow, foundLookupRow)
  }

  void test_get_wildcard_row_returns_none() {
    auto constexpr angle = 2.3;
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("Ay")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("El"))};

    const auto foundLookupRow = table.findWildcardLookupRow();
    TS_ASSERT(!foundLookupRow)
  }

  void test_searching_with_no_matching_title_but_matching_theta_with_matching_title_present() {
    auto angle = 0.7;
    auto regexRow = ModelCreationHelper::makeLookupRow(2.3, boost::regex("Ay"));
    auto nonRegexRow = ModelCreationHelper::makeLookupRow(angle, boost::none);
    auto table = LookupTable{regexRow, nonRegexRow};

    auto group = Group("Ay Bee", {ModelCreationHelper::makeRow(angle)});
    const auto foundLookupRow = table.findLookupRow(*group[0], m_exactMatchTolerance);
    TS_ASSERT(foundLookupRow)
    TS_ASSERT_EQUALS(nonRegexRow, foundLookupRow)
  }

  void test_multiple_row_title_matches_are_invalid() {
    auto constexpr angle = 2.3;
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex("A.*")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("AA.*"))};

    auto group = Group("AAA", {ModelCreationHelper::makeRow(angle)});
    TS_ASSERT_THROWS(table.findLookupRow(*group[0], m_exactMatchTolerance), MultipleRowsFoundException const &)
  }

  void test_get_index_for_lookup_row() {
    auto constexpr angle = 2.3;
    auto const lookupRow = ModelCreationHelper::makeLookupRow(angle, boost::regex("A.*"));
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex(".*")), lookupRow,
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("AA.*"))};
    TS_ASSERT_EQUALS(table.getIndex(lookupRow), 1);
  }

  void test_get_index_for_missing_lookup_row() {
    auto constexpr angle = 2.3;
    auto const lookupRow = ModelCreationHelper::makeLookupRow(angle, boost::regex("A.*"));
    auto table = LookupTable{ModelCreationHelper::makeLookupRow(angle, boost::regex(".*")),
                             ModelCreationHelper::makeLookupRow(angle, boost::regex("AA.*"))};
    TS_ASSERT_THROWS(table.getIndex(lookupRow), std::out_of_range const &);
  }

private:
  constexpr static const double m_exactMatchTolerance = 1e-6;

  void assertLookupRowAngle(boost::optional<LookupRow> lookupRow, double expected,
                            double match_tolerance = m_exactMatchTolerance) {
    TS_ASSERT(lookupRow)
    if (lookupRow) {
      const auto foundAngle = lookupRow->thetaOrWildcard();
      TS_ASSERT(foundAngle)
      if (foundAngle) {
        TS_ASSERT_DELTA(*foundAngle, expected, match_tolerance)
      }
    }
  }

  PreviewRow makePreviewRow(double theta) {
    auto previewRow = PreviewRow(std::vector<std::string>{"12345"});
    previewRow.setTheta(theta);
    return previewRow;
  }
};
