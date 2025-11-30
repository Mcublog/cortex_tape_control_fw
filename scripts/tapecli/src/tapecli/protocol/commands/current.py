import ctypes as ct
from dataclasses import dataclass

from battapp.interface import Command
from battapp.protocol.cobs.batt_proto_command import BattProtoCommand
from battapp.protocol.commands.list import Commands


class GetCurrentValuesV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_GET_CURRENT_VALUES.value


class CurrentValuesResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('percent', ct.c_uint8),
        ('process_time', ct.c_uint64),
        ('current_mA', ct.c_int32),
        ('voltage_mV', ct.c_int32),
        ('state', ct.c_uint8),
        ('cycles_counter', ct.c_uint32),
        ('cap_current', ct.c_uint32),
        ('cap_total', ct.c_uint32),
    ]


@dataclass
class CurrentValuesV1:
    percent: int = 0
    process_time: int = 0
    current_mA: int = 0
    voltage_mV: int = 0
    state: int = 0
    cycles_counter: int = 0
    cap_current: int = 0
    cap_total: int = 0

    def request(self) -> Command:
        cmd = BattProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(GetCurrentValuesV1LL())
        return cmd

    @staticmethod
    def response(raw: bytes) -> 'CurrentValuesV1':
        ll = CurrentValuesResponseV1LL.from_buffer_copy(raw)
        return CurrentValuesV1(
            percent=int(ll.percent),
            process_time=int(ll.process_time),
            current_mA=int(ll.current_mA),
            voltage_mV=int(ll.voltage_mV),
            state=int(ll.state),
            cycles_counter=abs(int(ll.cycles_counter)),
            cap_current=abs(int(ll.cap_current)),
            cap_total=abs(int(ll.cap_total)),
        )
