from git import Repo
import os, errno, argparse, shutil

def removefile(filename):
	try:
		shutil.rmtree(filename)
	except OSError as e:
		if e.errno != errno.ENOENT:
			raise

parser = argparse.ArgumentParser(description='Create a Git repo in a script!')
parser.add_argument('--remote', help="Remote URL")
parser.add_argument('--dir', help="Output directory")
args = parser.parse_args()
if args.dir is None:
	args.dir = "out"
removefile(args.dir)
bare_repo = Repo.init(args.dir)
assert bare_repo
origin = bare_repo.create_remote('origin', args.remote)
origin.fetch()
origin.pull(origin.refs[0].remote_head)