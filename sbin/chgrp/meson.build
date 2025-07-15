
chgrp_dep_list = [
]

chgrp_inc = include_directories('.')
chgrp_lib = library('chgrp', 'chgrp.c', 
  dependencies: chgrp_dep_list,
  install: true,
)
chgrp_dep = declare_dependency(
  link_with: chgrp_lib, 
  include_directories: chgrp_inc,
  dependencies: chgrp_dep_list,
)

chgrp_bin = executable('chgrp', 'main.c', 
  dependencies: [
    chgrp_dep,
  ],
  install: true,
)

# end of file
