import sys
import os

# Constants
EXPECTED_FILE_SIZE = 64 * 1024  # 64 kB
MIN_FF_BLOCK_LEN = 256  # Minimum length of FF block

def find_ff_blocks(data, min_len):
	blocks = []
	in_block = False
	block_start = None
	for pos, b in enumerate(data):
		if b == 0xFF:
			if not in_block:
				in_block = True
				block_start = pos
		else:
			if in_block:
				in_block = False
				if pos - block_start >= min_len:
					blocks.append((block_start, pos - 1))
	# Handle file ending with FF block
	if in_block and (len(data) - block_start >= min_len):
		blocks.append((block_start, len(data) - 1))
	return blocks

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

# Count total number of 0xFF bytes
ff_count = data.count(0xFF)

# Calculate and print results
ff_blocks = find_ff_blocks(data, MIN_FF_BLOCK_LEN)
print("---------------------------------")
print(f"Found {len(ff_blocks)} contiguous 0xFF blocks of at least {MIN_FF_BLOCK_LEN} bytes:")

total_length = 0

for start, end in ff_blocks:
	current_block_length = end - start + 1
	total_length += current_block_length
	print(f"Block from 0x{start:0>4X} to 0x{end:0>4X} (length {current_block_length} bytes)")

print("---------------------------------")
print(f"Total number of 0xFF bytes in {input_filename} is {ff_count} bytes.")
print(f"Total length of all 0xFF blocks is {total_length} bytes.")
print(f"Difference is {(ff_count - total_length)} bytes.")

print(f"ROM usage is approximately {(((EXPECTED_FILE_SIZE - total_length) / EXPECTED_FILE_SIZE) * 100):.0f}%.")

print("Done.")

