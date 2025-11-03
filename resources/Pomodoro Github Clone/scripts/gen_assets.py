"""
This script processes the assets directory to generate relevant C files
"""

import os
import sys
import shutil
from PIL import Image

# Directories
ICON_INPUT_DIR = sys.argv[1] if len(sys.argv) > 1 else "assets/icons"
IMAGE_INPUT_DIR = "assets/images"  # Background images (800x480 PNGs)
OUTPUT_DIR = "src"
ICONS_DIR = os.path.join(OUTPUT_DIR, "icons")
IMAGES_DIR = os.path.join(OUTPUT_DIR, "images")
ICONS_HEADER_FILE = os.path.join(OUTPUT_DIR, "icons.h")
ICONS_SOURCE_FILE = os.path.join(ICONS_DIR, "icons.cpp")
IMAGES_AGGREGATE_HEADER_FILE = os.path.join(OUTPUT_DIR, "images.h")


def image_to_c_array(image):
    width, height = image.size
    byte_data = []
    for y in range(height):
        byte_val, bit_count = 0, 0
        for x in range(width):
            if image.getpixel((x, y)) == 0:  # BLACK pixel
                byte_val |= 1 << (7 - bit_count)
            bit_count += 1
            if bit_count == 8:
                byte_data.append(byte_val)
                byte_val, bit_count = 0, 0
        if bit_count > 0:
            byte_data.append(byte_val)
    return byte_data


def process_icon_image(image_path):
    img = Image.open(image_path)
    if img.mode in ("RGBA", "LA"):
        img = img.convert("RGBA")
        new_img = Image.new("RGB", img.size, (255, 255, 255))
        new_img.paste(img, mask=img.split()[3])
        img = new_img.convert("L")
    else:
        img = img.convert("L")
    sizes = [192, 128, 64, 48]
    c_arrays = {}
    for size in sizes:
        resized = img.resize((size, size), Image.LANCZOS)
        bw_img = resized.convert("1", dither=Image.FLOYDSTEINBERG)
        c_arrays[size] = image_to_c_array(bw_img)
    return c_arrays


def process_icons_directory(input_directory):
    if not os.path.exists(input_directory):
        print(f"❌ Directory '{input_directory}' does not exist.")
        return

    files = [
        f for f in os.listdir(input_directory) if f.lower().endswith((".bmp", ".png"))
    ]
    if not files:
        print(f"⚠ No BMP or PNG files found in '{input_directory}'.")
        return

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    os.makedirs(ICONS_DIR, exist_ok=True)

    icon_names = []
    icon_data_entries = []

    with open(ICONS_SOURCE_FILE, "w") as cpp_file:
        cpp_file.write('#include "icons.h"\n\n')
        for filename in files:
            file_path = os.path.join(input_directory, filename)
            icon_slug = os.path.splitext(filename)[0]
            print(f"🔄 Processing icon {filename}...")
            c_arrays = process_icon_image(file_path)
            icon_names.append(icon_slug)
            for size, data in c_arrays.items():
                cpp_file.write(
                    f"const unsigned char icon_{icon_slug}_{size}[] = {{\n    "
                )
                for i, byte in enumerate(data):
                    cpp_file.write(f"0x{byte:02X}, ")
                    if (i + 1) % 12 == 0:
                        cpp_file.write("\n    ")
                cpp_file.write("\n};\n\n")
            icon_data_entries.append(
                f"Icon icon_{icon_slug}(icon_{icon_slug}_192, icon_{icon_slug}_128, icon_{icon_slug}_64, icon_{icon_slug}_48);"
            )
        cpp_file.write("\n".join(icon_data_entries))
        cpp_file.write("\n")

    with open(ICONS_HEADER_FILE, "w") as header_file:
        header_file.write("#ifndef ICONS_H\n#define ICONS_H\n\n")
        header_file.write('#include "icon.h"\n\n')
        for icon_slug in icon_names:
            header_file.write(f"extern Icon icon_{icon_slug};\n")
        header_file.write("\n#endif // ICONS_H\n")

    print(f"✔ Icons header file saved: {ICONS_HEADER_FILE}")
    print(f"✔ Icons source file saved: {ICONS_SOURCE_FILE}")


