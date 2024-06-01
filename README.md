# Galois Ring

To build the project, use
```bash
bazel build //main:galois-ring 
```
 The workaround `--sandbox_add_mount_pair=/tmp` is used for the issue of loading external gcc toolchain: https://github.com/bazelbuild/bazel/issues/20533. 

### Test
The current test tree looks like:

To run the tests, use 
```bash
bazel test //path:your-test 
```
for example,
```bash
bazel test //test/z2k:z2k-test 
```
