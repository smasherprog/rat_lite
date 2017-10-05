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
<li><a href="http://www.nasm.us/">NASM Assembler</a><p>For mac, use homebrew to install nasm<p></li>
<li><a href="https://cmake.org/download/">Cmake (Latest!)</a></li>
<li><a href="https://nodejs.org/en/download/">Node.js</a></li>
<li>TypeScript install: npm install typescript -g</li>
</ul>
<p>The library is not yet stable and in development . . . </p>
<h3>Mac issues</h3>
<p>To build on mac, I only support command line builds. There are too many strange issues when creating an Xcode project. So, to build with mac, you would have to do something like: cmake -DBUILD_SHARED_LIBS=OFF -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. && make </p>

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