def process_background_image(image_path):
    img = Image.open(image_path)
    if img.mode in ("RGBA", "LA"):
        img = img.convert("RGBA")
        new_img = Image.new("RGB", img.size, (255, 255, 255))
        new_img.paste(img, mask=img.split()[3])
        img = new_img.convert("L")
    else:
        img = img.convert("L")
    # Background images are assumed to be 800x480; no resizing.
    bw_img = img.convert("1", dither=Image.FLOYDSTEINBERG)
    return image_to_c_array(bw_img)


def process_images_directory(input_directory):
    if not os.path.exists(input_directory):
        print(f"❌ Directory '{input_directory}' does not exist.")
        return

    files = [f for f in os.listdir(input_directory) if f.lower().endswith(".png")]
    if not files:
        print(f"⚠ No PNG files found in '{input_directory}'.")
        return

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    os.makedirs(IMAGES_DIR, exist_ok=True)

    image_slugs = []

    for filename in files:
        file_path = os.path.join(input_directory, filename)
        image_slug = os.path.splitext(filename)[0]
        image_slugs.append(image_slug)
        print(f"🔄 Processing background image {filename}...")
        c_array = process_background_image(file_path)
        header_filename = f"image_{image_slug}.h"
        header_filepath = os.path.join(IMAGES_DIR, header_filename)
        include_guard = f"IMAGE_{image_slug.upper()}_H"
        with open(header_filepath, "w") as header_file:
            header_file.write(f"#ifndef {include_guard}\n#define {include_guard}\n\n")
            header_file.write(f"const unsigned char image_{image_slug}[] = {{\n    ")
            for i, byte in enumerate(c_array):
                header_file.write(f"0x{byte:02X}, ")
                if (i + 1) % 12 == 0:
                    header_file.write("\n    ")
            header_file.write("\n};\n\n")
            header_file.write(f"#endif // {include_guard}\n")
        print(f"✔ Background image header saved: {header_filepath}")

    # Create a single aggregated header for all background images.
    with open(IMAGES_AGGREGATE_HEADER_FILE, "w") as agg_header:
        agg_header.write("#ifndef IMAGES_H\n#define IMAGES_H\n\n")
        for slug in image_slugs:
            agg_header.write(f'#include "images/image_{slug}.h"\n')
        agg_header.write("\n#endif // IMAGES_H\n")
    print(f"✔ Aggregated images header saved: {IMAGES_AGGREGATE_HEADER_FILE}")


def process_font(font_name, sizes):
    """Process a font file in multiple sizes using fontconvert."""
    font_path = os.path.join("assets/fonts", font_name)
    if not os.path.exists(font_path):
        print(f"❌ Font file not found: {font_path}")
        return

    output_dir = os.path.join(OUTPUT_DIR, "fonts")
    os.makedirs(output_dir, exist_ok=True)

    for size in sizes:
        # Replace hyphens with underscores in the output filename
        base_name = os.path.splitext(font_name)[0].replace("-", "_")
        output_name = f"{base_name}{size}pt7b"
        output_path = os.path.join(output_dir, f"{output_name}.h")

        # Run fontconvert tool
        cmd = f"fontconvert {font_path} {size} > {output_path}"
        ret = os.system(cmd)
        if ret == 0:
            print(f"✔ Generated font header: {output_path}")
        else:
            print(f"❌ Failed to generate font: {output_path}")


def process_fonts():
    print("🔄 Processing fonts...")

    # Clean up existing fonts directory
    fonts_dir = os.path.join(OUTPUT_DIR, "fonts")
    if os.path.exists(fonts_dir):
        print(f"🗑 Removing existing fonts directory: {fonts_dir}")
        shutil.rmtree(fonts_dir)

    # Process only the fonts that are actually used in the project
    fonts_to_process = {
        "FunnelDisplay-Regular.ttf": [14],
        "FunnelDisplay-Bold.ttf": [18, 24, 32, 48, 60],
        "HelvetiPixel.ttf": [16, 24],
    }

    for font_name, sizes in fonts_to_process.items():
        process_font(font_name, sizes)


# Process icons, background images, and fonts
if __name__ == "__main__":
    process_icons_directory(ICON_INPUT_DIR)
    process_images_directory(IMAGE_INPUT_DIR)
    process_fonts()
