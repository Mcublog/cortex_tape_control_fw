import ctypes as ct
from dataclasses import dataclass

from tapecli.interface import Command
from tapecli.protocol.cobs.tape_proto_command import TapeProtoCommand
from tapecli.protocol.commands.list import Commands

# Действия высокоуровневой команды CMDID_MECH_TRANSPORT, см. mech_action_t.
ACTION_STOP = 0
ACTION_PLAY_FWD = 1
ACTION_PLAY_REV = 2
ACTION_REWIND_FWD = 3
ACTION_REWIND_REV = 4

ACTION_NAMES = {
    ACTION_STOP: 'stop',
    ACTION_PLAY_FWD: 'play fwd',
    ACTION_PLAY_REV: 'play rev',
    ACTION_REWIND_FWD: 'rewind fwd',
    ACTION_REWIND_REV: 'rewind rev',
}

# Пользовательские режимы, см. mech::UserMode.
USER_MODE_NAMES = {
    0: 'stop',
    1: 'play fwd',
    2: 'play rev',
    3: 'rec fwd',
    4: 'rec rev',
    5: 'rewind fwd',
    6: 'rewind rev',
}

# Состояния механизма, см. mech::Mode.
MODE_NAMES = {
    0: 'to init',
    1: 'sub init',
    2: 'sub to stop',
    3: 'sub wait stop',
    4: 'stop',
    5: 'sub to active',
    6: 'sub act',
    7: 'sub wait dir',
    8: 'sub hd dir sel',
    9: 'sub wait pinch',
    10: 'sub pinch sel',
    11: 'sub wait takeup',
    12: 'sub tu dir sel',
    13: 'sub wait run',
    14: 'pb fwd',
    15: 'pb rev',
    16: 'rc fwd',
    17: 'rc rev',
    18: 'fw fwd',
    19: 'fw rev',
    20: 'fw fwd hd rev',
    21: 'fw rev hd rev',
    22: 'sub to halt',
    23: 'halt',
}

# Ошибки транспорта, см. mech::Error.
ERROR_NAMES = {
    0x00: 'none',
    0x01: 'bad drive',
    0x02: 'no ctrl',
    0x04: 'logic fault',
}

SENSOR_NAMES = (
    (0x01, 'home'),
    (0x02, 'tacho'),
    (0x04, 'tape in'),
    (0x08, 'no rec fwd'),
    (0x10, 'no rec rev'),
)


class MechTransportRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
        ('action', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_MECH_TRANSPORT.value


@dataclass
class MechTransportV1:
    action: int = ACTION_STOP

    def request(self) -> Command:
        payload = MechTransportRequestV1LL()
        payload.action = ct.c_uint32(self.action)

        cmd = TapeProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(payload)
        return cmd

    @staticmethod
    def action_name(action: int) -> str:
        return ACTION_NAMES.get(action, f'unknown ({action})')


class MechStateRequestV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('cmdid', ct.c_uint32),
    ]

    def __init__(self):
        self.cmdid = Commands.CMDID_MECH_GET_STATE.value


class MechStateResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('mode', ct.c_uint32),
        ('target_mode', ct.c_uint32),
        ('user_mode', ct.c_uint32),
        ('transition', ct.c_uint32),
        ('retries', ct.c_uint32),
        ('error', ct.c_uint32),
        ('sensors', ct.c_uint32),
        ('last_dir', ct.c_uint32),
        ('capstan', ct.c_uint32),
    ]


@dataclass
class MechStateV1:
    mode: int = 0
    target_mode: int = 0
    user_mode: int = 0
    transition: int = 0
    retries: int = 0
    error: int = 0
    sensors: int = 0
    last_dir: int = 0
    capstan: int = 0

    def request(self) -> Command:
        cmd = TapeProtoCommand()
        cmd.channel = 0
        cmd.version = 0
        cmd.paylod = bytes(MechStateRequestV1LL())
        return cmd

    @staticmethod
    def response(raw: bytes) -> 'MechStateV1':
        ll = MechStateResponseV1LL.from_buffer_copy(raw)
        return MechStateV1(
            mode=int(ll.mode),
            target_mode=int(ll.target_mode),
            user_mode=int(ll.user_mode),
            transition=int(ll.transition),
            retries=int(ll.retries),
            error=int(ll.error),
            sensors=int(ll.sensors),
            last_dir=int(ll.last_dir),
            capstan=int(ll.capstan),
        )

    @staticmethod
    def mode_name(mode: int) -> str:
        return MODE_NAMES.get(mode, f'unknown ({mode})')

    @staticmethod
    def user_mode_name(mode: int) -> str:
        return USER_MODE_NAMES.get(mode, f'unknown ({mode})')

    @staticmethod
    def error_name(error: int) -> str:
        if error == 0:
            return ERROR_NAMES[0]
        names = [name for bit, name in ERROR_NAMES.items() if bit and error & bit]
        return ', '.join(names) if names else f'unknown (0x{error:02x})'

    @staticmethod
    def sensors_names(sensors: int) -> str:
        names = [name for bit, name in SENSOR_NAMES if sensors & bit]
        return ', '.join(names) if names else 'none'

    def describe(self) -> str:
        """Человекочитаемое описание состояния механизма."""
        dir_name = 'fwd' if self.last_dir == 0 else 'rev'
        return '\n'.join((
            f"user mode  : {self.user_mode_name(self.user_mode)}",
            f"mode       : {self.mode_name(self.mode)}"
            f" -> {self.mode_name(self.target_mode)}",
            f"transition : {self.transition} ticks ({self.transition * 2} ms)",
            f"retries    : {self.retries}",
            f"error      : {self.error_name(self.error)}",
            f"sensors    : {self.sensors_names(self.sensors)}",
            f"last dir   : {dir_name}",
            f"capstan    : {'on' if self.capstan else 'off'}",
        ))
