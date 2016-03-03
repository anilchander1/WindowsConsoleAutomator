# WindowsConsoleAutomator
Automate Interactive Win32 Console Programs 

#Why?
Because i did not find any portable low footprint utility to automate the windows console based programs , especially interactive programs.Python based modules like wexpect had dependencies and was not easily portable for my needs.Also such modules need python familiarity.

#How to build?
Open the project in Visual Studio 2012 or later and build.You will get <200 KB file, integrated.exe.If lazy, use the attached integrated.exe

#usage 
integrated.exe linux.ini<br>
where "integrated.exe" is the name of the executable and ini file is the file to control the intented program.The execuatble file will read the ini file contents into memory.Then it will set any working directory specified for the session,set the environment variables for the session and start running the program specified with arguments ,if any.How the program needs to be terminated is specified by END keyword.<br>
<p>
#Sections of ini file<br><hr>
[Path]<br>
Under this section you can set the working directory using  "key=value" style specification.Key can be any unique string.The log files for the current session will be created in the specified directory.<br>
Example:  PATH1=C:\MYDIRECTORY<br>

-------------------------------------------------------------------------------------------------
[EnvironmentVariables]<br>
Under this section specify all the environment variables those need to be set.All of them should be in a "key=value" format, where key is the environment variable and value is its value.<br>
Example: JAVA_HOME=C:\JDK1.5


---------------------------------------------------------------------------------------------------
[ProgramSection]<br>
Under this section specify the program name to be run and arguments for the program .All of them should be in a "key=value" format, where key "ProgramName" is for the program to be run and key "Arguments" is for the program's arguments.These keys should not be changed.For key "ProgramName" value is mandatory & should be an absolute path to the program,however for key "Arguments" value can be blank(but just keep the key there).<br>
Example: 
ProgramName=C:\mydirectory\mypgm.exe<br>
Arguments=/F /D<br>



***You can nest program call inside another program like :<br>
ProgramName=cmd<br>
Arguments=C:\mydirectory\mypgm.exe /F /D<br>


-------------------------------------------------------------------------------------------------
[SequenceCommands]<br>
This section specifies how the program to be controlled at run time by emulating a virtual user.

These are some sequence of steps specifed in keyvalue pair ,ending with the last step having key "END".<br>

Key should be unique and it also serves as  description of that step.The value can have a mximum of 5 parameters seperated by pipe delimiter.<br>

Parameters are:<br><br>
<ol>
<li>An expected value at the current prompt.The current prompt(where the current cursor position is) will be matched against this value.If you suffix the key with '#' then this value will be checked in last commands execution result(The whole buffer string just before the line of current cursor position).In both cases the values can be regularexpressions.<strong>mandatory</strong></li>
<li>A command to be executed when parameter 1 is matched.If "{ENTER}" is given as a value,it just simulates hitting return/enter key.A blank or space will type a space and then enter/return will be hit.<strong>mandatory</strong></li>
<li>Key of the next step to be executed when parameter 1 is matched & param 2 is executed.<strong>not mandatory, if the next two parameters are not given.In this case the next step in sequence will be executed</strong></li>
<li>Key of the next step to be executed when parameter 1 is not matched.<strong>not mandatory, if the next  parameter is not given.In this case the next step will be the step with key "END"</strong></li>
<li>Maximum timeout in seconds.The current prompt will be checked in every 1-2 seconds until the timeout is reached.<strong>not mandatory.In this case 25 seconds will be the default timeout</strong></li>
</ol>
<br>
Any line starting  with <strong>';'</strong> anywhere in ini file will be treated as a comment.<br>
<strong>The END step needs only a single value: "which ever command to be executed to exit the program"</strong><br>

Example: <br>

[SequenceCommands]<br>
login=.\*login.\*|user1|password|END<br>
password=.\*password.\*|passwd|prompt|END<br>
;execute somecommand when prompt contains username<br>
prompt=.\*user1.\*>|somecommand|ls#|END|3<br>
;check the result of somecommand execution contains "hist" and go to end.<br>
ls#=.\*hist.\*|END<br>
END=logout<br>


---------------------------------------------------------------------------------------------------




Sample Ini files are attached.<br>

---------------------------------------------------------------------------------------------------


#Credits & thanks:<br>
----------------------<br>
https://github.com/easylogging/easyloggingpp - I used for logging<br>
https://github.com/brofield/simpleini - To read Ini files<br>
