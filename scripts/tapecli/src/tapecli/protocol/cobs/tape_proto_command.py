from dataclasses import dataclass, field

import tapecli.protocol.cobs.message as msg
from tapecli.interface import Command


@dataclass
class TapeProtoCommand(Command):
    _channel: int = field(init=False)
    _paylod: bytes = field(init=False)
    _timeout_ms: int = field(default=100)
    _version: int = field(default=0)
    _cid: int = field(init=False, default=0)

    @property
    def channel(self) -> int:
        return self._channel

    @channel.setter
    def channel(self, channel: int):
        self._channel = channel

    @property
    def version(self) -> int:
        return self._version

    @version.setter
    def version(self, version: int):
        self._version = version

    @property
    def cid(self) -> int:
        return self._cid

    @cid.setter
    def cid(self, cid: int):
        self._cid = cid

    @property
    def paylod(self) -> bytes:
        return self._paylod

    @paylod.setter
    def paylod(self, paylod: bytes):
        self._paylod = paylod

    @property
    def timeout_ms(self) -> int:
        return self._timeout_ms

    @timeout_ms.setter
    def timeout_ms(self, timeout_ms: int):
        self._timeout_ms = timeout_ms

    def serialize(self) -> bytes:
        return msg.serialize(
            msg.Message(channel=self.channel,
                        version=self.version,
                        data=self.paylod))

    @staticmethod
    def deserialize(raw: bytes) -> 'Command':
        cmsg = msg.deserialize(raw)
        cmd = TapeProtoCommand()
        cmd.channel = cmsg.channel
        cmd.version = cmsg.version
        cmd.cid = int.from_bytes(cmsg.data[:4], 'little')
        cmd.paylod = cmsg.data[4:]
        return cmd
