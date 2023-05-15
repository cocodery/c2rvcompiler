import os
import sys
import argparse
import subprocess

test_flags: dict[bool] = { 'all': False, 'asm': False }

parser = argparse.ArgumentParser(
    prog=sys.argv[0],
    description="Test if compiler behaves well"
)

def build_llvmir(args):
    files: list[str] = args.filename
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
            resp = subprocess.run(cmd, timeout=180, stdout=logfile)

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
        if os.path.exists(inname):
            cmd = ['lli', llname]
            infile = open(inname, 'r')
        else:
            cmd = ['lli', llname]
        with open(logname, 'a') as logfile, open(resname, 'w') as resfile:
            resp = subprocess.run(cmd, timeout=300, stdin=infile, stdout=resfile, stderr=logfile)
        
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


def build_asm(args):
    pass


def build_bin(args):
    pass


def test_perf(args):
    pass


if __name__ == '__main__':
    parser.add_argument('filename', nargs='+')
    parser.add_argument('-l', '--llvmir',   action='store_true')
    parser.add_argument('-a', '--asm',      action='store_true')
    parser.add_argument('-A', '--all',      action='store_true')
    parser.add_argument('-d', '--dir',      action='store' , required=True)
    parser.add_argument('-c', '--compiler', action='store' , required=True)

    args = parser.parse_args()
    args.filename = sorted(args.filename)
    # print(args.filename, args.dir, args.compiler)

    if not os.path.exists(args.dir):
        os.makedirs(args.dir)

    if args.llvmir:
        build_llvmir(args)
    
    # if args.asm or args.all:
    #     build_asm(args)

    # if args.all and test_flags['asm']:
    #     build_bin(args)

    # test_perf(args)

    

