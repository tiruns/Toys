import os
import sys
import subprocess

CompilerPath = r"cl.exe"
Definitions = [r"UNICODE", r"_UNICODE", r"NDEBUG", r"_CONSOLE"]
CompilerFlags = [
    r"/W3 /sdl /nologo",
    r"/O2 /Oi /GL",
    r"/EHsc /MD /GS /Gy",
    r"/std:c++latest /permissive-",
]


def AddDefinition(d):
    return r"/D {} ".format(d)


def BuildCommand(programPath):
    command = r"{} ./forward.cpp ".format(CompilerPath)
    for d in Definitions:
        command += AddDefinition(d)
    command += AddDefinition(r'"PROGRAM_PATH=LR\"?({})?\""'.format(programPath))
    for f in CompilerFlags:
        command += r"{} ".format(f)
    command += r'/Fe: "{}" '.format(os.path.basename(programPath))
    command += r"/link"
    return command


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise RuntimeError("Program path not set.")
    programPath = sys.argv[1]
    if not os.path.exists(sys.argv[1]):
        raise RuntimeError("Program {} do not exist.".format(programPath))
    if not os.path.isfile(sys.argv[1]):
        raise RuntimeError("{} is not a program file.".format(programPath))

    command = BuildCommand(programPath)

    print(command)
    # exit()
    subprocess.run(command)
    if os.path.exists("forward.obj"):
        os.remove("forward.obj")  
