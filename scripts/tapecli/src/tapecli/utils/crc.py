from crc import Calculator, Configuration


def xcrc32(data: bytes, init: int = 0xFFFFFFFF) -> int:
    config = Configuration(
        width=32,
        polynomial=0x04c11db7,
        init_value=init,
        final_xor_value=0x00,
        reverse_input=False,
        reverse_output=False,
    )
    calculator = Calculator(config)
    return calculator.checksum(data)
