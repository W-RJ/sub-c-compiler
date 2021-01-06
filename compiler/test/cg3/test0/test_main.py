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

def test_main(tmpdir):
    scc = os.environ['SCC']
    shutil.copytree(os.path.join(os.path.dirname(__file__), input_dir), tmpdir.join(input_dir))
    shutil.copy(scc, tmpdir.join(input_dir))
    os.chdir(tmpdir.join(input_dir))
    assert os.system("timeout 1 ./scc > cout.txt") == 0
    os.system("rm scc sc.lang testfile.txt")
    assert os.system('diff "' + str(os.path.join(os.path.dirname(__file__), output_dir)) + '" "' + str(tmpdir.join(input_dir)) + '"') == 0
