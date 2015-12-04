@echo off
REM Create version_in.h if it doesn't exist.
if not exist "..\src\filters\transform\vsfilter\version_in.h" (
  cd.>"..\src\filters\transform\vsfilter\version_in.h"
 )
exit /b 0
