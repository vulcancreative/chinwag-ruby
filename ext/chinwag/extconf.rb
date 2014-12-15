require 'mkmf'

extension_name = "chinwag"

$CFLAGS = "-std=c99 -O3"

# dir_config(extension_name)
create_makefile(extension_name)
