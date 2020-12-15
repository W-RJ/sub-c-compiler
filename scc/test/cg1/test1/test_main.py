import os
import shutil

input_dir = 'input'
output_dir = 'output'

def test_main(tmpdir):
    scc = os.environ['SCC']
    shutil.copytree(os.path.join(os.path.dirname(__file__), input_dir), tmpdir.join(input_dir))
    os.chdir(tmpdir.join(input_dir))
    os.system(scc + " > cout.txt")
    assert os.system('diff "' + str(os.path.join(os.path.dirname(__file__), output_dir)) + '" "' + str(tmpdir.join(input_dir)) + '"') == 0
