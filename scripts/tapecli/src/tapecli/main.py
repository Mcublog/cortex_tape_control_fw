#!/usr/bin/env python3
import argparse
import json
import logging
import sys
import time
from pathlib import Path

from pylogus import logger_init

from tapecli.device.device import Device
from tapecli.device.serial_adapter import SerialPortAdapter
from tapecli.interface import Command
from tapecli.protocol.commands.list import Commands
from tapecli.protocol.commands.solenoid_pull import SolenoidPullV1
from tapecli.protocol.commands.version import VersionV1
from tapecli.version import VERSION

log = logger_init(__name__, logging.INFO)

DESCRIPTION = f"Sender data to tape controller v{VERSION}"

USAGE = '\n'.join((f"{DESCRIPTION}. Send some command to controller",
                   "USING: tapecli -p /dev/ttyACM0 -c command.json"))


def _on_command_rx(command: Command) -> None:
    log.debug(f"rx: {command} len: {len(command.paylod)}")
    if command.cid == Commands.CMDID_GET_VERSION.value:
        v = VersionV1.response(command.paylod)
        log.info(v)


def main():
    parser = argparse.ArgumentParser(prog='Tapecli',
                                     description=f"{DESCRIPTION}",
                                     usage=USAGE)
    parser.add_argument('-p',
                        '--port',
                        type=str,
                        help='Port with device',
                        default='/dev/ttyACM0',
                        required=True)
    parser.add_argument('-b',
                        '--baudrate',
                        type=int,
                        help='Port baudrate',
                        default=115200,
                        required=False)
    parser.add_argument('-c',
                        '--command',
                        type=str,
                        help='Path to command .json',
                        default='',
                        required=False)

    try:
        port = parser.parse_args().port
        baudrate = parser.parse_args().baudrate
        path_to_command = Path(parser.parse_args().command)
    except Exception as e:
        log.error(f"{e}")
        sys.exit(1)

    try:
        adapter = SerialPortAdapter(port, baudrate, False)
        device = Device(adapter=adapter)
    except Exception as e:
        log.error(e)
        sys.exit(1)

    device.on_command_rx = _on_command_rx
    device.run()

    cmd: Command
    if not path_to_command.name:
        cmd = VersionV1().request()
    else:
        try:
            data = json.loads(Path(path_to_command).read_text())
            cmd = SolenoidPullV1(**data).request()
        except Exception as e:
            log.error(e)
            sys.exit(1)

    device.send(cmd)

    for _ in range(1):
        time.sleep(1)
        log.info("...")
    device.stop()


if __name__ == "__main__":
    main()
