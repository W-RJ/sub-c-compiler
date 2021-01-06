'''
    Tests of SCC.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
'''

import os
import shutil

input_dir = 'input'
output_dir = 'output'

class TestClass:

    def setup(self):
        self.cwd = os.getcwd()

    def teardown(self):
        os.chdir(self.cwd)

    def test_text(self, tmpdir):
        scc = os.environ['SCC']
        shutil.rmtree(tmpdir.join(input_dir), True)
        shutil.copytree(os.path.join(os.path.dirname(__file__), input_dir), tmpdir.join(input_dir))
        os.system("cp " + scc + ' "' + str(tmpdir.join(input_dir)) + '"')
        os.chdir(tmpdir.join(input_dir))
        os.rename("sc.lang", "sc2.lang")
        assert os.system("timeout 1 ./scc test.sc -G sc2.lang -e result.txt -p @ -P -t > cout.txt 2> cerr.txt") == 0
        assert os.system("timeout 5 ./sci -t test.tpc < iin.txt > iout.txt 2> ierr.txt") == 0
        os.system("rm scc sci sc2.lang test.sc test.tpc iin.txt")
        assert os.system('diff "' + str(os.path.join(os.path.dirname(__file__), output_dir)) + '" "' + str(tmpdir.join(input_dir)) + '"') == 0

    def test_bin(self, tmpdir):
        scc = os.environ['SCC']
        shutil.rmtree(tmpdir.join(input_dir), True)
        shutil.copytree(os.path.join(os.path.dirname(__file__), input_dir), tmpdir.join(input_dir))
        os.system("cp " + scc + ' "' + str(tmpdir.join(input_dir)) + '"')
        os.chdir(tmpdir.join(input_dir))
        assert os.system("timeout 1 ./scc - -e result.txt -p @ -o - < test.sc > test.bpc 2> cerr.txt") == 0
        assert os.system("timeout 5 ./sci test.bpc < iin.txt > iout.txt 2> ierr.txt") == 0
        os.system("touch cout.txt")
        os.system("rm scc sci sc.lang test.sc test.bpc iin.txt")
        assert os.system('diff "' + str(os.path.join(os.path.dirname(__file__), output_dir)) + '" "' + str(tmpdir.join(input_dir)) + '"') == 0
