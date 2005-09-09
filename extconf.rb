require 'mkmf'

have_library('epeg', 'epeg_file_open') || die("Need libepeg")
create_makefile('epeg')
