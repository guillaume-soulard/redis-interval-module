from rmtest import ModuleTestCase
from setup import MODULE_PATH
import unittest


class ICardTestCase(ModuleTestCase(MODULE_PATH, module_args=())):

    def setUp(self):
        self.cmd('flushall')

    def tearDown(self):
        self.cmd('flushall')

    def test_icard_with_0_intervals_should_return_0(self):
        self.assertEqual(self.cmd('icard', 'intervals'), 0)

    def test_icard_should_return_1(self):
        self.cmd('iadd', 'intervals', '0,1', 'i1')
        self.assertEqual(self.cmd('icard', 'intervals'), 1)

    def test_icard_should_return_0_on_other_key(self):
        self.cmd('iadd', 'intervals', '0,1', 'i1')
        self.assertEqual(self.cmd('icard', 'other'), 0)

    def test_icard_should_return_2(self):
        self.cmd('iadd', 'intervals', '0,1', 'i1')
        self.cmd('iadd', 'intervals', '2,3', 'i2')
        self.assertEqual(self.cmd('icard', 'intervals'), 2)


if __name__ == '__main__':
    unittest.main()
