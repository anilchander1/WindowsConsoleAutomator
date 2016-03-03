# WindowsConsoleAutomator
Automate Interactive Win32 Console Programs 

#Why?
Because i did not find any portable low footprint utility to automate the windows console based programs , especially interactive programs.Python based modules like wexpect had dependencies and was not easily portable for my needs.Also such modules need python familiarity.

#How to build?
Open the project in Visual Studio 2012 or later and build.You will get <200 KB file, integrated.exe.If lazy, use the attached integrated.exe

#usage 
integrated.exe linux.ini<br>
where "integrated.exe" is the name of the executable and ini file is the file to control the intented program.The execuatble file will read the ini file contents into memory 


Sample Ini files are attached.


#Credits & thanks:<br>
----------------------<br>
https://github.com/easylogging/easyloggingpp - I used for logging<br>
https://github.com/brofield/simpleini - To read Ini files<br>
