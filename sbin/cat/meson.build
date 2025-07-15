
cat_dep_list = [
]

cat_inc = include_directories('.')
cat_lib = library('cat', 'cat.c', 
  dependencies: cat_dep_list,
  install: true,
)
cat_dep = declare_dependency(
  link_with: cat_lib, 
  include_directories: cat_inc,
  dependencies: cat_dep_list,
)

cat_bin = executable('cat', 'main.c', 
  dependencies: [
    cat_dep,
  ],
  install: true,
)

# end of file
