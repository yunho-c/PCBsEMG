import asyncio

from bleak import BleakClient

import time


class Btupdater:
    def __init__(self):
        self.data = [None] * 6

    def noti_handler(self, sender, data):
        """Simple notification handler which prints the data received."""
        dat = int.from_bytes(data, byteorder='little', signed=True)
        # print("{0}: {1}".format(sender, dat))
        self.data[utils.mapper(sender)] = dat


btlistener = Btupdater()

async def main(address, char_uuid, char_uuid2):
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")

        await client.start_notify(char_uuid, btlistener.noti_handler)
        await client.start_notify(char_uuid2, btlistener.noti_handler)
        while True:
            print(btlistener.data)
            # time.sleep(1) # flow control
            await asyncio.sleep(1) # flow control (try2)
        await client.stop_notify(char_uuid)


if __name__ == "__main__":
    asyncio.run(main(utils.ADDRESS, utils.CHARACTERISTIC_UUID, utils.CHARACTERISTIC_UUID2))
