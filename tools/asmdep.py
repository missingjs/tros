import os.path
from pathlib import Path
import re
import sys
from typing import List, Optional


def main():
    include_path_list = []
    input_file = None
    target_name = None
    i = 1
    while i < len(sys.argv):
        if sys.argv[i] == '-I':
            include_path_list.append(sys.argv[i+1])
            i += 1
        elif sys.argv[i] == '-MT':
            target_name = sys.argv[i+1]
            i += 1
        else:
            input_file = sys.argv[i]
        i += 1

    if not all([input_file, target_name]):
        show_usage()
        sys.exit(1)

    dep_files = find_deps(input_file, include_path_list)

    content = f"{target_name} : {input_file} {' '.join(dep_files)}"
    print(content)


def find_deps(input_file: str, include_path_list: List[str]) -> List[str]:
    dep_files = []
    collect_deps(input_file, include_path_list, dep_files)
    return dep_files


def collect_deps(input_file: str, include_path_list: List[str], dep_files: List[str]) -> None:
    with open(input_file, 'rt') as fp:
        include_pattern = re.compile(r'^\s*%include\s+"(.*)"')
        for line in fp:
            mat = include_pattern.match(line)
            if not mat:
                continue
            inc_file = mat.group(1)
            real_inc_file = search_file(inc_file, include_path_list)
            if not real_inc_file:
                parent = str(Path(input_file).parent)
                real_inc_file = search_file(inc_file, [parent])
                if not real_inc_file:
                    print(f"{inc_file} included in {input_file}: File not found", file=sys.stderr)
                    sys.exit(2)
            dep_files.append(real_inc_file)
            collect_deps(real_inc_file, include_path_list, dep_files)


def search_file(relative_file_path: str, include_path_list: List[str]) -> Optional[str]:
    for inc_path in include_path_list:
        norm_path = os.path.normpath(f"{inc_path}/{relative_file_path}")
        if Path(norm_path).is_file():
            return norm_path

    
def show_usage():
    print('usage: python3 asmdep.py <-I PATH> [-I PATH]... <-MT TARGET> <input-file>')
    print('')
    print('options:')
    print('    -I  PATH      set include path, you can use it multiple times')
    print('    -MT TARGET    set target name in output line')


if __name__ == '__main__':
    main()

