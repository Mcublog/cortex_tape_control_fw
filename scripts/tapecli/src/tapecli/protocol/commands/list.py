from enum import Enum, auto


class Commands(Enum):
    CMDID_UNKNOWN = 0x00
    CMDID_GET_VERSION = 0x01
    CMDID_SOLENOID_PULL = 0x02
    CMDID_MECH_SET_MODE = 0x03
    CMDID_MECH_GET_STATE = 0x04
    CMDID_MECH_TRANSPORT = 0x05
    CMDID_LAST = auto()

    @classmethod
    def _missing_(cls, value) -> 'Commands':
        return Commands.CMDID_UNKNOWN
