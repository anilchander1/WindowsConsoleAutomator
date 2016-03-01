# WindowsConsoleAutomator
Automate Interactive Win32 Console Programs 

#Why?
Because i did not find any portable low footprint utility to automate the windows console based programs , especially interactive programs.Python based modules like wexpect had dependencies and was not easily portable for my needs.

#How to use?
Open the project in Visual Studion 2012 or later and build.You will get <200 KB file, integrated.exe.

#You need to use it by feeding a preformatted ini file. like:
integrated.exe linux.ini

Sample Ini file:
--------------------------------
[linux.ini]

[Path]
;this is a comment
;if working directory need to be specified , give it here.Logs will be generated here
PATH1=
[EnvironmentVariables]
;give all the env variables to be set for the session here
PS_HOME=
[ProgramSection]
;ProgramName is the program to run Arguments self explanatory
;here ansicon is the program and plink is wrapped inside to support linux control characters
;ansicon download "https://github.com/adoxa/ansicon/downloads"
;plink comes with putty 
;this ini connects from windows to linux through plink and executes some commands.then logs out
ProgramName=ansicon
Arguments=plink -ssh <ur linux host>
[SequenceCommands]
;Here you mention the steps to be executed for the program.
;format is key=value and key should be unique
;sample key value format
;stepid=expected_prompt(regex supported)|what_command_to_be_executed_when_prompt_matches,space,no value are allowed to do ;nothing.{ENTER} will simulate a line feed  carriage return|which_step_id_to_be_executed_next_when_prompt_matches[optional, no value ;moves to next step]|which_step_id_to_be_executed_next_when_prompt_does_not_match[optional ,if preceding one is not ;given]|max_timeout([optional if preceding one is not given,default 25 sec])
;if stepid contains a '#' ,check will be done on previous command's output.Ex: ls#=.*hist.*|ls|END
;###########################################################
;login step checks prompt contains "login",if yes type userid given then goes to step password,else goes to step END.As timeout is not ;given,25 seconds will be given for a possible match to appear before going to step END.
login=.*login.*|<userid>|password|END 
password=.*password.*|<pwd>|prompt|END
;loop until prompt contains username
prompt=.*>|history|ls#|enter|3
enter=.*|{ENTER}|prompt|prompt|1
;loop ends
ls#=.*hist.*|ls|END
;END step id should be "END" and value can be usual commands like logout,exit etc.If a CTRL+C need to be executed to force terminate ;give "{}"
END=logout 


#Another Sample to run powershell 2009 version
-----------------------------------------------------
[powershell.ini]

[Path]
PATH1=
[EnvironmentVariables]
HOME=C:\PS_INST
[ProgramSection]
ProgramName=powershell
Arguments=
[SequenceCommands]
sendHelp#=.*2009 Microsoft.*|help|more1|sendHelp
more1=-- More  --| |prompt|END|10
prompt=PS.*||END|prompt
END=exit


#Credits & thanks:
----------------------
https://github.com/easylogging/easyloggingpp - I used for logging
https://github.com/brofield/simpleini - To read Ini files
