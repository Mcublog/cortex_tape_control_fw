import logging
from abc import abstractmethod
from dataclasses import dataclass, field
from queue import Queue
from threading import Event, Lock, Thread

from tapecli.interface import Adapter, Command

log = logging.getLogger(__name__)
log.setLevel(logging.INFO)


@dataclass
class Device:
    adapter: Adapter
    # tables: TablesByChannel

    _queue: Queue = field(init=False, repr=False)
    _polling: Thread = field(init=False, repr=False)
    _kill_evt: Event = field(init=False, repr=False)
    _mutex: Lock = field(init=False, default_factory=Lock)

    def __post_init__(self):
        self._queue = Queue()
        self._kill_evt = Event()
        self._polling = Thread(target=self._port_handler, name='port_handling')

    def run(self) -> bool:
        if not self.adapter.connect():
            return False
        self.adapter.on_adapter_error = self.on_device_error
        self._polling.start()
        return True

    def stop(self):
        self._kill_evt.set()
        try:
            self._polling.join()
        except Exception as e:
            log.error(e)
        self.adapter.disconnect()

    def send(self, cmd: Command) -> bool:
        self._queue.put_nowait(cmd)
        return True

    def _port_handler(self):
        while not self._kill_evt.wait(0.001):
            with self._mutex:
                command = self.adapter.read()
                if command:
                    log.debug(f"RX: {command}")
                    self.on_command_rx(command)
            if self._queue.empty():
                continue
            with self._mutex:
                command = self._queue.get_nowait()
                self.adapter.write(command)
                if not (command := self.adapter.read()):
                    continue
                log.info(f"RX: {command}")

    @abstractmethod
    def on_command_rx(self, command: Command) -> None:
        ...

    @abstractmethod
    def on_device_error(self, e: Exception) -> None:
        ...