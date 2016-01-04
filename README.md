<h3>Remote Access Library</h3>
<p>The library is not yet stable and in development . . . </p>


<hr/>
<h3>Visual Studio Development</h3>

<p><b>Client.sln:</b> Builds an application you would give to a person and tell them to run so you can connect to their computer.<br/> 
<b>Viewer.sln:</b> Builds an application that you use to see and interact with the Client application.
</p>
<h4>Visual Studio 2015 setup</h4>
<h5>Dependencies:</h5>

<b>wxWidgets</b>

<ul>
<li>
Download the latest release from here https://github.com/wxWidgets/wxWidgets/releases. Currently working with 3.0.2
</li>
<li>
Extract to c:\wxWidgets
</li>
<li>
Open the latest solution file in C:\wxWidgets\build\msw\    as of 3.0.2  the solution is wx_vc12.sln
<br/>
</li>
<li>
Build all of the libraries in Visual studio by going to Build -> Batch Build -> Select All -> Build . . . .  This will take a while while the build performs.
</li>
</ul>
<p>If you attempt to build 3.0.2 and receive a the build error<br/>
<b>C1189 #error: Macro definition of snprintf conflicts with Standard Library function declaration</b><br/>
The fix is to open the file c:\wxwidgets\src\tiff\libtiff\tif_config.h, goto line 367 and replace it with the following<br/>
<pre>
#if (defined(_MSC_VER) && (_MSC_VER < 1900))
  #define snprintf _snprintf
#endif
</pre>
Now, batch build and the compile will succeed.
</p>


<b>Boost</b>
<ul>
<li>
Download the latest release from here http://sourceforge.net/projects/boost/files/boost-binaries/  Currently working with 1.60<br/>
For example, goto 1.60.0 folder, then download boost_1_60_0-msvc-14.0-64.exe     and  boost_1_60_0-msvc-14.0-32.exe
</li>
<li>
Install to c:\boost
</li>
</ul>




<h3>LICENSE:</h3>
<p>https://tldrlegal.com/license/mit-license MIT</p>
