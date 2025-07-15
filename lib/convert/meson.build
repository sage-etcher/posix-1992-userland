
convert_dep_list = [
]

convert_inc = include_directories('.')
convert_lib = library('convert', 'convert.c',
  dependencies: convert_dep_list,
)
convert_dep = declare_dependency(
  link_with: convert_lib, 
  include_directories: convert_inc,
  dependencies: convert_dep_list,
)

# end of file
