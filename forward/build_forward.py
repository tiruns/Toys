import os
import sys

CompilerPath = "cl.exe"


if __name__=='__main__':
    if len(sys.argv) < 2:
        raise RuntimeError("Program path not set.")
    programPath = sys.argv[1]
    if not os.path.exists(sys.argv[1]):
        raise RuntimeError("Program {} do not exist.".format(programPath))
    build_command = CompilerPath + " " + '.\\forward\\main.cpp' + " "
    build_command += '/D "PROGRAM_PATH=LR\\\"({})\\\""'.format(programPath) + " "
    build_command += '/D "UNICODE"' + " "
    build_command += '/D "USE_NAMED_MUTEX"' + " "
    build_command += '/Fe: ' + os.path.basename(programPath)
    print(build_command)
    os.system(build_command)
    if os.path.exists("main.obj"):
        os.remove("main.obj")
    