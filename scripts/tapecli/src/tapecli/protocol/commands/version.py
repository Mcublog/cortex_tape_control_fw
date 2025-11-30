import ctypes as ct
from dataclasses import dataclass

from tapecli.interface import Command
from tapecli.protocol.cobs.tape_proto_command import TapeProtoCommand
from tapecli.protocol.commands.list import Commands


class VerionRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_GET_VERSION.value


class VerionResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('version', ct.c_uint8 * 32),
    ]

@dataclass
class VersionV1:
    version: str = ""

    def request(self) -> Command:
        cmd = TapeProtoCommand()
        cmd.cid = 0
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(VerionRequestV1LL())
        return cmd

    @staticmethod
    def response(raw: bytes) -> 'VersionV1':
        ll = VerionResponseV1LL.from_buffer_copy(raw)
        version = bytes(ll.version).split(b'\x00')[0].decode('ascii', 'ignore')
        return VersionV1(version=version)
