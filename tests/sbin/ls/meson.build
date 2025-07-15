
test_dirs = [
  'add_child',
  'dir_content',
  'filter',
  'lu_len',
  'map',
  'snprintlu',
  'snprintlu_dup',
  'strdup',
]

foreach test_dir : test_dirs
  test_bin = executable(test_dir, test_dir / 'main.c',
    dependencies: [
      ls_dep,
      tap_dep,
    ],
  )
  test(test_dir, test_bin)
endforeach

# end of file 
