DURATION = 1000
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a1"
CHARACTERISTIC_UUID2 = "beb5483e-36e1-4688-b7f5-ea07361b26a2"
CHARACTERISTIC_UUID3 = "beb5483e-36e1-4688-b7f5-ea07361b26a3"
CHARACTERISTIC_UUID4 = "beb5483e-36e1-4688-b7f5-ea07361b26a4"
CHARACTERISTIC_UUID5 = "63bac001-bee5-4148-b8c7-220305bada25"
CHARACTERISTIC_UUID6 = "b2465de1-6a70-4ca8-acf6-cd1cd0fa5d46"
ADDRESS = "98:f4:ab:6c:d9:76"

def mapper(handle): # need to configure!
    print(handle)
    if handle == 41:
        return 1
    if handle == 44:
        return 2
    if handle == 47:
        return 3
    if handle == 50:
        return 4
    if handle == 53:
        return 5
    if handle == 56:
        return 6
    # jonna inefficient