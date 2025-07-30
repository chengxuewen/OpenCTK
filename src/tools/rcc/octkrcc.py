import os
import re
import sys
import hashlib
import argparse
import textwrap

import xml.etree.ElementTree as XML

rcc_version = '0.1.1'
rcc_name = 'OpenCTK Resource Compiler'

def is_file_in_directory(file_path, directory_path):
    # Obtain the absolute path of the file path and directory path
    absolute_file_path = os.path.abspath(file_path)
    absolute_directory_path = os.path.abspath(directory_path)
    # Determine whether the file path starts with a directory path
    return absolute_file_path.startswith(absolute_directory_path)

chars_to_remove = ['\t', '\b', '\n', '\r', '\v', '\f']
def remove_chars(str, chars):
    return str.translate({ord(char): None for char in chars})

def replace_chars(str, chars, c):
    pattern = '[' + ''.join(re.escape(char) for char in chars) + ']'
    return re.sub(pattern, c, str)

def hex_array_from_number16bit(number):
    # Check if the number is within the range of 16 unsigned integers
    if not 0 <= number <= 0xFFFF:
        raise ValueError("Number must be between 0 and 65535")
    # Convert to hexadecimal and fill to 4 characters
    hex_str = format(number, '04X')
    # Split into two hexadecimal strings
    hex_array = [hex_str[:2], hex_str[2:]]
    return hex_array

def hex_array_from_number32bit(number):
    # Convert 32-bit integers to 8-bit hexadecimal strings, as 32-bit integers can be represented as up to
    # 8 hexadecimal characters
    hex_str = format(number, '08x')
    # Splitting hexadecimal strings into arrays of every two characters
    hex_array = [hex_str[i:i+2] for i in range(0, len(hex_str), 2)]
    return hex_array

def c_hex_data_from_hex_array(array):
    c_data = ''
    for hex_str in array:
        c_data += f'0x{hex_str},'
    return c_data

def c_hex_data_from_string(string):
    hex_array = [format(ord(c), '02x') for c in string]
    return c_hex_data_from_hex_array(hex_array)

class CustomHelpFormatter(argparse.HelpFormatter):
    def __init__(self, *args, **kwargs):
        # Can adjust parameters such as width and indentation
        super().__init__(*args, max_help_position=50, width=100, **kwargs)

    def _format_args(self, action, default_metavar):
        return '\b'
        # return None
        if action.nargs == '+':
            return '%s' % default_metavar
        else:
            return super()._format_args(action, default_metavar)

    def _split_lines(self, text, width):
        #This is a rewritten method used to handle the segmentation of text lines.
        #You can add custom formatting logic here.
        #The following code example will retain the spaces and line breaks added by the user.
        # print('text:', text)
        new_text = remove_chars(text, chars_to_remove).capitalize()
        format_text = '\t' + textwrap.fill(new_text, 80)
        format_text = format_text.replace('\n', '\n\t')
        return format_text.splitlines()

