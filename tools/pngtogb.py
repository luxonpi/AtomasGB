import argparse
from PIL import Image
import numpy as np
from collections import Counter

# Constants
TILE_SIZE = 8
IMAGE_WIDTH = 160
IMAGE_HEIGHT = 144
TILES_X = IMAGE_WIDTH // TILE_SIZE  # 20 tiles horizontally
TILES_Y = IMAGE_HEIGHT // TILE_SIZE  # 18 tiles vertically

def find_target_colors(img_array):
   
    unique_colors = Counter(img_array.flatten())
    
    if len(unique_colors) <= 4:
        # If 4 or fewer colors, use them directly
        target_colors = sorted(unique_colors.keys())
        # If fewer than 4 colors, add some standard values
        while len(target_colors) < 4:
            new_color = 85 * len(target_colors)  # Add values at 0, 85, 170, 255
            if new_color not in target_colors:
                target_colors.append(new_color)
    else:
        # If more than 4 colors, use quartiles
        values = np.array(list(unique_colors.keys()))
        target_colors = [
            np.percentile(values, 0),   # Darkest
            np.percentile(values, 33),   # Dark gray
            np.percentile(values, 66),   # Light gray
            np.percentile(values, 100)   # Lightest
        ]
        target_colors = [int(c) for c in target_colors]
    
    target_colors.sort()
    return target_colors

def map_to_closest_color(value, target_colors):
    return min(target_colors, key=lambda x: abs(x - value))

def convert_tile_to_2bpp(tile):
    tile_bytes = []
    for row in tile:
        low_byte = 0
        high_byte = 0
        for i, pixel in enumerate(row):
            bit_position = 7 - i
            low_byte |= ((pixel & 1) << bit_position)
            high_byte |= (((pixel >> 1) & 1) << bit_position)
        tile_bytes.extend([low_byte, high_byte])
    return tuple(tile_bytes)

def convert_image_to_gb_tiles(image_path):
    
    img = Image.open(image_path).convert("L")
    if img.size != (IMAGE_WIDTH, IMAGE_HEIGHT):
        raise ValueError(f"Image must be {IMAGE_WIDTH}x{IMAGE_HEIGHT} pixels")
    
    img_array = np.array(img)
    target_colors = find_target_colors(img_array)
    vectorized_map = np.vectorize(lambda x: map_to_closest_color(x, target_colors))
    quantized = vectorized_map(img_array)
    
    # Map to GB colors (0-3)
    color_to_gb = {
        target_colors[3]: 0,  # Lightest → 0
        target_colors[2]: 1,  # Light gray → 1
        target_colors[1]: 2,  # Dark gray → 2
        target_colors[0]: 3   # Darkest → 3
    }
    
    gb_pixels = np.vectorize(color_to_gb.get)(quantized)
    
    # Process tiles
    tile_dict = {}
    tile_list = []
    tile_map = []
    
    # Split image into 8x8 tiles
    for ty in range(TILES_Y):
        row = []
        for tx in range(TILES_X):
            tile = gb_pixels[ty*8:(ty+1)*8, tx*8:(tx+1)*8]
            tile_data = convert_tile_to_2bpp(tile)
            
            if tile_data not in tile_dict:
                tile_dict[tile_data] = len(tile_list)
                tile_list.append(tile_data)
            row.append(tile_dict[tile_data])
        tile_map.append(row)
    
    # Generate C code
    tile_bytes = [b for tile in tile_list for b in tile]
    tile_indices = [idx for row in tile_map for idx in row]
    
    c_code = f"""

const unsigned char titlescreen_tiles[] = {{
    {",".join(f"0x{b:02X}" for b in tile_bytes)}
}};

const unsigned char titlescreen_map[{TILES_X * TILES_Y}] = {{
    {",".join(str(i) for i in tile_indices)}
}};
"""
    
    with open("titlescreen.h", "w") as f:
        f.write(c_code)
    
    print(f"\nConverted {len(tile_list)} unique tiles")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("image_path", help="")
    args = parser.parse_args()
    convert_image_to_gb_tiles(args.image_path)
