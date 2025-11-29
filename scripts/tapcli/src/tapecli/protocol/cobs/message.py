#!/usr/bin/env python3

import ctypes as ct
from dataclasses import dataclass, field

from cobs import cobsr

from tapecli.utils.crc import xcrc32

DataPointer = ct.POINTER(ct.c_uint8)


class CMessageHeader(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('crc', ct.c_uint32),
        ('channel', ct.c_uint32),
        ('version', ct.c_uint32),
        ('size', ct.c_uint32),
    ]

    @property
    def crc32(self)-> int:
        return int(self.crc)

class CMessage(ct.Structure):
    _pack_ = 1
    _fields_ = [
        ('crc', ct.c_uint32),
        ('channel', ct.c_uint32),
        ('version', ct.c_uint32),
        ('size', ct.c_uint32),
        ('data', DataPointer),
    ]


@dataclass
class Message:
    """
    Контейнер для сообщений
    """
    channel: int
    # size with header
    size: int = field(init=False)
    version: int
    crc: int = field(init=False)
    data: bytes = field(default=b'')

    def __post_init__(self):
        self.size = header_size() if self.data is None else len(
            self.data) + header_size()
        header_bytes = self.channel.to_bytes(
            4, 'little') + self.version.to_bytes(
                4, 'little') + self.size.to_bytes(4, 'little')
        self.crc = xcrc32(header_bytes + self.data)


def serialize(msg: Message) -> bytes:
    cmsg = CMessage(channel=msg.channel,
                    size=msg.size,
                    version=msg.version,
                    crc=msg.crc)
    buffer = (ct.c_uint8 * msg.size)()
    ct.memmove(ct.addressof(buffer), ct.addressof(cmsg), header_size())
    if msg.data is not None:
        ct.memmove(
            ct.addressof(buffer) + header_size(), msg.data, len(msg.data))
    return cobsr.encode(bytes(buffer)) + b'\x00'


def deserialize(raw: bytes) -> Message:
    # remove /x00
    size = len(raw) - 1
    decoded = cobsr.decode(raw[:size])
    decoded += b'\x00' * (size - len(decoded))
    cmsg = CMessageHeader.from_buffer_copy(decoded)
    out = Message(channel=cmsg.channel,
                  version=cmsg.version,
                  data=decoded[header_size():])
    if out.crc != cmsg.crc32:
        print("crc error")
    return out


    return Message(channel=cmsg.channel,
                   version=cmsg.version,
                   data=decoded[header_size():])


def header_size() -> int:
    return ct.sizeof(CMessageHeader)
