#!/bin/bash
# Cheesy testing script. Extracts expected results from tests.scm, gets the
# actual results by running tests.scm through bs, then shows a diff of the
# two.
#
# The tests.scm file should look like this:
#
# 1                             ; 1
# (define a 23)                 ; ok
# #t                            ; #t
#
# And so on. The code to be executed is on the left, and the expected result
# on the right, after a semicolon and a space.
#
# Hacked together in about 10 minutes, and it shows.

echo "Rebuilding bs (if needed)"
pushd .. > /dev/null
scons -s
popd > /dev/null

echo Extracting expected results
awk 'BEGIN { FS = "; " } { print $2 }' < tests.scm > expected
echo Running tests
../bs tests.scm > actual
echo Comparing results
diff -s --suppress-common-lines --width=80 -U 0 expected actual
rm expected actual

