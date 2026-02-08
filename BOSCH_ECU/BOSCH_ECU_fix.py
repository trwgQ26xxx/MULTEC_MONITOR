import sys
import os
import hashlib

# Constants
EXPECTED_FILE_SIZE = 64 * 1024  # 64 KB
EXPECTED_SHA256 = "2c921678329497693b89186f869707b8b2901983050a4219cdd91542d5ccd518"

# Get input filename from first command line argument
if len(sys.argv) < 2:
	print(f"Usage: python {sys.argv[0]} <input_file>")
	sys.exit(1)

input_filename = sys.argv[1]

# Check if file exists
if not os.path.isfile(input_filename):
	print(f"Error: File '{input_filename}' does not exist.")
	sys.exit(1)

# Check if file has expected size
file_size = os.path.getsize(input_filename)
if file_size != EXPECTED_FILE_SIZE:
	print(f"Error: File size is {file_size} bytes, expected {EXPECTED_FILE_SIZE} bytes.")
	sys.exit(1)

# Read file data
with open(input_filename, "rb") as f:
	data = bytearray(f.read())

# Check if file has expected SHA256 hash
sha256 = hashlib.sha256(data).hexdigest()
if sha256 != EXPECTED_SHA256:
	print(f"Error: File SHA256 hash is {sha256}, expected {EXPECTED_SHA256}.")
	sys.exit(1)

# Apply patches
data[0x5142] = 0x01
data[0x524F] = 0x1F
data[0x5253] = 0x80
data[0x5254] = 0x16
data[0x5255] = 0x00
data[0x5256] = 0x00
data[0x5257] = 0x00
data[0x5258] = 0x00
data[0x5259] = 0x00
data[0x525A] = 0x00
data[0x525B] = 0x00
data[0x525C] = 0x00
data[0x525D] = 0x00
data[0x525E] = 0x00
data[0x525F] = 0x00
data[0x5260] = 0x00
data[0x5261] = 0x00
data[0x5262] = 0x00
data[0x5263] = 0x00
data[0x5264] = 0x00
data[0x5265] = 0x00
data[0x5266] = 0x00
data[0x5267] = 0x00
data[0x5268] = 0x00
data[0x5269] = 0x00
data[0x526A] = 0x00
data[0x5270] = 0x02
data[0x52B3] = 0x01
data[0xDF8C] = 0x01
data[0xDF8D] = 0x8A

# Write patched data to new file
output_filename = os.path.join(os.path.dirname(input_filename), "patched_" + os.path.basename(input_filename))
with open(output_filename, "wb") as f:
	f.write(data)

print(f"Patched file created: {output_filename}")

print("Done.")

