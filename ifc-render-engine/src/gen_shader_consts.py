import os

shader_dir = '../../workdir/shaders/'
shader_output_dir = 'ifcre/render/gl/'
shader_output_file = shader_output_dir + 'shader_consts.h'
file_list = os.listdir(shader_dir)
ff = open(shader_output_file, 'w')
flag = 1

file_content_header = "#pragma once\n" 
file_content_header += "#ifndef IFCRE_SHADER_CONSTS_H_\n"
file_content_header += "#define IFCRE_SHADER_CONSTS_H_\n\n"
file_content_header += "namespace ifcre {\n"
file_content_header += "\tnamespace sc {\n"

file_content_tail = "\t}// shader_consts\n\n"
file_content_tail += "}\n"
file_content_tail += "#endif\n"
ff.write(file_content_header)
for file in file_list:
    flag = 1 - flag
    f = open(shader_dir + file)
    lines = f.readlines()
    # if file.endswith('.vert'):
    #     ff = open(shader_output_dir + 'v_' + file[0:-5], 'w')
    # else:
    #     ff = open(shader_output_dir + 'f_' + file[0:-5], 'w')
    ff.write('\t\tconst char* ')
    if file.endswith('.vert'):
        ff.write('v_' + file[0:-5])
    else:
        ff.write('f_' + file[0:-5])
    ff.write(' = ')
    index = 0
    for line in lines:
        line = line.strip()
        if line != "":
            if index != 0:
                ff.write('\t\t\t')
            ff.write(('\"' + line + '\\r\\n\"'))
            if index != len(lines) - 1:
                ff.write('\n')
        index = index + 1
    ff.write(";\n")
    if flag == 1:
        ff.write('\n')
ff.write(file_content_tail)
print('ok')