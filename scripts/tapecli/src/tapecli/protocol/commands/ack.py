import ctypes as ct
from dataclasses import dataclass


class AckResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [('error', ct.c_uint32)]


@dataclass
class AckV1:
    error: int = 0

    @staticmethod
    def response(raw: bytes) -> 'AckV1':
        ll = AckResponseV1LL.from_buffer_copy(raw)
        return AckV1(error=abs(int(ll.error)))
