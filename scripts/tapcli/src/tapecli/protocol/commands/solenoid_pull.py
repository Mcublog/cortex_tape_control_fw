import ctypes as ct
from dataclasses import dataclass

from tapecli.interface import Command
from tapecli.protocol.cobs.tape_proto_command import TapeProtoCommand
from tapecli.protocol.commands.list import Commands


class SolenoidPullRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
        ('pull_time_ms', ct.c_uint32),
        ('cooldown_ms', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_SOLENOID_PULL.value


class SettingsResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('error', ct.c_uint32),
    ]


@dataclass
class SolenoidPullV1:
    pull_time_ms: int = 0
    cooldown_ms: int = 0

    @staticmethod
    def response(raw: bytes) -> 'SolenoidPullV1':
        ll = SettingsResponseV1LL.from_buffer_copy(raw)
        return SolenoidPullV1(
            pull_time_ms=int(ll.pull_time_ms),
            cooldown_ms=int(ll.cooldown_ms),
        )

    def request(self) -> Command:
        payload = SolenoidPullRequestV1LL()
        payload.pull_time_ms = ct.c_uint32(self.pull_time_ms)
        payload.cooldown_ms = ct.c_uint32(self.cooldown_ms)

        cmd = TapeProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(payload)
        return cmd
