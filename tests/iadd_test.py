from rmtest import ModuleTestCase
from setup import MODULE_PATH
import unittest


class IAddTestCase(ModuleTestCase(MODULE_PATH, module_args=())):

    def setUp(self):
        self.cmd('flushall')

    def tearDown(self):
        self.cmd('flushall')

    def test_iadd_with_0_arguments_should_return_an_error(self):
        with self.redis() as r:
            with self.assertResponseError():
                r.execute_command('iadd')

    def test_iadd_with_1_arguments_should_return_an_error(self):
        with self.redis() as r:
            with self.assertResponseError():
                r.execute_command('iadd', 'intervals')

    def test_iadd_with_2_arguments_should_return_an_error(self):
        with self.redis() as r:
            with self.assertResponseError():
                r.execute_command('iadd', 'intervals', '0,1')

    def test_iadd_with_3_arguments_should_return_an_error(self):
        with self.redis() as r:
            with self.assertResponseError():
                r.execute_command('iadd', 'intervals', '0,1')

    def test_iadd_with_1_interval_should_return_1(self):
        self.assertEqual(self.cmd('iadd', 'intervals', '0,1', 'i1'), 1)

    def test_iadd_with_2_intervals_should_return_2(self):
        self.assertEqual(self.cmd('iadd', 'intervals', '0,1', 'i1', '2,3', 'i2'), 2)


if __name__ == '__main__':
    unittest.main()
