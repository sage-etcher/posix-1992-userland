
# lists
ls_srcs = [
  'ls.c',
]

ls_dep_list = [
  convert_dep,
  functional_dep,
  m_dep,
  vstring_dep,
]

# localization
subdir('po')

# manpages
install_man('ls.1')

# library generation
ls_inc = include_directories('.')
ls_lib = library('ls', ls_srcs, 
  dependencies: ls_dep_list,
  install: true,
)
ls_dep = declare_dependency(
  link_with: ls_lib, 
  include_directories: ls_inc,
  dependencies: ls_dep_list,
)

# executable generation
ls_bin = executable('ls', 'main.c', 
  dependencies: [
    ls_dep,
  ],
  install: true,
)

# end of file
