#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

import subprocess
from TestHarnessTestCase import TestHarnessTestCase

class TestHarnessTester(TestHarnessTestCase):
    def testDiffs(self):
        """
        Test for error in input to CSVDiff
        """
        with self.assertRaises(subprocess.CalledProcessError) as cm:
            self.runTests('-i', 'csvdiffs')

        e = cm.exception
        self.assertRegexpMatches(e.output, r'test_harness\.test_csvdiff.*?FAILED \(Override inputs not the same length\)')
        self.assertRegexpMatches(e.output, r'test_harness\.test_badfile.*?FAILED \(MISSING GOLD FILE\)')
        self.checkStatus(e.output, failed=2)
