import subprocess
import os
import glob

TEST_BINARIES_DIR = './test_cases'
EXECUTABLES_DIR = './solution'
DISASM_PATH = os.path.join(EXECUTABLES_DIR, 'disassemble')
ASM_PATH = os.path.join(EXECUTABLES_DIR, 'assemble')


def disassemble(test_bin_path, test_bin_out_path):
    args = [DISASM_PATH, test_bin_path, test_bin_out_path]
    print(f"Executing {' '.join(args)}")
    result = subprocess.call(args)
    if result != 0:
        print('Failed to disassemble ' + test_bin_path)
        exit(1)
 

def assemble(s_file_path, bin_out_path):
    args = [ASM_PATH, s_file_path, bin_out_path]
    print(f"Executing {' '.join(args)}")
    result = subprocess.call(args)
    if result != 0:
        print('Failed to assemble ' + s_file_path)
        exit(1)
   

def main():
    test_bins = glob.glob(TEST_BINARIES_DIR + '/**/*.bin', recursive=True)
    print(test_bins)

    for bin_path in test_bins:
        s_file_path = bin_path + '_out.s'
        out_bin_path = s_file_path + '_out.bin'
        disassemble(bin_path, s_file_path)
        assemble(s_file_path, out_bin_path)
        with open(bin_path, 'rb') as fp:
            original_bin = fp.read()
        with open(out_bin_path, 'rb') as fp:
            generated_bin = fp.read()
        if original_bin != generated_bin:
            print("Generated binary is not the same as original binary")      
            exit(1)

    print(f"ALL {len(test_bins)} TESTS PASSED!")

if __name__ == "__main__":
    main()