def main():
    print(f'{rcc_name} {rcc_version}')
    parser = argparse.ArgumentParser(prog='octkrcc',
                                     description=rcc_name + ' version ' + rcc_version,
                                     formatter_class=CustomHelpFormatter)
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {rcc_version}')
    parser.add_argument('-n', '--name', action='store', metavar='\b',
                        help='Name of resource.')
    parser.add_argument('-o', '--output', action='store', metavar='\b',
                        help='Path to output file. If the file exists it will be overwritten. The default value of "output" is the '
                             'execution directory of the octk-rcc.')
    parser.add_argument('-V', '--verbose', action='store', metavar='s',
                        help='Run in verbose mode')
    parser.add_argument('-c', '--compress', metavar='', action='store', type=int,
                        help='Compress input files by <level>.')
    parser.add_argument('-p', '--prefix', action='store', type=str,
                        help='Value "prefix" tells octk-rcc to prepend a directory-style path to the resource filepath in the resulting '
                             'binary')
    parser.add_argument('-w', '--whence', action='store', type=str,
                        help='Value "whence" tells octk-rcc how to rewrite the filepaths to the resource files. The default value of '
                             '"whence" is the execution directory of the octk-rcc.')
    parser.add_argument('-r', '--resources', metavar='', nargs='+',
                        help='Paths to resource files, relative to the current working directory')

    parser.add_argument('--no-compress', action='store_true',
                        help='Disable all compression')

    args = parser.parse_args()

    # set work_directory
    work_directory = os.getcwd()
    # set resource_name
    resource_name = 'resource'
    if args.name:
        resource_name = args.name
    # set output_dir
    output_dir = work_directory
    if args.output:
        output_dir = args.output
    # set output_file
    output_file = f'{output_dir}/octk_rc_{resource_name}.cpp'

    if args.compress:
        print('compress:', args.compress)

    if args.resources:
        print('resources:', args.resources)

    file_paths = args.resources
    resource_content = ''
    resource_data_content = ''
    resource_name_content = '' # name_length[2], name_hash[16], name
    resource_tree_content = '' # flags[2], name_offset[4], data_offset[4]
    resource_data_offset = 0
    resource_name_offset = 0
    for index, file_path in enumerate(file_paths):
        if not os.path.exists(file_path):
            sys.exit(f'Error:file {file_path} not exist')
        if not is_file_in_directory(file_path, work_directory):
            sys.exit(f'Error:file \'{file_path}\' not in directory \'{work_directory}\'')
        file_name = os.path.basename(file_path)
        file_name_len = len(file_name)
        file_name_md5 = hashlib.md5(file_name.encode('utf-8')).hexdigest()
        # print("file_name_md5=", file_name_md5)
        absolute_path = os.path.abspath(file_path)
        relative_path = os.path.relpath(file_path, work_directory)
        relative_path_len = len(relative_path)
        relative_path_md5 = hashlib.md5(relative_path.encode('utf-8')).hexdigest()
        with open(file_path, 'rb') as file:
            content = file.read()
            content_len = len(content)
            # write data to resource_data_content
            if content_len > 0:
                bytes_num = 0
                resource_data_content += f'// {absolute_path}\n'
                for i, byte in enumerate(content):
                    resource_data_content += f'0x{byte:02x},'
                    if 0 == (i + 1) % 24 and (i + 1)  != content_len:
                        resource_data_content += '\n'
                    bytes_num += 1
                resource_data_content += '\n'
            # write data to resource_name_content
            resource_name_content += f'// {file_name}\n'
            file_name_len_hex_array = hex_array_from_number16bit(file_name_len)
            file_name_len_c_hex_data = c_hex_data_from_hex_array(file_name_len_hex_array)
            resource_name_content += f'{file_name_len_c_hex_data}\n'
            file_name_md5_hex_array = [file_name_md5[i:i+2] for i in range(0, len(file_name_md5), 2)]
            file_name_md5_c_hex_data = c_hex_data_from_hex_array(file_name_md5_hex_array)
            resource_name_content += f'{file_name_md5_c_hex_data}\n'
            file_name_c_hex_data = c_hex_data_from_string(file_name)
            resource_name_content += f'{file_name_c_hex_data}\n'
            # update name offset
            resource_name_offset += len(file_name_len_hex_array) + len(file_name_md5_hex_array) + len(file_name)

            # write data to resource_tree_content
            resource_tree_content += f'// :/{relative_path}\n'
            flags_hex_array = hex_array_from_number16bit(0)
            flags_c_hex_data = c_hex_data_from_hex_array(flags_hex_array)
            resource_tree_content += f'{flags_c_hex_data}    '
            name_offset_array = hex_array_from_number32bit(resource_name_offset)
            name_offset_c_hex_data = c_hex_data_from_hex_array(name_offset_array)
            resource_tree_content += f'{name_offset_c_hex_data}    '
            data_offset_array = hex_array_from_number32bit(resource_data_offset)
            data_offset_c_hex_data = c_hex_data_from_hex_array(data_offset_array)
            resource_tree_content += f'{data_offset_c_hex_data}\n'
            # update data offset
            resource_data_offset += content_len


    resource_content += f'\nstatic const unsigned char resource_data_content[] = {{\n {resource_data_content} }};\n\n'
    resource_content += '// name_length[2], name_hash[16], name'
    resource_content += f'\nstatic const unsigned char resource_name_content[] = {{\n {resource_name_content} }};\n\n'
    resource_content += '// flags[2], name_offset[4], data_offset[4]'
    resource_content += f'\nstatic const unsigned char resource_tree_content[] = {{\n {resource_tree_content} }};\n\n'

    resource_content += '\ntypedef int (*resources_callback_func)(int, const unsigned char *, const unsigned char *, const unsigned char *);\n'
    resource_content += f'\nint octk_init_resources_{resource_name}(resources_callback_func func) {{'
    resource_content += '\n    return func(0x02, resource_tree_content, resource_name_content, resource_data_content);'
    resource_content += '\n}'
    resource_content += f'\nint octk_cleanup_resources_{resource_name}(resources_callback_func func) {{'
    resource_content += '\n    return func(0x02, resource_tree_content, resource_name_content, resource_data_content);'
    resource_content += '\n}\n'
    resource_content += '\nint octk_resource_register_data(int, const unsigned char *, const unsigned char *, const unsigned char *);\n'
    resource_content += '\nint octk_resource_cleanup_data(int, const unsigned char *, const unsigned char *, const unsigned char *);\n'
    resource_content += f'\nint octk_resource_register_data_{resource_name}() {{'
    resource_content += '\n    return octk_resource_register_data(0x02, resource_tree_content, resource_name_content, resource_data_content);'
    resource_content += '\n}'
    resource_content += f'\nint octk_resource_cleanup_data_{resource_name}() {{'
    resource_content += '\n    return octk_resource_cleanup_data(0x02, resource_tree_content, resource_name_content, resource_data_content);'
    resource_content += '\n}\n'

    #
    # write rc_content to rc source file
    with open(output_file, 'w') as rc_file:
        rc_file.write(resource_content)

# main
if __name__ == '__main__':
    main()
