rem �л���.protoЭ�����ڵ�Ŀ¼  
cd  ..  
rem ����ǰ�ļ����е�����Э���ļ�ת��Ϊlua�ļ�  
for %%i in (*.proto) do (    
echo %%i  
"pblua\protoc-gen-lua-master\protoc.exe" --plugin=protoc-gen-lua="pblua\protoc-gen-lua-master\plugin\protoc-gen-lua.bat" --lua_out=pblua. %%i  
  
)  
echo end  

cd pblua
start ProtocGenLuaTools.exe.lnk %cd% ..
