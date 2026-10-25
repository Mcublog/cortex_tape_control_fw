import ctypes as ct
from dataclasses import dataclass

from tapecli.interface import Command
from tapecli.protocol.cobs.tape_proto_command import TapeProtoCommand
from tapecli.protocol.commands.list import Commands


class MechSetModeRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
        ('mode', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_MECH_SET_MODE.value


@dataclass
class MechSetModeV1:
    mode: int = 0

    def request(self) -> Command:
        payload = MechSetModeRequestV1LL()
        payload.mode = ct.c_uint32(self.mode)

        cmd = TapeProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(payload)
        return cmd
