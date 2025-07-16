
functional_dep_list = [
]

functional_inc = include_directories('.')
functional_lib = library('functional', 'functional.c',
  dependencies: functional_dep_list,
)
functional_dep = declare_dependency(
  link_with: functional_lib, 
  include_directories: functional_inc,
  dependencies: functional_dep_list,
)

# end of file
