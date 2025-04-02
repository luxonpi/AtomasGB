import math

def generate_trig_tables(resolution=100):

    sin_values = []
    cos_values = []
    
    # Calculate step size for angles
    step = 2 * math.pi / resolution
    radius = 40
    
    # Generate values
    for i in range(resolution):
        angle = i * step
        sin_values.append(round(math.sin(angle) * radius))
        cos_values.append(round(math.cos(angle) * radius))
    
    return sin_values, cos_values

def print_table(values, name):

    print(f"const int8_t {name}[{len(values)}] = {{")
    
    for i in range(0, len(values), 10):
        line = values[i:i+10]
        print("    " + ", ".join(f"{x:3d}" for x in line) + ",")
    
    print("};")

def main():

    sin_table, cos_table = generate_trig_tables(100)
    print("// Sine table")
    print_table(sin_table, "sin_table")
    print("\n// Cosine table")
    print_table(cos_table, "cos_table")
    

if __name__ == "__main__":
    main()
