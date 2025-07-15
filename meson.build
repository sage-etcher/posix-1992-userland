
project('posix-1992-userland', 'c',
  version: '0.1.0',
  default_options: [
    'c_std=c89',
    'warning_level=3',
  ],
  meson_version: '>=1.1.0',
)

# external dependencies
pkgconf = import('pkgconfig')
i18n = import('i18n')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

# configuration file
conf_data = configuration_data()
conf_data.set_quoted('PROJECT_NAME',    meson.project_name())
conf_data.set_quoted('PROJECT_VERSION', meson.project_version()) 
conf_data.set('_POSIX_C_SOURCE', 1) # POSIX.1-1988

conf_file = configure_file(
  configuration: conf_data,
  input:  'config.h.in',
  output: 'config.h',
)

# build logic
subdir('lib')
subdir('sbin')

# unit tests
if get_option('tests')
  subdir('tests')
endif

# end of file
