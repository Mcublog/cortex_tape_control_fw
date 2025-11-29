#!/usr/bin/env python3
import logging
import time

from pylogus import logger_init

from tapecli.device.device import Device
from tapecli.device.serial_adapter import SerialPortAdapter
from tapecli.interface import Command
from tapecli.protocol.commands.list import Commands
from tapecli.protocol.commands.version import VersionV1

log = logger_init(__name__, logging.INFO)


def _on_command_rx(command: Command) -> None:
    log.debug(f"rx: {command} len: {len(command.paylod)}")
    if command.cid == Commands.CMDID_GET_VERSION.value:
        v = VersionV1.response(command.paylod)
        log.info(v)


def main_tst():
    adapter = SerialPortAdapter("/dev/ttyS10", 115200, False)
    device = Device(adapter=adapter)
    device.on_command_rx = _on_command_rx

    device.run()

    cmd = VersionV1().request()
    device.send(cmd)

    for _ in range(1):
        time.sleep(1)
        log.info("...")
    device.stop()


if __name__ == "__main__":
    # main()
    main_tst()
