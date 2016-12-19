@echo off

set path="PATH WHERE THIS FILE IS USE %UserProfile% TO GET TO YOUR USER DIRECTORY"
REM path="%UserProfile%\Desktop\Github\..." BEISPIEL

echo Folder TheTime created on your desktop
echo link the Arduino IDE right to that folder
echo either copy the folder to your scratchbook
echo or choose to open the main file in the project
echo %cd%

mkdir "%UserProfile%\Desktop\TheTime"
mklink "%UserProfile%\Desktop\TheTime\Hackathon_Wecker" "src"
mklink "%UserProfile%\Desktop\TheTime\libraries" "libraries"
mklink "%UserProfile%\Desktop\TheTime\tools" "tools"
pause

@echo on