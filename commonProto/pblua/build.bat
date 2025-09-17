rem 切换到.proto协议所在的目录  
cd  ..  
rem 将当前文件夹中的所有协议文件转换为lua文件  
for %%i in (*.proto) do (    
echo %%i  
"pblua\protoc-gen-lua-master\protoc.exe" --plugin=protoc-gen-lua="pblua\protoc-gen-lua-master\plugin\protoc-gen-lua.bat" --lua_out=pblua. %%i  
  
)  
echo end  

cd pblua
start ProtocGenLuaTools.exe.lnk %cd% ..
