
chmod_dep_list = [
  basename_dep,
  convert_dep,
  err_dep,
  iterdir_dep,
  vstring_dep,
]

chmod_inc = include_directories('.')
chmod_lib = library('chmod', 'chmod.c', 
  dependencies: chmod_dep_list,
  install: true,
)
chmod_dep = declare_dependency(
  link_with: chmod_lib, 
  include_directories: chmod_inc,
  dependencies: chmod_dep_list,
)

chmod_bin = executable('chmod', 'main.c', 
  dependencies: [
    chmod_dep,
  ],
  install: true,
)

# end of file
