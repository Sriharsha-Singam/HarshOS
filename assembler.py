import argparse
import re
from string import Template

def extract_defines(file_name):
    file = open(file_name, 'r')
    file_contents = file.read()
    define_indices = [i.start() for i in re.finditer("&define", file_contents)]

    function_requirements = {}
    for index in define_indices:
        print(str(index))
        print(file_contents[index:index+30])
        function_name_index = index + len("&define ")
        print(str(function_name_index))
        start_of_args = file_contents[function_name_index:].find('(') + function_name_index
        end_of_args = file_contents[start_of_args:].find(')') + start_of_args
        function_name = file_contents[function_name_index:start_of_args]
        print(str(function_name_index) + ", " + str(start_of_args))
        function_contents = file_contents[file_contents[start_of_args:].find('\n')+start_of_args+1:file_contents[start_of_args:].find('&end')+start_of_args]
        func_args = [final.strip() for final in file_contents[start_of_args+1:end_of_args].split(',')]
        print(str(func_args))
        function_requirements[function_name] = {
            "func_args": func_args,
            "func_contents": function_contents
        }
        file_contents = file_contents[:index] + file_contents[file_contents[start_of_args:].find('&end') + start_of_args + len('&end\n'):]

    file.close()

    # file = open(file_name[:-2] + '.s', 'w')
    # file.write(file_contents)
    # file.close()

    return function_requirements, file_contents

def convert_file(file_name):
    functions, file_contents = extract_defines(file_name)

    print(str(functions))
    # print(file_contents)
    # return
    # file = open(file_name[:-2] + '.s', 'w+')
    # file_contents = file.read()

    for key, value in functions.items():
        func_indices = [i.start() for i in re.finditer(key, file_contents)]
        for index in func_indices:
            start_of_args = file_contents[index:].find('(') + index
            end_of_args = file_contents[start_of_args:].find(')') + start_of_args
            func_args = [final.strip() for final in file_contents[start_of_args+1:end_of_args].split(',')]
            function_content = value["func_contents"]

            print(function_content)

            func_arg_index = 0
            function_template = Template(function_content)
            custom = {}
            for func_arg in value["func_args"]:
                # print(func_arg)
                # func_arg_locs = [i.start() for i in re.finditer("$" + func_arg, function_content)]

                custom[func_arg] = func_args[func_arg_index]
                func_arg_index = func_arg_index + 1
            # file_contents = function_template.substitute(custom)
                # for func_arg_log in func_arg_locs:
                #     function_content = function_content[:func_arg_log-1] + func_args[func_arg_index] + function_content[func_arg_log+len(func_arg):]
                # func_arg_index = func_arg_index + 1

            file_contents = file_contents[:index] + function_template.substitute(custom) + file_contents[file_contents[index:].find("\n")+1+index:]

    print(file_contents)
    # file.close()


def main():
    parser = argparse.ArgumentParser(description='Pass assembly file with custom macros.')
    parser.add_argument('--files', nargs='+', help='All files to be converted')
    args = parser.parse_args()

    if args.files:
        for file in args.files:
            if file[len(file)-2:len(file)] == '.S':
                convert_file(file)




if __name__ == "__main__":
    main()
