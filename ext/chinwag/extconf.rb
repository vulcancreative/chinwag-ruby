require 'mkmf'

extension_name = "chinwag"
have_header("ruby/io.h")

$CFLAGS = "-std=c99 -O3"

# dir_config(extension_name)
create_makefile(extension_name)
