import argparse
import os
import subprocess


if __name__ == '__main__':
    parser = argparse.ArgumentParser('cacaodev', "Develop cacao")

    parser.add_argument('action', type=str, help='The action to perform, one out of {compile}')
    parser.add_argument('path', type=str, help='A path to the directory that is to be compiled')

    args = parser.parse_args()

    if args.path is None:
        args_path = os.getcwd()
    else:
        args_path = str(args.path)

    print('CWD="{0}"'.format(os.getcwd()))

    if args.action is None:
        print("Please specify an action.")
        exit(1)
    elif args.action == "compile":
        subprocess.run(['python', './compile/coffeec.py', args_path])
    else:
        print("Unknown action")
        exit(1)
    

    exit(0)
