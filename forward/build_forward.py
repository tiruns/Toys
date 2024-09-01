import os
import sys
import subprocess

CompilerPath = r'cl.exe'
Definitions = [
    r'UNICODE',
    r'_UNICODE',
    r'O2',
    r'NDEBUG',
    # r'USE_NAMED_MUTEX'
]

def AddDefinition(d):
    return r'/D "{}" '.format(d)

def BuildCommand(programPath):
    # Input
    command = r'"{}" .\main.cpp /MD '.format(CompilerPath)
    # Definitions
    for d in Definitions:
        command += AddDefinition(d)
    command += AddDefinition(r'PROGRAM_PATH=LR\"({})\"'.format(programPath))
    # Output
    command += r'/Fe: {} '.format(os.path.basename(programPath))
    # Lib Paths
    command += r'/link /OPT:REF /OPT:ICF '

    return command


if __name__=='__main__':
    if len(sys.argv) < 2:
        raise RuntimeError("Program path not set.")
    programPath = sys.argv[1]
    if not os.path.exists(sys.argv[1]):
        raise RuntimeError("Program {} do not exist.".format(programPath))
    if not os.path.isfile(sys.argv[1]):
        raise RuntimeError("{} is not a program file.".format(programPath))

    command = BuildCommand(programPath)

    # print(command)
    subprocess.run(command)
    if os.path.exists("main.obj"):
        os.remove("main.obj")
    