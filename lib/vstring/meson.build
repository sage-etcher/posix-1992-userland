
vstring_dep_list = [
]

vstring_inc = include_directories('.')
vstring_lib = library('vstring', 'vstring.c',
  dependencies: vstring_dep_list,
)
vstring_dep = declare_dependency(
  link_with: vstring_lib, 
  include_directories: vstring_inc,
  dependencies: vstring_dep_list,
)

# end of file
