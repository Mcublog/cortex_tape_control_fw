'''Port scanner for detecting our sensors'''
import logging
import os
from abc import abstractmethod
from dataclasses import dataclass
from threading import Event, Thread

# import serial
from pylogus import logger_init
from serial.tools.list_ports_common import ListPortInfo

import tapecli.config as config

log = logger_init(__name__, logging.INFO)

SENSOR_VID = 1155
SENSOR_PID = 22336

EMUPORT_HW_ID = "CNCB1"
EMULIB_ERROR = -1

LINUX_EMU_PORT_NAME = "ttyS10"


def get_port_list() -> list[ListPortInfo]:
    try:
        # NOTE: exception handling on wine
        import serial.tools.list_ports as list_ports
        return list_ports.comports()
    except Exception as e:
        log.error(e)
    return []


class Sensors:
    pass


@dataclass
class SensorPort:
    sensor: Sensors
    portname: str


class PortScanner:
    devices: list[SensorPort] = []
    scanning: bool = False
    _scanning_proc: Thread
    _kill_scan: Event = Event()
    _scanning_proc: Thread

    def __init__(self) -> None:
        self._scanning_proc = Thread(name="port scanning",
                                     target=self.device_scan,
                                     args=[self._kill_scan],
                                     daemon=True)
        self._scanning_proc.start()

    @staticmethod
    def format_porname(portname: str) -> str:
        return f"/dev/{portname}" if os.name != 'nt' else portname

    @staticmethod
    def get_ports_list() -> list[ListPortInfo]:
        port_list: list[ListPortInfo] = [
            p for p in get_port_list() if p.vid != None or (
                (EMUPORT_HW_ID in p.hwid or p.name == LINUX_EMU_PORT_NAME)
                and config.DEBUG_PORT_ALLOWED)
        ]
        # if os.name != 'nt' and config.DEBUG_PORT_ALLOWED:
        #     port_list.append(ListPortInfo(device=LINUX_EMU_PORT_NAME))
        return port_list

    def check_disconnected_devices(self):
        avalable_ports = [
            PortScanner.format_porname(p.name)
            for p in PortScanner.get_ports_list()
        ]
        for s in self.devices:
            if s.portname not in avalable_ports:
                log.info(f"Disconnected: {s}")
                self.devices.remove(s)
                self.sensor_removed(s)

    def device_scan(self, kill_evt: Event) -> None:
        while not kill_evt.wait(0.01):
            if not self.scanning:
                continue
            self.check_disconnected_devices()

    def clear(self):
        self.devices.clear()

    def destroy(self):
        self._kill_scan.set()
        self._scanning_proc.join()

    @abstractmethod
    def new_sensor_detection(self, sensor: SensorPort):
        ...

    @abstractmethod
    def sensor_removed(self, sensor: SensorPort):
        ...
