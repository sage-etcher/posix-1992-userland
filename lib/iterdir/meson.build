
iterdir_dep_list = [
  err_dep,
]

iterdir_inc = include_directories('.')
iterdir_lib = library('iterdir', 'iterdir.c',
  dependencies: iterdir_dep_list,
)
iterdir_dep = declare_dependency(
  link_with: iterdir_lib, 
  include_directories: iterdir_inc,
  dependencies: iterdir_dep_list,
)

# end of file
