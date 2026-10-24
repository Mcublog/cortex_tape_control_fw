#!/usr/bin/env python3
import argparse
import logging
import sys
import time

from pylogus import logger_init

from tapecli.device.device import Device
from tapecli.device.serial_adapter import SerialPortAdapter
from tapecli.interface import Command
from tapecli.protocol.commands.ack import AckV1
from tapecli.protocol.commands.list import Commands
from tapecli.protocol.commands.mech import (ACTION_PLAY_FWD, ACTION_PLAY_REV,
                                            ACTION_REWIND_FWD, ACTION_REWIND_REV,
                                            ACTION_STOP, MechStateV1,
                                            MechTransportV1)
from tapecli.protocol.commands.set_mode import MechSetModeV1
from tapecli.protocol.commands.solenoid_pull import SolenoidPullV1
from tapecli.protocol.commands.version import VersionV1
from tapecli.version import VERSION

log = logger_init(__name__, logging.INFO)

DESCRIPTION = f"Sender data to tape controller v{VERSION}"

USAGE = '\n'.join((
    f"{DESCRIPTION}.",
    "USING: tapecli -p /dev/ttyACM0 play-fwd",
))

# Коды ошибок ack: 0 — ok, 1 — неверный аргумент, 2 — отклонено, 3 — нет обработчика.
ACK_ERRORS = {
    0: 'ok',
    1: 'bad argument',
    2: 'rejected by mechanism',
    3: 'no handler',
}


def _print_ack(error: int) -> None:
    name = ACK_ERRORS.get(error, f'unknown ({error})')
    log.info(f"error: {error} ({name})")


def _on_command_rx(command: Command) -> None:
    log.debug(f"rx: {command} len: {len(command.paylod)}")
    if command.cid == Commands.CMDID_GET_VERSION.value:
        v = VersionV1.response(command.paylod)
        log.info(f"version: {v.version}")
    elif command.cid == Commands.CMDID_MECH_TRANSPORT.value:
        _print_ack(AckV1.response(command.paylod).error)
    elif command.cid == Commands.CMDID_MECH_SET_MODE.value:
        _print_ack(AckV1.response(command.paylod).error)
    elif command.cid == Commands.CMDID_SOLENOID_PULL.value:
        _print_ack(SolenoidPullV1.response(command.paylod).error)
    elif command.cid == Commands.CMDID_MECH_GET_STATE.value:
        state = MechStateV1.response(command.paylod)
        log.info(f"\n{state.describe()}")
    elif command.cid == Commands.CMDID_UNKNOWN.value:
        log.error("unknown command id")
    else:
        log.info(f"raw: {command.paylod.hex()}")


def _transport(action: int) -> Command:
    return MechTransportV1(action=action).request()


def _add_transport_parser(subparsers, name: str, action: int, help_text: str):
    parser = subparsers.add_parser(name, help=help_text)
    parser.set_defaults(builder=lambda _args: _transport(action))


def _build_parser() -> argparse.ArgumentParser:
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

    subparsers = parser.add_subparsers(dest='command', required=True)

    version = subparsers.add_parser('version', help='read firmware version')
    version.set_defaults(builder=lambda _args: VersionV1().request())

    state = subparsers.add_parser('state', help='read mechanism state')
    state.set_defaults(builder=lambda _args: MechStateV1().request())

    _add_transport_parser(subparsers, 'stop', ACTION_STOP, 'stop transport')
    _add_transport_parser(subparsers, 'play-fwd', ACTION_PLAY_FWD,
                          'play forward')
    _add_transport_parser(subparsers, 'play-rev', ACTION_PLAY_REV,
                          'play reverse')
    _add_transport_parser(subparsers, 'rewind-fwd', ACTION_REWIND_FWD,
                          'rewind forward')
    _add_transport_parser(subparsers, 'rewind-rev', ACTION_REWIND_REV,
                          'rewind reverse')

    set_mode = subparsers.add_parser('set-mode',
                                     help='request raw user mode 0..6')
    set_mode.add_argument('mode', type=int, choices=range(0, 7))
    set_mode.set_defaults(
        builder=lambda args: MechSetModeV1(mode=args.mode).request())

    solenoid = subparsers.add_parser('solenoid', help='pull solenoid')
    solenoid.add_argument('--pull-ms', type=int, default=400)
    solenoid.add_argument('--cooldown-ms', type=int, default=100)
    solenoid.set_defaults(
        builder=lambda args: SolenoidPullV1(pull_time_ms=args.pull_ms,
                                            cooldown_ms=args.cooldown_ms).request())

    return parser


def main():
    parser = _build_parser()
    args = parser.parse_args()

    try:
        adapter = SerialPortAdapter(args.port, args.baudrate, False)
        device = Device(adapter=adapter)
    except Exception as e:
        log.error(e)
        sys.exit(1)

    device.on_command_rx = _on_command_rx
    device.run()

    try:
        cmd = args.builder(args)
    except Exception as e:
        log.error(e)
        device.stop()
        sys.exit(1)

    device.send(cmd)

    for _ in range(1):
        time.sleep(1)
        log.info("...")
    device.stop()


if __name__ == "__main__":
    main()
