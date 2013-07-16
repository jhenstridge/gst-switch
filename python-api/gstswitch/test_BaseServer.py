from server import BaseServer
import pytest


class TestSetVideoPort(object):

    def test_negative(self):
        port = -3
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_video_port(port)

    def test_zero(self):
        port = 0
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_video_port(port)

    def test_huge(self):
        port = 1e6
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_video_port(port)

    def test_none(self):
        port = None
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_video_port(port)

    def test_string(self):
        port = ""
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_video_port(port)

    def test_dict(self):
        port = {}
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_video_port(port)

    def test_normal(self):
        port = 3000
        instance = BaseServer()
        instance.set_video_port(port)
        assert port == int(instance.VIDEO_PORT)


class TestSetAudioPort(object):

    def test_negative(self):
        port = -3
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_audio_port(port)

    def test_zero(self):
        port = 0
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_audio_port(port)

    def test_huge(self):
        port = 1e6
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_audio_port(port)

    def test_none(self):
        port = None
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_audio_port(port)

    def test_string(self):
        port = ""
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_audio_port(port)

    def test_dict(self):
        port = {}
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_audio_port(port)

    def test_normal(self):
        port = 3000
        instance = BaseServer()
        instance.set_audio_port(port)
        assert port == int(instance.AUDIO_PORT)


class TestSetControlPort(object):

    def test_negative(self):
        port = -3
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_control_port(port)

    def test_zero(self):
        port = 0
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_control_port(port)

    def test_huge(self):
        port = 1e6
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_control_port(port)

    def test_none(self):
        port = None
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_control_port(port)

    def test_string(self):
        port = ""
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_control_port(port)

    def test_dict(self):
        port = {}
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_control_port(port)

    def test_normal(self):
        port = 3000
        instance = BaseServer()
        instance.set_control_port(port)
        assert port == int(instance.CONTROL_PORT)


class TestSetRecordFile(object):

    def test_int(self):
        name = 1234
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_record_file(name)

    def test_dict(self):
        name = {1: 5, 2: 7}
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_record_file(name)

    def test_none(self):
        name = None
        instance = BaseServer()
        with pytest.raises(TypeError):
            instance.set_record_file(name)

    def test_zero_length(self):
        name = ''
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_record_file(name)

    def test_forward_slash(self):
        name = 'abcd/xyz'
        instance = BaseServer()
        with pytest.raises(ValueError):
            instance.set_record_file(name)

    def test_normal(self):
        name = "record1234"
        instance = BaseServer()
        instance.set_record_file(name)
        assert name == instance.RECORD_FILE

