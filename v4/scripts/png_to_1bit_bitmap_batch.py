
#!/usr/bin/env python3

import os
from PIL import Image

# Change this to your folder containing .png files
BITMAPS_FOLDER = "bitmaps"
OUTPUT_FILENAME = "bitmaps.h"

# Threshold for converting grayscale pixel to a 1-bit value.
# Anything >= 128 is considered "1" (drawn), else "0" (transparent).
GREY_THRESHOLD = 128

ALPHA_THRESHOLD = 128

def sanitize_name(filename):
    """
    Remove extension, replace non-alphanumeric chars with underscore,
    and prefix with 'digit_'.
    Example: "6_29x14.png" -> "digit_6_29x14"
    """
    base = os.path.splitext(filename)[0] # remove ".png"
    return "digit_" + base


def process_image_to_array(image_path):
    """
    Open the image, convert to 1-bit array, and return (name, width, height, byte_list).
    """
    # Derive array name from the filename
    filename = os.path.basename(image_path)
    array_name = sanitize_name(filename)

    # Load the image using PIL
    img = Image.open(image_path).convert('RGBA')  # convert to grayscale
    w, h = img.size

    # Convert to bits
    # Each row is (w + 7) // 8 bytes
    byte_width = (w + 7) // 8
    byte_list = []

    for y in range(h):
        row_byte_value = 0
        bit_index = 7  # start from MSB
        for x in range(w):
            r, g, b, a = img.getpixel((x, y))
            if a < ALPHA_THRESHOLD:  
                bit_val = 0  
            else:  
                # Simple luminosity approximation  
                gray_val = int(0.299*r + 0.587*g + 0.114*b)  
                bit_val = 1 if gray_val < GREY_THRESHOLD else 0
            row_byte_value |= (bit_val << bit_index)

            if bit_index == 0:
                byte_list.append(row_byte_value)
                row_byte_value = 0
                bit_index = 7
            else:
                bit_index -= 1

        # If the last byte in the row didn't get filled up completely, append it
        if bit_index != 7:
            byte_list.append(row_byte_value)

    return array_name, w, h, byte_list


def main():
    # Gather all PNG files in the specified folder
    png_files = [f for f in os.listdir(BITMAPS_FOLDER) 
                 if f.lower().endswith(".png")]

    output_lines = []
    output_lines.append("#pragma once")
    output_lines.append('#include <Arduino.h>')
    output_lines.append("")  # blank line

    for fname in sorted(png_files):
        image_path = os.path.join(BITMAPS_FOLDER, fname)
        array_name, w, h, byte_list = process_image_to_array(image_path)

        # Prepare the C array
        array_size = len(byte_list)
        line = f"uint8_t PROGMEM {array_name}[{array_size}] = {{"
        output_lines.append(f"// Image: {fname}, size: {w}x{h}")
        output_lines.append(f"// Bytes per row = {(w + 7)//8}, total bytes = {array_size}")
        output_lines.append(line)

        # Convert each byte in byte_list to binary-like format (e.g. 0b11110000)
        row_content = []
        for b in byte_list:
            row_content.append(f"0b{b:08b}")

        # Join them so each row of output isn't too long
        INDENT = "  "
        max_per_line = 12
        for i in range(0, len(row_content), max_per_line):
            chunk = row_content[i:i+max_per_line]
            output_lines.append(INDENT + ", ".join(chunk) + ",")

        output_lines.append("};\n")

    with open(OUTPUT_FILENAME, "w") as f:
        for line in output_lines:
            f.write(line + "\n")

    print(f"Generated '{OUTPUT_FILENAME}' with {len(png_files)} bitmap(s).")

if __name__ == "__main__":
    main()
