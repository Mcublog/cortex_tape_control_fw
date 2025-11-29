from enum import Enum, auto


class Commands(Enum):
    CMDID_UNKNOWN = 0x00
    CMDID_GET_VERSION = 0x01
    CMDID_LAST = auto()

    @classmethod
    def _missing_(cls, value) -> 'Commands':
        return Commands.CMDID_UNKNOWN
