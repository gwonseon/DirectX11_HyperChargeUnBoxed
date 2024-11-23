xcopy	/y/s	.\Engine\Public\*.h			.\EngineSDK\inc\

xcopy	/y		.\Engine\Bin\Engine.dll		.\Client\Bin\		
xcopy	/y		.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\	

xcopy	/y /s	.\Engine\Bin\ShaderFiles\*.hlsli	.\Client\Bin\ShaderFiles\
xcopy	/y /s	.\Engine\Bin\ShaderFiles\*.hlsl	.\Client\Bin\ShaderFiles\