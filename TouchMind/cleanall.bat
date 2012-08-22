del ..\TouchMind\*.fxo

del ..\TouchMindMUI_en_us\ribbonmarkup.bml
del ..\TouchMindMUI_en_us\ribbonres.h
del ..\TouchMindMUI_en_us\ribbonres.rc

del ..\TouchMindMUI_ja_jp\ribbonmarkup.bml
del ..\TouchMindMUI_ja_jp\ribbonres.h
del ..\TouchMindMUI_ja_jp\ribbonres.rc

del ..\TouchMindMUI_nl_nl\ribbonmarkup.bml
del ..\TouchMindMUI_nl_nl\ribbonres.h
del ..\TouchMindMUI_nl_nl\ribbonres.rc

rmdir /s /q ..\Debug
rmdir /s /q ..\Release
rmdir /s /q ..\x64
rmdir /s /q ..\ipch

rmdir /s /q ..\TouchMind\Debug
rmdir /s /q ..\TouchMind\Release
rmdir /s /q ..\TouchMind\x64

rmdir /s /q ..\TouchMindLib\Debug
rmdir /s /q ..\TouchMindLib\Release
rmdir /s /q ..\TouchMindLib\x64

rmdir /s /q ..\TouchMindLib_Test\Debug
rmdir /s /q ..\TouchMindLib_Test\Release
rmdir /s /q ..\TouchMindLib_Test\x64

rmdir /s /q ..\TouchMindMUI_en_us\Debug
rmdir /s /q ..\TouchMindMUI_en_us\Release
rmdir /s /q ..\TouchMindMUI_en_us\x64

rmdir /s /q ..\TouchMindMUI_ja_jp\Debug
rmdir /s /q ..\TouchMindMUI_ja_jp\Release
rmdir /s /q ..\TouchMindMUI_ja_jp\x64

rmdir /s /q ..\TouchMindMUI_nl_nl\Debug
rmdir /s /q ..\TouchMindMUI_nl_nl\Release
rmdir /s /q ..\TouchMindMUI_nl_nl\x64

rmdir /s /q ..\TouchMindWix\bin
rmdir /s /q ..\TouchMindWix\obj

del /s ..\*.vcxproj.user
del /s ..\*.orig
del ..\TouchMind.sdf
del /A:H ..\TouchMind.suo
