from abc import abstractmethod
from typing import Protocol


class Command(Protocol):

    @property
    def channel(self) -> int:
        ...

    @channel.setter
    def channel(self, channel: int):
        ...

    @property
    def version(self) -> int:
        ...

    @version.setter
    def version(self, version: int):
        ...

    @property
    def cid(self) -> int:
        ...

    @cid.setter
    def cid(self, cid: int):
        ...

    @property
    def paylod(self) -> bytes:
        ...

    @paylod.setter
    def paylod(self, paylod: bytes):
        ...

    @property
    def timeout_ms(self) -> int:
        ...

    @timeout_ms.setter
    def timeout_ms(self, timeout_ms: int):
        ...

    def serialize(self) -> bytes:
        ...

    @staticmethod
    def deserialize(raw: bytes) -> 'Command':
        ...


class Adapter(Protocol):

    @abstractmethod
    def connect(self) -> bool:
        ...

    @abstractmethod
    def disconnect(self) -> bool:
        ...

    @abstractmethod
    def write(self, command: Command) -> int:
        ...

    @abstractmethod
    def read(self) -> Command | None:
        ...

    @abstractmethod
    def on_adapter_error(self, e: Exception) -> None:
        ...
