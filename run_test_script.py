import subprocess, json
from jinja2 import Template


def run_sage_generate(testcase_nums, k, d1, d2=None, op="mul"):
    if d2 is not None:
        args = ["sage", "genGR.sage", str(testcase_nums), str(k), str(d1), str(d2), op]
    else:
        args = ["sage", "genGR.sage", str(testcase_nums), str(k), str(d1), op]
    print(f"Running: {' '.join(args)}")
    subprocess.run(args, check=True)
    
    
def render_cpp(k, d, template_file="main_template.cpp"):
    with open(template_file) as f:
        template = Template(f.read())
    cpp_code = template.render(K=k, D=d)
    # TODO: modiy the main.cpp to what test cpp file should be
    with open("main.cpp", "w") as f:
        f.write(cpp_code)

def compile_cpp():
    subprocess.run(["rm", "-rf", "build"], check=True)
    subprocess.run(["cmake", "-B", "build"], check=True)
    subprocess.run(["cmake", "--build", "build"], check=True)

    subprocess.run(["./build/grperformance"], check=True)
    
def run_cpp():
    result = subprocess.run(["./main"], capture_output=True, text=True)
    print(result.stdout)
    return result.stdout

def parse_time(output):
    for line in output.splitlines():
        if "Elapsed" in line:
            return int(line.split()[-2])

def run_full_test(k, d, N):
    print(f"=== Running test for k={k}, d={d}, N={N} ===")
    run_sage_generate(k, d, N)
    render_cpp(k, d)
    compile_cpp()
    output = run_cpp()
    t = parse_time(output)
    print(f"Time: {t} us")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--k", type=int, required=True)
    parser.add_argument("--d", type=int, required=True)
    parser.add_argument("--n", type=int, default=10)
    args = parser.parse_args()
    run_full_test(args.k, args.d, args.n)
