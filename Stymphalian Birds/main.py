from git import Repo
import os, errno, argparse, shutil

def removefile(filename):
	try:
		shutil.rmtree(filename)
	except OSError as e:
		if e.errno != errno.ENOENT:
			raise
def ignore_specific(language):
	return {
		"C": "# Prerequisites\n*.d\n\n# Object files\n*.o\n*.ko\n*.obj\n*.elf\n\n# Linker output\n*.ilk\n*.map\n*.exp\n\n# Precompiled Headers\n*.gch\n*.pch\n\n# Libraries\n*.lib\n*.a\n*.la\n*.lo\n\n# Shared objects (inc. Windows DLLs)\n*.dll\n*.so\n*.so.*\n*.dylib\n\n# Executables\n*.exe\n*.out\n*.app\n*.i*86\n*.x86_64\n*.hex\n\n# Debug files\n*.dSYM/\n*.su\n*.idb\n*.pdb\n\n# Kernel Module Compile Results\n*.mod*\n*.cmd\n.tmp_versions/\nmodules.order\nModule.symvers\nMkfile.old\ndkms.conf",
		"Go": "# Binaries for programs and plugins\n*.exe\n*.exe~\n*.dll\n*.so\n*.dylib\n\n# Test binary, build with `go test -c`\n*.test\n\n# Output of the go coverage tool, specifically when used with LiteIDE\n*.out",
		"C++": "# Prerequisites\n*.d\n\n# Compiled Object files\n*.slo\n*.lo\n*.o\n*.obj\n\n# Precompiled Headers\n*.gch\n*.pch\n\n# Compiled Dynamic libraries\n*.so\n*.dylib\n*.dll\n\n# Fortran module files\n*.mod\n*.smod\n\n# Compiled Static libraries\n*.lai\n*.la\n*.a\n*.lib\n\n# Executables\n*.exe\n*.out\n*.app\n",
		"Perl": "!Build/\n.last_cover_stats\n/META.yml\n/META.json\n/MYMETA.*\n*.o\n*.pm.tdy\n*.bs\n\n# Devel::Cover\ncover_db/\n\n# Devel::NYTProf\nnytprof.out\n\n# Dizt::Zilla\n/.build/\n\n# Module::Build\n_build/\nBuild\nBuild.bat\n\n# Module::Install\ninc/\n\n# ExtUtils::MakeMaker\n/blib/\n/_eumm/\n/*.gz\n/Makefile\n/Makefile.old\n/MANIFEST.bak\n/pm_to_blib\n/*.zip\n",
	}[language]

def add_ignore(directory, language):
	f = open('/'.join((directory, ".gitignore")), "wa")
	res = ignore_specific(language)
	f.write(res)
	f.close()

parser = argparse.ArgumentParser(description='Create a Git repo in a script!')
parser.add_argument('--remote', help="Remote URL", required=True)
parser.add_argument('--dir', help="Output directory")
parser.add_argument('--ignore', help="Language you're using to make a gitignore based off of Languages supported [C, Go, C++, and Perl]")
args = parser.parse_args()
if args.dir is None:
	args.dir = "out"
removefile(args.dir)
bare_repo = Repo.init(args.dir)
assert bare_repo
origin = bare_repo.create_remote('origin', args.remote)
origin.fetch()
origin.pull(origin.refs[0].remote_head)
if args.ignore is not None:
	add_ignore(args.dir, args.ignore)