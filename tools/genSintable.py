import math

def generate_trig_tables(resolution=100):
    """
    Generate sine and cosine tables with specified resolution.
    Returns values scaled to 64 (similar to the original table).
    """
    # Generate sine and cosine values
    sin_values = []
    cos_values = []
    
    # Calculate step size for angles
    step = 2 * math.pi / resolution
    radius = 32
    
    # Generate values
    for i in range(resolution):
        angle = i * step
        sin_values.append(round(math.sin(angle) * radius))
        cos_values.append(round(math.cos(angle) * radius))
    
    return sin_values, cos_values

def print_table(values, name):
    """Print the table in C array format"""
    print(f"const int8_t {name}[{len(values)}] = {{")
    
    # Print 10 values per line
    for i in range(0, len(values), 10):
        line = values[i:i+10]
        print("    " + ", ".join(f"{x:3d}" for x in line) + ",")
    
    print("};")

def main():
    # Generate tables with 100 points (same as original)
    sin_table, cos_table = generate_trig_tables(100)
    
    print("// Sine table")
    print_table(sin_table, "sin_table")
    print("\n// Cosine table")
    print_table(cos_table, "cos_table")

if __name__ == "__main__":
    main()
