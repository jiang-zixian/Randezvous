# The Randezvous Project

This repository contains and organizes code that we used to evaluate Randezvous.

ps:This is the documentation modified for SEU's srtp.  <font color="red"> We(srtp) use wsl but not vm. So,The installation details are somewhat different.</font>

## Brief Introduction

Randezvous is a software defense against control-flow hijacking attacks on
embedded microcontroller (MCU) systems, built up on diversification
and eXecute-Only Memory (XOM).
Unlike other control-flow hijacking defenses on MCU systems, Randezvous assumes
a broader threat model in which attackers can not only use buffer overflows to
corrupt memory but also use buffer overreads to leak memory content.
In order to mitigate such powerful attacks, Randezvous employs several novel
techniques to protect in-memory control data from leakage and corruption.
It also slows down brute force attacks to make randomization effective even
with MCUs' limited memory.

For more details, please check out
[our ACSAC paper](https://doi.org/10.1145/3564625.3567970).
To cite Randezvous, you can use the following BibTeX entry:
```bibtex
@inproceedings{Randezvous:ACSAC22,
  author = {Shen, Zhuojia and Dharsee, Komail and Criswell, John},
  title = {Randezvous: Making Randomization Effective on {MCUs}},
  booktitle = {Proceedings of the 38th Annual Computer Security Applications Conference},
  series = {ACSAC '22},
  year = {2022},
  isbn = {978-1-4503-9759-9},
  location = {Austin, TX, USA},
  pages = {28--41},
  numpages = {14},
  url = {https://doi.org/10.1145/3564625.3567970},
  doi = {10.1145/3564625.3567970},
  publisher = {ACM},
  address = {New York, NY, USA},
}
```

## Directory Hierarchy

```shell
Randezvous
|-- build                    # Directory for building LLVM, Newlib, and compiler-rt
|   |-- build.llvm.sh        # Script to build LLVM
|   |-- build.newlib.sh      # Script to build Newlib
|   |-- build.compiler.rt.sh # Script to build compiler-rt
|
|-- data                     # Directory containing generated experiment data (to
|                            # be created by our scripts)
|
|-- debug                    # Directory containing compiled binaries and build
|                            # logs (to be created by our scripts)
|
|-- llvm-project             # A submodule containing source code of LLVM and
|                            # Randezvous passes
|
|-- newlib-cygwin            # A submodule containing source code of Newlib
|
|-- scripts                  # Directory containing scripts
|   |-- import.sh            # Script to import projects into IDE
|   |-- hal.sh               # Script to compile HAL library for MIMXRT685-EVK
|   |-- mbedtls.sh           # Script to compile MbedTLS library
|   |-- beebs.sh             # Script to compile/debug/run BEEBS benchmarks
|   |-- coremark-pro.sh      # Script to compile/debug/run CoreMark-Pro benchmarks
|   |-- mbedtls-benchmark.sh # Script to compile/debug/run MbedTLS-Benchmark
|   |-- pinlock.sh           # Script to compile/debug/run PinLock
|   |-- sdcard_fatfs.sh      # Script to compile/debug/run FatFs-SD
|   |-- shell.sh             # Script to compile/debug/run LED-Shell
|   |-- exploit.sh           # Script to compile/debug/run proof-of-concept exploit
|   |-- cve.sh               # Script to compile/debug/run CVE exploit
|   |-- gen_csv.py           # Script to collect experiment results into CSV files
|
|-- workspace                # Directory containing source code
|   |-- mimxrt685s           # Source code of HAL library for MIMXRT685-EVK
|   |-- mbedtls              # Source code of MbedTLS library
|   |-- beebs                # Source code of BEEBS benchmarks
|   |-- coremark-pro         # Source code of CoreMark-Pro benchmarks
|   |-- mbedtls-benchmark    # Source code of MbedTLS-Benchmark
|   |-- pinlock              # Source code of PinLock
|   |-- sdcard_fatfs         # Source code of FatFs-SD
|   |-- shell                # Source code of LED-Shell
|   |-- exploit              # Source code of application used in proof-of-concept exploit
|   |-- cve                  # Source code of application used in CVE exploit
|
|-- README.md                # This README file
```

## Prerequisites

- We assume the host operating system is Linux.
  Other operating systems may
  work but were not tested.

- We use CMake, Ninja, and Clang to build the LLVM-based Randezvous compiler,
  so `cmake`, `ninja`, and `clang` of appropriate versions must be found in
  `PATH`.
- We use the Randezvous compiler to build Newlib and compiler-rt, so make sure
  that common development tools needed to build Newlib and compiler-rt for
  bare-metal ARM environments (such as `arm-none-eabi-gcc` and `make`) are
  there in `PATH`.
  In particular, one of our build scripts uses
  `arm-none-eabi-gcc` to find out where a bare-metal ARM `libgcc` is installed.
- We use [MCUXpresso IDE](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE)
  to build, run, and debug programs and require the IDE to be installed at
  `/usr/local`, `/opt`, or `$HOME`.
- We use an [NXP MIMXRT685-EVK](https://www.nxp.com/part/MIMXRT685-EVK) board
  to run programs and assume a
  readable/writable character device `/dev/ttyACM0` is connected to the board's
  serial port after plugging in the board.
- We use GNU Screen to receive program output from the board's serial port, so
  `screen` of an appropriate version must be found in `PATH`.
- We use GDB to debug ELF binaries and have debugging support included in our
  scripts.
  If you would like to use our script for debugging, make sure either
  `gdb-multiarch` or `arm-none-eabi-gdb` is there in `PATH`.

## Detailed Steps

### Set up the Environment

The following steps will set up the environment from scratch.
They only need
to be done once.

1. Download [MCUXpresso IDE](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE)
   and install it at `/usr/local`, `/opt`, or `$HOME`.

   For srtp:.bin file(I call it .binFile) can be downloaded from [here](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE).then install ide use
   ```bash
    sudo chmod a+x .binFile
    ./.binFile
   ```
   However,WSL is missing a critical thing for installing the ide... the ubuntu desktop.  Even though I only needed the command-line versions of the tool installed, there's no way to do that by itself since only one installation executable is provided by NXP.  The solution for me was to get a full blown ubuntu desktop running in WSL, and then installing the tool.  Once installed, the command-line tools can be used without a desktop session.
   I was able to get this to work by installing MCUXpresso IDE after installing the full-blown desktop version of Ubuntu 20.04 LTS in my WSL2.VcXsrv is a good choose.You can follow [this](https://www.bilibili.com/read/cv11143517/#:~:text=%E5%AE%89%E8%A3%85%E5%9B%BE%E5%BD%A2%E5%8C%96%E7%95%8C%E9%9D%A2%20%E4%B8%8B%E8%BD%BD%E5%B9%B6%E5%AE%89%E8%A3%85VcXsrv%20Next%E4%B8%8B%E4%B8%80%E6%AD%A5,%E5%AE%89%E8%A3%85%E5%AE%8C%E6%88%90%E5%90%8E%E7%82%B9%E5%87%BBClose%E5%85%B3%E9%97%AD%20%E5%AE%89%E8%A3%85%E5%AE%8C%E5%90%8E%E6%A1%8C%E9%9D%A2%E4%BC%9A%E7%94%9F%E6%88%90%E4%B8%80%E4%B8%AAXLaunch%E7%9A%84%E5%BF%AB%E6%8D%B7%E6%96%B9%E5%BC%8F%EF%BC%8C%E7%82%B9%E5%87%BB%E8%BF%90%E8%A1%8C%20%E9%80%89%E6%8B%A9%E6%98%BE%E7%A4%BA%E6%96%B9%E5%BC%8F%EF%BC%8C%E6%88%91%E9%80%89%E7%9A%84%E6%98%AF%E5%A4%A7%E7%AA%97%E5%8F%A3%E6%98%BE%E7%A4%BA%20%E7%9B%B4%E6%8E%A5%E4%B8%8B%E4%B8%80%E6%AD%A5).After that you must open terminal from the VcXsrv desktop and run the installing command above.
   Head up.If you can't download the ide all at once, you'll need to run these commands before the next download:
   ```bash
    sudo dpkg -r mcuxpressoide
    sudo apt-get install -f
   ```
   Please try more and best wishes.
2. Clone this repository.
   ```shell
   git clone --recurse-submodules https://github.com/URSec/Randezvous.git
   ```
3. Build the Randezvous compiler.
   ```shell
   cd Randezvous && ./build/build.llvm.sh
   ```
   Note that all our scripts (in the `build` and `scripts` directories) are
   CWD-agnostic; each of them can be run from any working directory and would
   have the same outcome.
   After `./build/build.llvm.sh` finishes, the
   Randezvous compiler will be installed in `build/llvm/install`.
4. Build Newlib and compiler-rt.
   ```shell
   ./build/build.newlib.sh && ./build/build.compiler.rt.sh
   ```
   After the two scripts finish, Newlib will be installed in
   `build/newlib-cygwin/install` and compiler-rt will be installed in
   `build/compiler-rt/install`.
5. Import all the IDE projects in the `workspace` directory into the IDE.
   ```shell
   ./scripts/import.sh
   ```

   For srtp:If you get wrong "MCUXpresso IDE cannot be found!" in this step,the file which needs to be modified is ./script/common.sh. You can follow `find_ide()` and find errors for the environment you've configured.
6. Build a `baseline` version of the HAL library.
   All our programs will be linked
   against the `baseline` HAL library.
   ```shell
   ./scripts/hal.sh baseline
   ```
7. Build a `baseline` version of the MbedTLS library.
   A benchmark and an application used in our evaluation will be linked against
   the `baseline` MbedTLS library.
   ```shell
   ./scripts/mbedtls.sh baseline
   ```

### Build, Debug, and Run Programs

We have six scripts (`beebs.sh`, `coremark-pro.sh`, `mbedtls-benchmark.sh`,
`pinlock.sh`, `sdcard_fatfs.sh`, and `shell.sh`)
that can compile, debug, and run three benchmark suites
([BEEBS](https://beebs.mageec.org/),
[CoreMark-Pro](https://www.eembc.org/coremark-pro), and
[MbedTLS-Benchmark](https://github.com/ARMmbed/mbedtls/blob/development/programs/test/benchmark.c))
and three real-world applications (PinLock, FatFs-SD, and LED-Shell),
respectively.
These scripts support identical command-line argument formats
```shell
./scripts/<script-name>.sh <CONFIG> [PROGRAM [PROGRAM]...]
```
or
```shell
./scripts/<script-name>.sh run <CONFIG> [PROGRAM [PROGRAM]...]
```
or
```shell
./scripts/<script-name>.sh debug <CONFIG> <PROGRAM>
```
where `CONFIG` is the name of a configuration (see below) and `PROGRAM` is the
name of a program in the corresponding benchmark suite.
For compile and run,
if `PROGRAM` is not specified, all the programs in the corresponding benchmark
suite will be compiled/run.And you can find the compilation file from `./workspace`.
For example, running `./scripts/beebs.sh baseline`
will compile all the benchmark programs in BEEBS using the `baseline`
configuration, and running `./scripts/coremark-pro.sh run randezvous zip-test`
will run the `zip-test` program in CoreMark-Pro that was compiled using the
`randezvous` configuration.

More specifically, we use two configurations of experiments for each benchmark
suite and application:
- **Baseline**: Compile the programs without any of our passes, denoted as
  `baseline`.
- **Randezvous**: Turn on all the Randezvous passes with all seeds set to zero,
  denoted as `randezvous`.

The following shell code compiles all benchmarks and applications we use, with
all possible
configurations:
```shell
for conf in baseline randezvous; do
    ./scripts/beebs.sh $conf
    ./scripts/coremark-pro.sh $conf
    ./scripts/mbedtls-benchmark.sh $conf
    ./scripts/pinlock.sh $conf
    ./scripts/sdcard_fatfs.sh $conf
    ./scripts/shell.sh $conf
done
```
Note that compilation using our scripts must be done one at a time (i.e., **no
parallel compiling of multiple programs**).
This is because the IDE runs a
singleton mode.

The following shell code runs all benchmarks and applications compiled by the
above shell code:
```shell
for conf in baseline randezvous; do
    ./scripts/beebs.sh run $conf
    ./scripts/coremark-pro.sh run $conf
    ./scripts/mbedtls-benchmark.sh run $conf
    ./scripts/pinlock.sh run $conf
    ./scripts/sdcard_fatfs.sh run $conf
    ./scripts/shell.sh run $conf
done
```
Note that in order to run programs, an NXP MIMXRT685-EVK board must be
connected to the host machine.
Also note that FatFs-SD requires an SD card inserted into the board's SD card
slot.

### Collect Experiment Results

After compiling a program, an ELF binary with a `.axf` suffix will be
placed in the `debug` directory, and after running a program, experiment data
with performance metrics will be generated in the `data` directory.
The names
of all the subdirectories and files under `debug` and `data` are
self-explanatory.
For example, `debug/beebs-baseline/baseline-whetstone.axf`
is the ELF binary of the `whetstone` program in BEEBS compiled using the
`baseline` configuration, and
`data/coremark-pro-randezvous/randezvous-core.stat` contains the
execution time of running the `core` program in CoreMark-Pro compiled with the
`randezvous` configuration.

You can use the `scripts/gen_csv.py` script to collect the raw experiment data
and write the summarized results to a CSV file.
This script takes three
optional command-line arguments:
```shell
-b benchmark_name # "beebs", "coremark-pro", "mbedtls-benchmark", "pinlock", "sdcard_fatfs", or "shell", default "beebs"
-t data_type      # "perf", "codesize", or "datasize", default "perf"
-o output_file    # Path of the output CSV file; if not specified, a default
                  # name "data_type-benchmark_name.csv" will be used
```
For example, if you want to see the performance numbers on BEEBS, run
```shell
./scripts/gen_csv.py -b beebs -t perf
```
and you will get an output file named `perf-beebs.csv` in the working directory.

### Demonstrative Exploits

In addition to performance evaluation, this repository also contains a
proof-of-concept (PoC) exploit and a real-world CVE exploit that we used to
demonstrate Randezvous's security.
The PoC exploit consists of a script `scripts/exploit.sh` representing an
attacker and a vulnerable application in `workspace/exploit`.
The CVE exploit consists of a script `scripts/cve.sh` representing an attacker
and a benign application in `workspace/cve` that is linked against a vulnerable
HAL library with
[CVE-2021-27421](https://www.cve.org/CVERecord?id=CVE-2021-27421)
(a `cve` version of the HAL library can be built by `./scripts/hal.sh cve`).
Both scripts take the same command-line argument formats as those used in
performance evaluation.

Unlike in performance evaluation, here we use three configurations for both the
PoC and CVE exploits:
- **Baseline**: Compile the application without any of our passes, denoted as
  `baseline`.
- **Randomization plus XOM**: Compile the application with only code/data
  layout randomization and XOM support, denoted as `randxom`.
  As the entropy of randomization on MCU systems heavily depends on memory size,
  we picked three exemplary MCUs of different memory sizes, deriving three
  sub-configurations denoted as `randxom-small`, `randxom-medium`, and
  `randxom-large`.
- **Randezvous**: Compile the application with all the Randezvous passes on and
  all seeds set to zero, denoted as `randezvous`.
  Likewise, we derived three sub-configurations denoted as `randezvous-small`,
  `randezvous-medium`, and `randezvous-large` for the three different-sized
  MCUs.

For each (sub-)configuration, the PoC script will generate attack payloads
using the best strategies for the attacker and keep sending payloads in a
brute-forcing manner, while the CVE script will do the same except that attack
payloads are generated using less efficient strategies.
In order to keep the exploit running, both scripts will reboot the application
each time before sending a new payload.
For either the PoC or CVE exploit,
you can compile the application and run the script to see how long the exploit
takes to succeed for each case.

## Contacts

Zhuojia Shen: zshen10@cs.rochester.edu

Komail Dharsee: kdharsee@cs.rochester.edu

John Criswell: criswell@cs.rochester.edu