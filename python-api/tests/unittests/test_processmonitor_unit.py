"""Unittests for ProcessMonitor class in server.py"""
import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(__file__, "../../../")))

from gstswitch.process_monitor import ProcessMonitor
import pytest
from gstswitch.exception import ServerProcessError, MatchTimeoutError
from gstswitch.exception import MatchEofError, SelectError
from mock import Mock, patch, mock_open, DEFAULT, ANY
from six import StringIO

PATH = '/usr/bin/'


class TestProcessMonitor(object):
    """ Unittests for ProcessMonitor class in server.py
    """

    def test_start(self):
        """ Test if starting a ProcessMonitor calls the Popen-Constructor
            in the right way
        """

        with patch('subprocess.Popen.__init__') as mock:
            mon = ProcessMonitor('abc')
            assert mon is not None
            mock.assert_called_once_with('abc',
                                         shell=False,
                                         stdout=ANY,
                                         bufsize=ANY,
                                         stderr=ANY)

    def test_start_raises(self):
        """ Test if starting a ProcessMonitor with a Command that makes the
            Popen-Constructor throw an OSError correctly raises a
            ServerProcessError
        """

        with patch('subprocess.Popen.__init__') as mock:
            mock.side_effect = OSError
            with pytest.raises(ServerProcessError):
                mon = ProcessMonitor('abc')
                assert mon is not None

    def test_terminate(self):
        """ Test if terminating a ProcessMonitor at least calls
            communicate and terminate on the Popen-Instance
        """
        with patch.multiple('subprocess.Popen',
                            __init__=DEFAULT,
                            terminate=DEFAULT,
                            communicate=DEFAULT) as mocks:

            mon = ProcessMonitor('abc')
            mon.stdout = sys.stdout
            mon.terminate()

            assert mocks['communicate'].call_count == 1
            assert mocks['terminate'].call_count == 1

    def test_copy_remaining_on_terminate(self):
        """ Test if terminating a ProcessMonitor at least calls
            communicate and terminate on the Popen-Instance
        """
        with patch.multiple('subprocess.Popen',
                            __init__=DEFAULT,
                            terminate=DEFAULT,
                            communicate=DEFAULT):

            m_file = mock_open(read_data='testpattern')
            m_file.fileno = Mock(return_value=123)

            with patch('select.select', return_value=([m_file], [], [])):
                reads = [b'testpattern', b'']
                with patch('os.read', side_effect=lambda a, b: reads.pop(0)):
                    mon = ProcessMonitor('abc')
                    mon.stdout = m_file
                    mon._cmd_output_target = StringIO()
                    mon.pid = 123
                    mon.terminate()

                assert mon._cmd_output_target.getvalue() == 'testpattern'

    def test_break_no_remaining_on_terminate(self):
        """ Test if terminating a ProcessMonitor at least calls
            communicate and terminate on the Popen-Instance
        """
        with patch.multiple('subprocess.Popen',
                            __init__=DEFAULT,
                            terminate=DEFAULT,
                            communicate=DEFAULT):

            m_file = mock_open(read_data='testpattern')
            m_file.fileno = Mock(return_value=123)

            with patch('select.select', return_value=([], [], [])):
                reads = [b'testpattern', b'']
                with patch('os.read', side_effect=lambda a, b: reads.pop(0)):
                    mon = ProcessMonitor('abc')
                    mon.stdout = m_file
                    mon._cmd_output_target = StringIO()
                    mon.pid = 123
                    mon.terminate()

                assert mon._cmd_output_target.getvalue() == ''

    def test_instant_return(self):
        """ Test if wait_for_output returns instantly if the buffer
            already contains the match-string
        """
        with patch('subprocess.Popen.__init__') as mock:
            mon = ProcessMonitor('abc')
            mon._buffer = 'aaa ZZZ ccc'
            mon.wait_for_output('ZZZ', timeout=0, count=1)
            assert mock.call_count == 1

    def test_instant_fail(self):
        """ Test if wait_for_output fails instantly if the buffer
            does not contain the match-string and timeout is 0
            (mocking select.select to return instantly with no
            readable buffer when timeout is 0)
        """
        with patch('select.select') as selectmock:
            selectmock.return_value = ([], [], [])
            with patch('subprocess.Popen.__init__') as mock:
                mon = ProcessMonitor('abc')
                mon.stdout = None
                mon._buffer = 'aaa bbb ccc'
                with pytest.raises(MatchTimeoutError):
                    mon.wait_for_output('ZZZ', timeout=0, count=1)
                assert mock.call_count == 1

    def test_instant_return_two_times(self):
        """ Test if wait_for_output returns instantly if the buffer
            contains the match-string two times and a two-times match
            is requested
        """
        with patch('subprocess.Popen.__init__') as mock:
            mon = ProcessMonitor('abc')
            mon._buffer = 'aaa ZZZ ccc ZZZ ddd'
            mon.wait_for_output('ZZZ', timeout=0, count=2)
            assert mock.call_count == 1

    def test_instant_return_two_times_in_buffer(self):
        """ Test if wait_for_output returns instantly if the buffer
            contains the match-string two times and a one-time match
            is requested
        """
        with patch('subprocess.Popen.__init__') as mock:
            mon = ProcessMonitor('abc')
            mon._buffer = 'aaa ZZZ ccc ZZZ ddd'
            mon.wait_for_output('ZZZ', timeout=0, count=1)
            assert mock.call_count == 1

    def test_instant_fail_two_times(self):
        """ Test if wait_for_output fails instantly if the buffer
            contains the match-string only one times and a two-times
            match is requested
        """
        with patch('select.select') as selectmock:
            selectmock.return_value = ([], [], [])
            with patch('subprocess.Popen.__init__'):
                mon = ProcessMonitor('abc')
                mon.stdout = 123
                mon._buffer = 'aaa ZZZ bbb ccc'
                with pytest.raises(MatchTimeoutError):
                    mon.wait_for_output('ZZZ', timeout=0, count=2)
            assert selectmock.call_count == 1

    def test_fail_not_a_timeout(self):
        """ Test if wait_for_output fails if select.select returns without
            no readable pipe and the timeout is not reached yet
        """
        with patch('select.select') as selectmock:
            selectmock.return_value = ([], [], [])
            with patch('subprocess.Popen.__init__') as mock:
                mon = ProcessMonitor('abc')
                mon.stdout = 123
                mon._buffer = 'aaa bbb ccc'
                with pytest.raises(SelectError):
                    mon.wait_for_output('ZZZ', timeout=500, count=1)
                    assert mock.call_count == 1

    def test_read_returns_normally(self):
        """ Test if wait_for_output returns successfull with when os.read
            returns a chunk containing the requested match
        """
        with patch('select.select') as selectmock:
            stdoutmock = Mock()
            selectmock.return_value = ([stdoutmock], [], [])

            with patch('os.read') as readmock:
                readmock.return_value = 'aaa ZZZ ccc'.encode('utf-8')

                with patch('subprocess.Popen.__init__'):
                    mon = ProcessMonitor('abc')
                    mon.stdout = stdoutmock

                    mon.wait_for_output('ZZZ', timeout=0, count=1)

                readmock.assert_called_once_with(stdoutmock.fileno(), ANY)

    def test_read_eof(self):
        """ Test if wait_for_output raises an error when os.read returns
            an empty string, signaling an EOF this way
        """
        with patch('select.select') as selectmock:
            stdoutmock = Mock()
            selectmock.return_value = ([stdoutmock], [], [])

            with patch('os.read') as readmock:
                # simulate EOF
                readmock.return_value = ''.encode('utf-8')

                with patch('subprocess.Popen.__init__'):
                    mon = ProcessMonitor('abc')
                    mon.stdout = stdoutmock

                    with pytest.raises(MatchEofError):
                        mon.wait_for_output('ZZZ', timeout=0, count=1)

                readmock.assert_called_once_with(stdoutmock.fileno(), ANY)
