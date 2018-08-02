@echo off
set path=%path%;C:\Program Files\NSIS

makensis.exe /V3 /X"SetCompressor /solid lzma" !Main.nsi

