# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from mantid.kernel import VMD
import math
import unittest


class VMDTest(unittest.TestCase):

    def test_default_construction_gives_object_with_single_dimension(self):
        one = VMD()
        self.assertEqual(1, one.getNumDims())

    def test_constructors_with_dimension_pts(self):
        pts = [1]
        for i in range(2,7):
            pts.append(i)
            vector = VMD(*pts) #unpack list
            self.assertEqual(i, vector.getNumDims())

    def test_scalar_prod_returns_expected_value(self):
        a = VMD(1.0,2.0,1.0)
        b = VMD(1.0,-2.0,-1.0)
        sp = a.scalar_prod(b)
        self.assertAlmostEqual(sp,-4.0)

    def test_crossprod(self):
        a = VMD(1.0,0.0,0.0)
        b = VMD(0.0,1.0,0.0)
        c = a.cross_prod(b)
        self.assertAlmostEqual(c[0],0.0)
        self.assertAlmostEqual(c[1],0.0)
        self.assertAlmostEqual(c[2],1.0)

    def test_norm(self):
        p = VMD(1.0,-5.0,8.0);
        self.assertAlmostEqual(p.norm(), math.sqrt(90.0),places=6)

    def test_norm2(self):
        p = VMD(1.0,-5.0,8.0);
        self.assertAlmostEqual(p.norm2(), 90.0, places=6)

    def test_normalize(self):
        a = VMD(3,4, math.sqrt(39.0))
        pre_norm = a.norm()
        self.assertEqual(pre_norm, a.normalize())

        b = VMD(3./8,4./8, math.sqrt(39.0)/8.) # normalized version
        self.assertAlmostEqual(b[0], a[0], places=6)
        self.assertAlmostEqual(b[1], a[1], places=6)
        self.assertAlmostEqual(b[2], a[2], places=6)

    def test_angle(self):
        a = VMD(1,0,0);
        b = VMD(0,1,0);
        self.assertAlmostEqual(a.angle(b), math.pi/2, places=4);

    def test_value_read_access_succeeds_for_valid_indices(self):
        vector = VMD(1.0,2.0)
        self.assertAlmostEqual(1.0, vector[0])
        self.assertAlmostEqual(2.0, vector[1])

    def test_value_write_access_succeeds_for_valid_indices(self):
        vector = VMD(1.0,2.0)
        vector[0] = 1.5
        vector[1] = 1.6
        self.assertAlmostEqual(1.5, vector[0])
        self.assertAlmostEqual(1.6, vector[1])

    def test_standard_mathematical_operators(self):
        v1 = VMD(1.0,2.0)
        v2 = VMD(5.0,-1.0)

        v3 = v1 + v2
        self.assertAlmostEqual(6.0, v3[0])
        self.assertAlmostEqual(1.0, v3[1])
        v3 = v1 - v2
        self.assertAlmostEqual(-4.0, v3[0])
        self.assertAlmostEqual(3.0, v3[1])
        v3 = v1 * v2
        self.assertAlmostEqual(5.0, v3[0])
        self.assertAlmostEqual(-2.0, v3[1])
        v3 = v1 / v2
        self.assertAlmostEqual(1.0/5.0, v3[0])
        self.assertAlmostEqual(-2.0, v3[1])

    def test_inplace_mathematical_operators(self):
        v1 = VMD(1.0,2.0)
        v2 = VMD(5.0,-1.0)

        v1 += v2
        self.assertAlmostEqual(6.0, v1[0])
        self.assertAlmostEqual(1.0, v1[1])
        v1 = VMD(1.0,2.0)
        v1 -= v2
        self.assertAlmostEqual(-4.0, v1[0])
        self.assertAlmostEqual(3.0, v1[1])
        v1 = VMD(1.0,2.0)
        v1 *= v2
        self.assertAlmostEqual(5.0, v1[0])
        self.assertAlmostEqual(-2.0, v1[1])
        v1 = VMD(1.0,2.0)
        v1 /= v2
        self.assertAlmostEqual(1.0/5.0, v1[0])
        self.assertAlmostEqual(-2.0, v1[1])

    def test_equality_operators(self):
        v1 = VMD(1.0,2.0)
        self.assertEqual(v1,  v1)
        v2 = VMD(1.0,2.0) # different object, same value
        self.assertEqual(v1,  v2)
        self.assertFalse(v1 != v2)
        v3 = VMD(1.0,-5.0)
        self.assertNotEqual(v1,  v3)


    #==================== Failure cases =======================================
    def test_value_read_access_raises_error_for_invalid_indices(self):
        vector = VMD(1.0,2.0)
        self.assertRaises(IndexError, vector.__getitem__, 2)

    def test_value_write_access_raises_error_for_invalid_indices(self):
        vector = VMD(1.0,2.0)
        self.assertRaises(IndexError, vector.__setitem__, 2, 5.0)

    def test_scalar_prod_raises_error_with_dimension_mismatch(self):
        v1 = VMD(1,2,3)
        v2 = VMD(1,2,3,4)
        self.assertRaises(RuntimeError, v1.scalar_prod, v2)

    def test_cross_prod_raises_error_with_dimension_mismatch(self):
        v1 = VMD(1,2,3)
        v2 = VMD(1,2,3,4)
        self.assertRaises(RuntimeError, v1.cross_prod, v2)

    def test_angle_raises_error_with_dimension_mismatch(self):
        v1 = VMD(1,2,3)
        v2 = VMD(1,2,3,4)
        self.assertRaises(RuntimeError, v1.angle, v2)

if __name__ == '__main__':
    unittest.main()
