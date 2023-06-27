import os
import sys
import argparse
import subprocess
import time

parser = argparse.ArgumentParser(
    prog=sys.argv[0],
    description="Test if compiler behaves well"
)

def build_run(args, exts, cc_arg, simcc_arg):
    for f in args.filename:
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
        name     = '/'.join([args.dir, '.'.join([basename, exts['name']])])
        runname  = '/'.join([args.dir, '.'.join([basename, exts['run']])])

        logname  = '/'.join([args.dir, '.'.join([basename, 'log'])])
        resname  = '/'.join([args.dir, '.'.join([basename, 'res'])])
        outname  = '/'.join([srcpath,  '.'.join([basename, 'out'])])
        inname   = '/'.join([srcpath,  '.'.join([basename,  'in'])])

        cmd = [args.compiler, '-S', simcc_arg, name, f]
        resp = None

        try:
            with open(logname, 'w') as logfile:
                start = time.time()
                resp = subprocess.run(cmd, timeout=180, stdout=logfile)
                total_time = time.time() - start
                logfile.write("Compile use time: {}\n".format(str(total_time)))
        except subprocess.TimeoutExpired:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m".format(basename))
            continue

        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m".format(basename))
            continue

        cmd = [args.cc, args.sylib, name, '-o', runname]
        for a in cc_arg:
            cmd.append(a)

        with open(logname, 'a') as logfile:
            resp = subprocess.run(cmd, stdout=logfile)

        if resp.returncode != 0:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m".format(basename))
            continue

        infile = subprocess.DEVNULL
        cmd: list[str] = args.sim.split(' ')
        cmd.append(runname)
        
        if os.path.exists(inname):
            infile = open(inname, 'r')

        try:
            with open(logname, 'a') as logfile, open(resname, 'w') as resfile, open("/".join([args.dir, "perf.log"]), "a") as perfile:
                start = time.time()
                resp = subprocess.run(cmd, timeout=300, stdin=infile, stdout=resfile, stderr=logfile)
                total_time = time.time() - start
                perfile.write("{},{}\n".format(basename, str(total_time)))
        except subprocess.TimeoutExpired:
            print("\033[1;31mFAIL:\033[0m {}\t\033[1;31mTime Limit Exceed\033[0m".format(basename))
            continue
        
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


if __name__ == '__main__':
    parser.add_argument('filename', nargs='+')
    parser.add_argument('-l', '--llvmir',   action='store_true')
    parser.add_argument('-a', '--asm',      action='store_true')

    parser.add_argument('-d', '--dir',      action='store' , required=True)
    parser.add_argument('-c', '--compiler', action='store' , required=True)
    parser.add_argument('-s', '--sylib',    action='store')
    parser.add_argument('-x', '--cc',       action='store')
    parser.add_argument('-m', '--sim',      action='store')

    args = parser.parse_args()

    with open("/".join([args.dir, "perf.log"]), "w") as perfile:
        perfile.write("{}:\n".format(args.dir))

    print("Testing: {}".format(args.dir))

    if not os.path.exists(args.dir):
        os.makedirs(args.dir)

    if args.llvmir:
        exts = {
            'name': 'll',
            'run': 'o.ll'
        }

        cc_arg = ['-S']

        build_run(args, exts, cc_arg, '-l')
    
    if args.asm:
        exts = {
            'name': 's',
            'run': 'elf'
        }

        cc_arg = ['-static']

        build_run(args, exts, cc_arg, '-o')