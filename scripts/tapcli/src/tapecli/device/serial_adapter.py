import logging
from dataclasses import dataclass, field

import serial
from pylogus import logger_init

from tapecli.interface import Adapter, Command

log = logger_init(__name__, logging.INFO)

@dataclass
class SerialPortAdapter(Adapter):
    portname: str
    baudrate: int
    ctsrts: bool = field(default=False)

    _serial: serial.Serial = field(init=False)

    _lastcmd: Command = field(init=False)
    _read_buf: bytes = b''

    def __post_init__(self):
        self._serial = serial.Serial(self.portname, baudrate=self.baudrate)

    def connect(self) -> bool:
        if not self._serial.is_open:
            self._serial.open()
        return self._serial.is_open

    def disconnect(self) -> bool:
        if not self._serial.is_open:
            return False
        self._serial.close()
        return True

    def write(self, command: Command) -> int:
        self._lastcmd = command
        try:
            ret = self._serial.write(command.serialize())
        except Exception as e:
            log.error(e)
            self.on_adapter_error(e)
            return 0
        return ret if ret is not None else 0

    def read(self) -> Command | None:
        try:
            raw = self._serial.read_all()
        except Exception as e:
            log.error(e)
            self.on_adapter_error(e)
            return None
        if (raw):
            self._read_buf += raw
        if b'\00' not in self._read_buf:
            return None
        ack = self._lastcmd.deserialize(self._read_buf)
        self._read_buf = b''
        return ack

    def on_adapter_error(self, e: Exception) -> None:
        pass
