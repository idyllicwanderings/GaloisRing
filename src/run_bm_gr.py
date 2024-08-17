import re

k = 32
d1 = 1
d2 = 2
d3 = 0
d4 = 0



def modify_cpp_file(file_path, k, d1, d2 = 0, d3 = 0, d4 = 0):
    with open(file_path, 'r') as file:
        content = file.read()

    if k is not None:
        content = re.sub(r'#define grt_k \d+', f'#define grt_k {k}', content)
    if d1 is not None:
        content = re.sub(r'#define grt_d1 \d+', f'#define grt_d1 {d1}', content)
        
    if d2 != 0:
        content =re.sub(r'#define grt_d2 \d+', f'#define grt_d2 {d2}', content)
    else:
        content = re.sub(r'#define grt_d2 \d+', f'#define grt_d2 0', content)
    
    if d3 != 0:
        content = re.sub(r'#define grt_d3 \d+', f'#define grt_d3 {d3}', content)
    else:
        content = re.sub(r'#define grt_d3 \d+', f'#define grt_d3 0', content)
 
    grt_template = "using GRTins = "
    if d4 != 0:
        grt_template += f"GRT1e<GRT1e<GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>, grt_d3>, grt_d4>>;"
    elif d3 != 0:
        grt_template += f"GRT1e<GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>, grt_d3>>;"
    elif d2 != 0:
        grt_template += f"GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>>;"
    else:
        grt_template += f"GR1e<k, d1>;"

    content = re.sub(r'using GRTins = GRT1e<.*;', grt_template, content)

    with open(file_path, 'w') as file:
        file.write(content)


file_path = 'run_benchmark.cc' 

modify_cpp_file(file_path, k, d1, d2, d3, d4)
