from RLTest import Env


class TestIOverlaps():

    def __init__(self):
        self.env = Env()

    def test_ioverlaps_should_return_an_error_with_0_parameters(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('ioverlaps').error()

    def test_ioverlaps_should_return_an_error_with_1_parameter(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('ioverlaps', 'intervals').error()

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_is_in_the_interval(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '6,7').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_interval_is_in_the_parameter_interval(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '0,20').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_overlaps_lower_bound(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '0,7').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_overlaps_upper_bound(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '7,15').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_abuts_lower_bound(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '0,5').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_abuts_upper_bound(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '10,15').equal(
            [[b'i1', b'1', b'5', b'1', b'10']])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_abuts_lower_bound_and_lower_not_included(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', ']5,10', 'i1')
        self.env.expect('ioverlaps', 'intervals', '0,5').equal([])

    def test_ioverlaps_should_return_the_interval_when_parameter_interval_abuts_upper_bound_and_upper_not_included(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '5,10[', 'i1')
        self.env.expect('ioverlaps', 'intervals', '10,15').equal([])

    def test_ioverlaps_should_return_an_error_when_interval_is_incorrect(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('ioverlaps', 'intervals', '10,').error()
