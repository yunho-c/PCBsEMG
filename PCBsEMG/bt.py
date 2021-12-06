# -*- coding: utf-8 -*-
"""
Notifications
-------------

Example showing how to add notifications to a characteristic and handle the responses.

Updated on 2019-07-03 by hbldh <henrik.blidh@gmail.com>

"""

import sys
import asyncio
import platform

from bleak import BleakClient


DURATION = 1000
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a1"
CHARACTERISTIC_UUID2 = "beb5483e-36e1-4688-b7f5-ea07361b26a2"
CHARACTERISTIC_UUID3 = "beb5483e-36e1-4688-b7f5-ea07361b26a3"
CHARACTERISTIC_UUID4 = "beb5483e-36e1-4688-b7f5-ea07361b26a4"
CHARACTERISTIC_UUID5 = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
CHARACTERISTIC_UUID6 = "beb5483e-36e1-4688-b7f5-ea07361b26a6"
ADDRESS = "98:f4:ab:6c:d9:76"


def notification_handler(sender, data):
    """Simple notification handler which prints the data received."""
    print("{0}: {1}".format(sender, int.from_bytes(data, byteorder='little', signed=True)))


async def main(address, char_uuid, char_uuid2):
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")

        # await client.start_notify(char_uuid, notification_handler)
        # await client.start_notify(char_uuid2, notification_handler)
        # await client.start_notify("beb5483e-36e1-4688-b7f5-ea07361b26a1", notification_handler)
        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
        await asyncio.sleep(DURATION)
        await client.stop_notify(char_uuid)


if __name__ == "__main__":
    asyncio.run(main(ADDRESS, CHARACTERISTIC_UUID, CHARACTERISTIC_UUID2))