import asyncio

from bleak import BleakClient

from bt_config import *


class Btupdater:
    def __init__(self):
        self.data = [None] * 6

    def noti_handler(self, sender, data):
        """Simple notification handler which prints the data received."""
        data = int.from_bytes(data, byteorder='little', signed=True)
        # print("{0}: {1}".format(sender, self.data))
        self.data[mapper(sender)] = data


btlistener = Btupdater()

async def main(address, char_uuid, char_uuid2):
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")

        await client.start_notify(char_uuid, btlistener.noti_handler)
        await client.start_notify(char_uuid2, btlistener.noti_handler)
        await asyncio.sleep(DURATION)
        await client.stop_notify(char_uuid)


if __name__ == "__main__":
    asyncio.run(main(ADDRESS, CHARACTERISTIC_UUID, CHARACTERISTIC_UUID2))