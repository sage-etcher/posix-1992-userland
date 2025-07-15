
basename_dep_list = [
]

basename_inc = include_directories('.')
basename_lib = library('basename', 'basename.c', 
  dependencies: basename_dep_list,
  install: true,
)
basename_dep = declare_dependency(
  link_with: basename_lib, 
  include_directories: basename_inc,
  dependencies: basename_dep_list,
)

basename_bin = executable('basename', 'main.c', 
  dependencies: [
    basename_dep,
  ],
  install: true,
)

# end of file
