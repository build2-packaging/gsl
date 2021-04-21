./: lib{gsl}: gsl/hxx{**}
{
  cxx.export.poptions = "-I$src_root"
}
hxx{*}: install.subdirs = true

./: tests/ manifest legal{LICENSE}
tests/: install = false

# Install UPSTREAM.README.md as README.md
# to make the package README.md the default
# for the build2 package repository web view.
./: doc{UPSTREAM.README.md}
doc{UPSTREAM.README.md}@./: install = doc/README.md

