@if "%echo%"=="" (@echo off) else (@echo on)

pushd .
cls
REM ======================================================================
REM
REM Copyright Microsoft Corp. 2009
REM
REM Module : ParseData.cmd
REM
REM Summary: Command batch file to execute Amalga tests.
REM
REM History: (8/28/2008) manag - Initial Coding.
REM
REM ======================================================================


REM ======================================================================
REM Ensure proper parameters entered
REM ======================================================================
if {%1} equ {}   (goto :Usage)
if {%1} equ {/?} (goto :Usage)
if {%1} equ {?}  (goto :Usage)
if {%1} equ {-?} (goto :Usage)

REM ======================================================================
REM Setup envronment variables
REM ======================================================================
set LOGFILE=%SYSTEMDRIVE%\ParseData.log
del %LOGFILE%

call :LogMsg Extracting Reversed Messages from cab file
%SYSTEMROOT%\System32\Extrac32.exe /e /a /y /l %TEMP% %1

set MESSAGES=%TEMP%\ReversedMessages
set XCOPY=xcopy /qchives

call :LogMsg Changing to ReverseMessages Folder %MESSAGES%
pushd %MESSAGES%
call :LogMsg Copying data for ADT Messages to be parsed.
set MESSAGEFEED=ADT
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for DICTATION Messages to be parsed.
set MESSAGEFEED=DIC
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for DIET ORDER Messages to be parsed.
set MESSAGEFEED=GDIET.ORM
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for LAB ORDER Messages to be parsed.
set MESSAGEFEED=GLAB.ORM
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for LAB RESULT Messages to be parsed.
set MESSAGEFEED=LAB.ORU
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for LAB OSU Messages to be parsed.
set MESSAGEFEED=LAB.OSU
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for MEDORDER Messages to be parsed.
set MESSAGEFEED=MEDORDER
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for MEDORDER Messages to be parsed.
set MESSAGEFEED=MUSE
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for PATHOLOGY Messages to be parsed.
set MESSAGEFEED=PATH.ORU
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for PYXIX Messages to be parsed.
set MESSAGEFEED=PYXIS
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for RADIOLOGY ORDER Messages to be parsed.
set MESSAGEFEED=GRAD.ORM
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for RADIOLOGY RESULT Messages to be parsed.
set MESSAGEFEED=RAD.ORU
%XCOPY% *.RAW.%MESSAGEFEED% "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for PICIS HL7 Messages to be parsed.
set MESSAGEFEED=PICIS.H
%XCOPY% "PDF\*.hl7" "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

call :LogMsg Copying data for PICIS PDF Messages to be parsed.
set MESSAGEFEED=PICIS.I
%XCOPY% "PDF\*.pdf" "%ProgramFiles%\Microsoft Amalga\2.0\DADEServer\MessageReceiver\Amalga.2.0-DADEReceiver-%MESSAGEFEED%\Validated"

popd



REM ======================================================================
REM Logging Function - log to screen and file
REM ======================================================================
:LogMsg

setlocal
set STRMSG=%TIME% - %*
echo %STRMSG%
echo %STRMSG% >> %LOGFILE%
endlocal
goto :EOF



REM ======================================================================
REM Usage
REM ======================================================================
:Usage
echo.
echo Usage:
echo Description:
echo.
echo    Command batch file to parse reversed messages.
echo    pre-Requisites are installed.
echo        1) Amalga V2.0 installed using SetupAmalga.cmd
echo.
echo Example: ParseData.cmd  [HL7MessageCabFile]
echo.
echo Required Parameters:
echo    [HL7MessageCabFile]  - Cab file containing all the reversed messages
echo    Example:
echo        ParseData.cmd C:\AmalgaTest\Infrastructure\TestInputData\ReversedMessages.cab
echo.
goto :End

:End
popd
