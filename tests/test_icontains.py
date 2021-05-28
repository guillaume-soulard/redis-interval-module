from RLTest import Env


class TestIContains():

    def __init__(self):
        self.env = Env()

    def test_icontains_should_return_an_error_with_0_parameters(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('icontains').error()

    def test_icontains_should_return_an_error_with_1_parameter(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('icontains', 'intervals').error()

    def test_icontains_should_return_an_error_when_second_parameter_is_not_a_number(self):
        self.env.cmd('FLUSHALL')
        self.env.expect('icontains', 'intervals', 'test').error()

    def test_icontains_should_return_one_member(self):
        self.env.cmd('FLUSHALL')
        self.env.cmd('iadd', 'intervals', '0,1', 'i1')
        self.env.expect('icontains', 'intervals', '0').equal([[b'i1', b'1', b'0', b'1', b'1']])
