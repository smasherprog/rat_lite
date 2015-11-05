<h3>Remote Access Library</h3>
<p>To build this library, you need to download two additional repos that I am maintaining:<br/>
https://github.com/smasherprog/Mouse_Capture<br/> and <br/>https://github.com/smasherprog/Screen_Capture</p>
<p>Place these libraries in the same folder as this library, for example this is what it should look like:<br/><img src="https://raw.githubusercontent.com/smasherprog/Remote_Access_Library/master/layout.PNG"/></p>
<p>Once you have the repos, you can start building. There is a Client.sln, which builds the application you would give to a person and tell them to run so you can connect to their computer. The Viewer.sln builds an application that you use to see and interact with the Client appliction.</p>
<h4>Note for Contributors:</h4>
<p>Please search for cross-platform solutions to problems. If there are none, please try to seperate the platform specific code into their own .cpp files and #ifdef the code.</p>
<h3>LICENSE:</h3>
<p>https://tldrlegal.com/license/mit-license MIT</p>
