# Galois Ring

### Preliminary Libraries
Bazel

### Run
To build the project, use
```bash
bazel build //main:galois-ring 
```
 The workaround `--sandbox_add_mount_pair=/tmp` is used for the issue of loading external gcc toolchain: https://github.com/bazelbuild/bazel/issues/20533. 

Current test tree looks like:

├── basering
├── galoisring
└── z2k
    ├── BUILD
    ├── expected_out_add
    ├── expected_out_multiply
    ├── z2k-update-test.sh
    ├── z2k.cc
    └── z2k.sage

To run the tests, first run the corresponding bash script,
then use 
```bash
bazel test //path/to/your:test_target 

```
for example, to test the $Z210$, use
```bash
bazel test //test/z2k:z2k-test \
    --define k=10 \
    --define testcase_num=100 
```
