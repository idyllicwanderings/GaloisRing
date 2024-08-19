import re

# parameters for the GR
k = 64          # define the k, d for GR
d1 = 8
d2 = 0          # set to 0 if not used
d3 = 0
d4 = 0

lift_ds = 3     # lift from GR(Z2k, d_small) to GR(Z2k, d_large)
lift_dl = 24   


def modify_cpp_file(file_path, k, d1, d2 = 0, d3 = 0, d4 = 0):
    with open(file_path, 'r') as file:
        content = file.read()
    if k is not None:
        content = re.sub(r'#define gr_k \d+', f'#define gr_k {k}', content)
    if d1 is not None:
        content = re.sub(r'#define gr_d1 \d+', f'#define gr_d1 {d1}', content)
    if d2 != 0:
        content =re.sub(r'#define gr_d2 \d+', f'#define gr_d2 {d2}', content)
    else:
        content = re.sub(r'#define gr_d2 \d+', f'#define gr_d2 0', content)
    if d3 != 0:
        content = re.sub(r'#define gr_d3 \d+', f'#define gr_d3 {d3}', content)
    else:
        content = re.sub(r'#define gr_d3 \d+', f'#define gr_d3 0', content)
    if d4 != 0:
        content = re.sub(r'#define gr_d4 \d+', f'#define gr_d4 {d4}', content)
    else:
        content = re.sub(r'#define gr_d4 \d+', f'#define gr_d4 0', content)
    if lift_ds != 0:
        content = re.sub(r'#define lift_d1 \d+', f'#define lift_d1 {lift_ds}', content)
    else:
        content = re.sub(r'#define lift_d1 \d+', f'#define lift_d1 0', content)
    if lift_dl != 0:
        content = re.sub(r'#define lift_d2 \d+', f'#define lift_d2 {lift_dl}', content)
    else:
        content = re.sub(r'#define lift_d2 \d+', f'#define lift_d2 0', content)     
    gr_template = "using GRTins = "
    if d4 != 0:
        gr_template += f"GRT1e<GRT1e<GRT1e<GR1e<gr_k, gr_d1>, gr_d2>, gr_d3>, gr_d4>;"
    elif d3 != 0:
        gr_template += f"GRT1e<GRT1e<GR1e<gr_k, gr_d1>, gr_d2>, gr_d3>;"
    elif d2 != 0:
        gr_template += f"GRT1e<GR1e<gr_k, gr_d1>, gr_d2>;"
    else:
        gr_template += f"GR1e<gr_k, gr_d1>;"

    content = re.sub(r'using GRTins = .*;', gr_template, content)

    with open(file_path, 'w') as file:
        file.write(content)


file_path = 'benchmark_GR.cc' 

modify_cpp_file(file_path, k, d1, d2, d3, d4)
print(" ======================================== Modified file ended!!!! ========================================")
