import ctypes as ct
from dataclasses import dataclass


class UnknownResponseV1LL(ct.Structure):
    _pack_ = 1
    _fields_ = [('status', ct.c_uint32)]


@dataclass
class UnknownV1:
    status: int = 0

    @staticmethod
    def response(raw: bytes) -> 'UnknownV1':
        ll = UnknownResponseV1LL.from_buffer_copy(raw)
        return UnknownV1(status=abs(int(ll.status)))
