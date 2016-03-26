import sys
from cx_Freeze import setup, Executable


# Gather extra runtime dependencies.
def gather_extra_redist():
	import os
	import gs
	import inspect

	path = os.path.dirname(inspect.getfile(gs))
	files = os.listdir(path)

	out = []
	for file in files:
		name, ext = os.path.splitext(file)
		if ext in ['.dll', '.so'] and "Debug" not in name:
			out.append(os.path.join(path, file))

	return out


extra_redist = gather_extra_redist()

# Dependencies are automatically detected, but it might need fine tuning.
options = {
	'build_exe': {
		'compressed': True,
		'packages': ['gs'],
		'include_files': ['pkg.core/', 'assets/'] + extra_redist
	}
}

setup(  name = "Amos Demo",
		version = "1.0",
		description = "Amos Demo",
		options = options,
		executables = [Executable("amos_demo.py")])
