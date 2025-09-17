@echo off 
del *.cs
set out_path=%cd%/../CSharp
cd ../ProtoGen
del *.proto
del *.sh
cd ../
copy *.proto ProtoGen
copy *.sh ProtoGen
cd ProtoGen
rem 查找文件  
for /f "delims=" %%i in ('dir.. /b "*.proto"') do echo %%i  
rem 转cpp  for /f "delims=" %%i in ('dir /b/a "*.proto"') do protoc -I=. --cpp_out=. %%i  
for /f "delims=" %%i in ('dir /b/a "*.proto"') do protogen -i:%%i -o:%out_path%/%%~ni.cs  
del *.proto
del *.sh
cd ../CSharp
ren Misc.cs MiscProto.cs
ren Team.cs TeamProto.cs
ren Task.cs TaskProto.cs
pause 
