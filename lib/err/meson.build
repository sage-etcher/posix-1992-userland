
err_dep_list = [
]

err_inc = include_directories('.')
err_lib = library('err', 'err.c',
  dependencies: err_dep_list,
)
err_dep = declare_dependency(
  link_with: err_lib, 
  include_directories: err_inc,
  dependencies: err_dep_list,
)

# end of file
