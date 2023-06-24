import os
import sys
import argparse
import subprocess
import time


parser = argparse.ArgumentParser(
    prog=sys.argv[0],
    description="Test if compiler behaves well"
)


def build_llvmir(args):
    files: list[str] = sorted(args.filename)
    for f in files:
        # check if file exists
        if not os.path.exists(f):
            print(f, " not found")
            continue

        srcpath = os.path.dirname(f)
        # remove file extension
        basename: list[str] = os.path.basename(f).split('.')
        if len(basename) < 2 or basename[len(basename)-1] != 'sy':
            print(f, " not supported")
            continue
        basename.pop(len(basename)-1)
        basename = '.'.join(basename)

        # generate file names
        llname   = '/'.join([args.dir, '.'.join([basename,  'll'])])
        logname  = '/'.join([args.dir, '.'.join([basename, 'log'])])
        resname  = '/'.join([args.dir, '.'.join([basename, 'res'])])
        outname  = '/'.join([srcpath,  '.'.join([basename, 'out'])])
        inname   = '/'.join([srcpath,  '.'.join([basename,  'in'])])
        # print(llname, logname, resname, outname, inname)

        # compile
        cmd = [args.compiler, '-S', '-l', llname, f]
        resp = None
        with open(logname, 'w') as logfile:
            start = time.time()
            resp = subprocess.run(cmd, timeout=180, stdout=logfile)
            total_time = time.time() - start
            logfile.write("Compile use time: {}\n".format(str(total_time)))
            

        if resp.returncode == 124:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m".format(basename))
            continue
        elif resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m".format(basename))
            continue

        # llvm link
        cmd = ['llvm-link', 'sylib.ll', llname, '-S', '-o', llname]
        with open(logname, 'a') as logfile:
            resp = subprocess.run(cmd, stdout=logfile)

        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m".format(basename))
            continue

        # lli interpret
        infile = subprocess.DEVNULL
        cmd = ['lli', llname]
        if os.path.exists(inname):
            infile = open(inname, 'r')

        with open(logname, 'a') as logfile, open(resname, 'w') as resfile:
            resp = subprocess.run(cmd, timeout=300, stdin=infile, stdout=resfile, stderr=logfile)
        
        # test if program output sth
        recv = resp
        cmd = ['tail', '-c', '1', resname]

        # append return code to result file
        resp = subprocess.run(cmd, stdout=subprocess.PIPE)
        resp = resp.stdout.decode()
        with open(resname, 'a') as resfile:
            if len(resp.strip(' \n\t')):
                resfile.write('\n' + str(recv.returncode))
            else:
                resfile.write(str(recv.returncode))
        
        # diff test
        cmd = ['diff', '-Z', resname, outname]
        resp = subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mWrong Answer\033[0m".format(basename))
        else:
            print("\033[1;32mPASS:\033[0m {}".format(basename))


def build_asm(args):
    files: list[str] = sorted(args.filename)
    for f in files:
        # check if file exists
        if not os.path.exists(f):
            print(f, " not found")
            continue

        srcpath = os.path.dirname(f)
        # remove file extension
        basename: list[str] = os.path.basename(f).split('.')
        if len(basename) < 2 or basename[len(basename)-1] != 'sy':
            print(f, " not supported")
            continue
        basename.pop(len(basename)-1)
        basename = '.'.join(basename)

        # generate file names
        asmname  = '/'.join([args.dir, '.'.join([basename,   's'])])
        spkname  = '/'.join([args.dir, '.'.join([basename, 'spk'])])
        elfname  = '/'.join([args.dir, '.'.join([basename, 'elf'])])
        logname  = '/'.join([args.dir, '.'.join([basename, 'log'])])
        resname  = '/'.join([args.dir, '.'.join([basename, 'res'])])
        outname  = '/'.join([srcpath,  '.'.join([basename, 'out'])])
        inname   = '/'.join([srcpath,  '.'.join([basename,  'in'])])
        # print(llname, logname, resname, outname, inname)

        # compile
        cmd = [args.compiler, '-S', '-o', asmname, f]
        resp = None
        with open(logname, 'w') as logfile:
            resp = subprocess.run(cmd, timeout=180, stdout=logfile)

        if resp.returncode == 124:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m".format(basename))
            continue
        elif resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m".format(basename))
            continue

        # TODO: set march
        cmd = ['/home/blur/gits/riscv-gnu-toolchain/build/bin/riscv64-unknown-elf-gcc', args.sylib, asmname, '-o', elfname]

        with open(logname, 'a') as logfile:
            resp = subprocess.run(cmd, stdout=logfile)

        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m".format(basename))
            continue

        infile = subprocess.DEVNULL
        cmd = ['/home/blur/gits/riscv-gnu-toolchain/build/bin/spike', '/home/blur/gits/riscv-gnu-toolchain/build/riscv64-unknown-elf/bin/pk', elfname]
        if os.path.exists(inname):
            infile = open(inname, 'r')

        with open(logname, 'a') as logfile, open(spkname, 'w') as resfile:
            try:
                resp = subprocess.run(cmd, timeout=3, stdin=infile, stdout=resfile, stderr=logfile)
            except subprocess.TimeoutExpired as e:
                print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mTime Limit Exceed\033[0m".format(basename))
                continue
        
        with open(spkname, 'r') as spkfile, open(resname, 'w') as resfile:
            a = spkfile.readlines()
            b = []
            for s in a:
                if not s.startswith('bbl') and not s.startswith('TOTAL'):
                    b.append(s)
            c = ''.join(b)
            resfile.write(c)
        
        recv = resp
        cmd = ['tail', '-c', '1', resname]

        resp = subprocess.run(cmd, stdout=subprocess.PIPE)
        resp = resp.stdout.decode()
        with open(resname, 'a') as resfile:
            if len(resp.strip(' \n\t')):
                resfile.write('\n' + str(recv.returncode))
            else:
                resfile.write(str(recv.returncode))
        
        cmd = ['diff', '-Z', resname, outname]
        resp = subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mWrong Answer\033[0m".format(basename))
        else:
            print("\033[1;32mPASS:\033[0m {}".format(basename))


def test_perf(args):
    pass


if __name__ == '__main__':
    parser.add_argument('filename', nargs='+')
    parser.add_argument('-l', '--llvmir',   action='store_true')
    parser.add_argument('-a', '--asm',      action='store_true')
    parser.add_argument('-A', '--all',      action='store_true')
    parser.add_argument('-s', '--sylib',    action='store')
    parser.add_argument('-d', '--dir',      action='store' , required=True)
    parser.add_argument('-c', '--compiler', action='store' , required=True)

    args = parser.parse_args()

    print("Testing: {}".format(args.dir))

    if not os.path.exists(args.dir):
        os.makedirs(args.dir)

    if args.llvmir:
        build_llvmir(args)
    
    if args.asm:
        build_asm(args)
        test_perf(args)

    # using perf ?
    # if args.all:
    #     test_perf(args)


    

"""
2 2
3 1
4 2
4 4
1 2
2 3
2 1
3 2
3 4
3 1
"""