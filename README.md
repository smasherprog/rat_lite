<h2>All development happens in the master branch. To get stable releases, use tags by going to the release section.</h2>

<h3>Remote Access Library</h3>
<p>Linux/Mac <img src="https://travis-ci.org/smasherprog/rat_lite.svg?branch=master"/><p>
<p>Windows <img src="https://ci.appveyor.com/api/projects/status/vmascb1xd2ogy8af"/><p>

<p>This library is intended to be a VNC-like application, but with a few design goals:
<ul>
<li>
Cross-platform:Windows, Linux, Modern Web Browsers, Andriod, Mac
</li>
<li>
Performance 
</li>
<li>
Encryption (openssl)
</li>
<li>
Latest standards: c++ 17 
</li>
</ul>
<h3>Required tools</h3>
<ul>
<li><a href="https://cmake.org/download/">Cmake (Latest!)</a></li>
<li><a href="https://nodejs.org/en/download/">Node.js (Latest!)</a>
</li>

</ul>
<h3>Libraries</h3>
<ul>
<li>TypeScript/Electron install: npm install -g typescript electron </li>
<li>Mac: brew install openssl zlib libjpeg-turbo</li>
<li>Windows: vcpkg install openssl zlib libjpeg-turbo</li>
<li>Ubutnu: sudo apt-get npm install libxext-dev libx11-dev libxfixes-dev libxinerama-dev libxtst-dev libjpeg-turbo8-dev libssl-dev zlib1g-dev -y</li>
</ul>

<h3>Building</h3>
<p>Below are examples to build. Some paths, like windows vcpkg will need to be updated to your location to the vcpkg.cmake</p>
<ul>
<li>Mac: cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl . && make</li>
<li>Windows: cmake -DCMAKE_TOOLCHAIN_FILE=c:\projects\build\vcpkg\scripts\buildsystems\vcpkg.cmake . </li>
<li>Ubutnu: cmake . && make</li>
</ul>

<h5>Windows Connecting to Mac</h5>
<img style="width:50%;float:left;" src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/Windows_Mac.PNG" target="_blank"/>

<h5>Mac Connecting to Windows</h5>
<img src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/Mac_Windows.PNG" target="_blank"/>

<h5>Windows Connecting to Android</h5>
<img style="width:50%;float:left;" src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/Android.PNG" target="_blank"/>
<img style="width:50%;float:left;" src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/android2.PNG" target="_blank"/>

<h5>Windows Connecting to Linux</h5>
<img src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/Windows_to_Linux.png" target="_blank"/>

<h5>Linux Connecting to Windows</h5>
<img src="https://raw.githubusercontent.com/smasherprog/Projects_Setup/master/rat_content/linux_to_windows.png" target="_blank"/>


