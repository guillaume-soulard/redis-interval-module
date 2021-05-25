from RLTest import Env


class TestIAdd():

    def __init__(self):
        self.env = Env()

    def shouldError(self, interval):
        self.env.cmd('FLUSHALL')
        self.env.expect('iadd', 'intervals', interval, 'i1').error()

    def shouldNoError(self, interval):
        self.env.cmd('FLUSHALL')
        self.env.expect('iadd', 'intervals', interval, 'i1').equal(1)

    def test_iadd_empty_interval_should_return_an_error(self):
        self.shouldError('')

    def test_iadd_interval_with_only_bound_separator_should_return_an_error(self):
        self.shouldError(',')

    def test_iadd_interval_with_only_1_number_should_return_an_error(self):
        self.shouldError('0')

    def test_iadd_interval_with_only_lower_bound_should_return_an_error(self):
        self.shouldError('0,')

    def test_iadd_interval_with_only_upper_bound_should_return_an_error(self):
        self.shouldError(',1')

    def test_iadd_interval_with_text_should_return_an_error(self):
        self.shouldError('test')

    def test_iadd_interval_with_text_on_lower_bound_should_return_an_error(self):
        self.shouldError('test,1')

    def test_iadd_interval_with_text_on_upper_bound_should_return_an_error(self):
        self.shouldError('0,test')

    def test_iadd_interval_with_lower_greater_than_upper_should_return_an_error(self):
        self.shouldError('1,0')

    def test_iadd_interval_with_minus_inf_on_upper_should_return_an_error(self):
        self.shouldError('1,-inf')

    def test_iadd_interval_with_inf_on_lower_should_return_an_error(self):
        self.shouldError('inf,1')

    def test_iadd_interval_with_plus_inf_on_lower_should_return_an_error(self):
        self.shouldError('+inf,1')

    def test_iadd_interval_with_inf_reversed_should_return_an_error(self):
        self.shouldError('+inf,-inf')

    def test_iadd_interval_with_invalid_bound_inclusion_should_return_an_error(self):
        self.shouldError('(+inf,-inf}')

    def test_iadd_with_valid_interval_should_return_1(self):
        self.shouldNoError('0,1')

    def test_iadd_with_minus_inf_on_lower_interval_should_return_1(self):
        self.shouldNoError('-inf,1')

    def test_iadd_with_inf_on_upper_interval_should_return_1(self):
        self.shouldNoError('0,inf')

    def test_iadd_with_plus_inf_on_upper_interval_should_return_1(self):
        self.shouldNoError('0,+inf')

    def test_iadd_with_valid_bounds_1_should_return_1(self):
        self.shouldNoError('[0,1]')

    def test_iadd_with_valid_bounds_2_should_return_1(self):
        self.shouldNoError('[0,1[')

    def test_iadd_with_valid_bounds_3_should_return_1(self):
        self.shouldNoError(']0,1]')

    def test_iadd_with_valid_bounds_4_should_return_1(self):
        self.shouldNoError(']0,1[')
