# -*- coding: utf-8 -*-
"""
Notifications
-------------

Example showing how to add notifications to a characteristic and handle the responses.

Updated on 2019-07-03 by hbldh <henrik.blidh@gmail.com>

"""

import asyncio

from bleak import BleakClient

from bt_config import *


class Btupdater:
    def __init__(self):
        self.data = None

    def noti_handler(self, sender, data):
        """Simple notification handler which prints the data received."""
        self.data = int.from_bytes(data, byteorder='little', signed=True)
        print("{0}: {1}".format(sender, self.data))


btlistener = Btupdater()

async def main(address, char_uuid):
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")

        # await client.start_notify(char_uuid, notification_handler)
        # await client.start_notify(char_uuid2, notification_handler)
        # await client.start_notify("beb5483e-36e1-4688-b7f5-ea07361b26a1", notification_handler)
        await client.start_notify(char_uuid, btlistener.noti_handler)
        await asyncio.sleep(DURATION)
        await client.stop_notify(char_uuid)


if __name__ == "__main__":
    asyncio.run(main(ADDRESS, CHARACTERISTIC_UUID3))