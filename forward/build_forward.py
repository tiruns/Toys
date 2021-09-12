import os
import sys
import subprocess

CompilerPath = r'cl.exe'
Definitions = [
    r'UNICODE',
    r'_UNICODE',
    r'O2',
    r'NDEBUG',
    r'USE_NAMED_MUTEX'
]
IncludesPaths = []
LibPaths = []

def AddIncludePath(path):
    return r'/I "{}" '.format(path)

def AddLibPath(path):
    return r'/LIBPATH:"{}" '.format(path)

def AddDefinition(d):
    return r'/D "{}" '.format(d)

def BuildCommand(programPath):
    # Input
    command = r'"{}" .\forward\main.cpp /MD '.format(CompilerPath)
    # Definitions
    for d in Definitions:
        command += AddDefinition(d)
    command += AddDefinition(r'PROGRAM_PATH=LR\"({})\"'.format(programPath))
    # Include Paths
    for p in IncludesPaths:
        command += AddIncludePath(p)
    # Output
    command += r'/Fe: {} '.format(os.path.basename(programPath))
    # Lib Paths
    command += r'/link /OPT:REF /OPT:ICF '
    for p in LibPaths:
        command += AddLibPath(p)

    return command


if __name__=='__main__':
    if len(sys.argv) < 2:
        raise RuntimeError("Program path not set.")
    programPath = sys.argv[1]
    if not os.path.exists(sys.argv[1]):
        raise RuntimeError("Program {} do not exist.".format(programPath))

    command = BuildCommand(programPath)

    # print(command)
    subprocess.run(command)
    if os.path.exists("main.obj"):
        os.remove("main.obj")
    