#!/usr/bin/env bash

# dependencies are fetched on windows only
SUPPORTED=1
(uname -a | grep MINGW64) || SUPPORTED=0
if [ $SUPPORTED -eq 0 ]; then
	echo "Sorry, this platform is not supported"
	exit 1
fi

if [[ $(python --version | grep -q "3.6") ]]; then
	# Fetch python
	if [ ! -e python35.exe ]; then
		echo "Fetching python, please wait"
		curl https://www.python.org/ftp/python/3.6.5/python-3.6.5.exe -o python35.exe
	fi
	# Install it
	echo "If python is not yet installed, install it"
	./python35.exe
else
	echo "Found python!"
fi

# installing meson
echo "Installing meson"
pip3 install meson

if [[ $(ninja --version | grep -q "1.8") ]]; then
	echo "Installing ninja"

	if [ ! -e ninja182.zip ]; then
		curl -L https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip -o ninja182.zip
	fi
	unzip ninja182.zip
	mv ninja $(dirname $(realpath $(which python)))/

	echo "Ninja should be ready, you should see it's version here (v1.8.2)"
	ninja --version
else
    echo "Found Ninja!"
fi

echo "All done!"
echo "Now you can run meson inside a window native command prompt, trying to spawn it for you"
explorer.exe 'C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Visual Studio 2017\Visual Studio Tools\VC\x64 Native Tools Command Prompt for VS 2017.lnk'

echo "You should run there:"
echo 
echo "meson.py --buildtype release --backend vs2017 buildVS2017Rel"
echo "meson.py --buildtype release --backend ninja buildVS2017NinjaRel"
echo "ninja -C buildVS2017NinjaRel"