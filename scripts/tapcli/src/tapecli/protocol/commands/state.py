import ctypes as ct
from dataclasses import dataclass

from battapp.interface import Command
from battapp.protocol.cobs.batt_proto_command import BattProtoCommand
from battapp.protocol.commands.list import Commands


class StateSetRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
        ('state', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_SET_STATE.value


@dataclass
class StateV1:
    state: int = 0

    def set_request(self) -> Command:
        payload = StateSetRequestV1LL()
        payload.state = ct.c_uint32(self.state)

        cmd = BattProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(payload)
        return cmd
