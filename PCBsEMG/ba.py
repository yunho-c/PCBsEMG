

# print(bytearray(b'D\x8bl?').decode('base16'))
# print(bytearray(b'D\x8bl?').decode('base64'))
# print(bytearray(b'D\x8bl?').decode('ascii'))
# print(bytearray(b'D\x8bl?').decode('utf-8'))
data = list(bytearray(b'\xe4\x0c\x00\x00'))
[f'{i:02x}' for i in data]
print(int.from_bytes(data, byteorder='little', signed=False))
