' ***********************************************************************
'
' Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
' 
' This file is part of Photon (photonsoftware.org).
' 
' This file may be distributed and/or modified under the terms of the
' GNU General Public License version 2 or version 3 as published by the 
' Free Software Foundation and appearing in the file COPYING.GPL2 and 
' COPYING.GPL3 included in the packaging of this software.
'
' Licensees holding a valid Photon Commercial license can use this file 
' in accordance with the terms of their license.
' 
' This software is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
'
' ***********************************************************************

Option Explicit

' ***********************************************************************
'
' Declaration of variables
'
' ***********************************************************************

Dim rc, numTests, numFailed, verbose, status
Dim objShell
Dim objWshScriptExec
Dim fso

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(70)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(15)
Dim fail_programs(54)

Dim exe_name, prog_path, program, dll_path
Dim consoleMode
Dim objArgs, strArg, i
dim strOutput

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
ok_programs(0)  = "FiniPass"
ok_programs(1)  = "GetFirstPass"
ok_programs(2)  = "GetLastPass"
ok_programs(3)  = "InitPass"
ok_programs(4)  = "IsValidPass"
ok_programs(5)  = "PeakFirstPass"
ok_programs(6)  = "PeakLastPass"
ok_programs(7)  = "PeakNextPass"
ok_programs(8)  = "PeakPreviousPass"
ok_programs(9)  = "PutFirstPass"
ok_programs(10) = "PutLastPass"
ok_programs(11) = "RemovePass"
ok_programs(12) = "ReplacePass"
ok_programs(13) = "ResetPass"
ok_programs(14) = "TestSmall"
ok_programs(15) = "TestBig"

fail_programs(0)  = "FiniInvSig"
fail_programs(1)  = "FiniNullList"
fail_programs(2)  = "GetFirstInvSig"
fail_programs(3)  = "GetFirstNullItem"
fail_programs(4)  = "GetFirstNullList"
fail_programs(5)  = "GetLastInvSig"
fail_programs(6)  = "GetLastNullItem"
fail_programs(7)  = "GetLastNullList"
fail_programs(8)  = "InitNullList"
fail_programs(9)  = "IsValidInvSig"
fail_programs(10) = "IsValidNullList"
fail_programs(11) = "IsValidNullUnknown"
fail_programs(12) = "PeakFirstInvSig"
fail_programs(13) = "PeakFirstNullItem"
fail_programs(14) = "PeakFirstNullList"
fail_programs(15) = "PeakLastInvSig"
fail_programs(16) = "PeakLastNullItem"
fail_programs(17) = "PeakLastNullList"
fail_programs(18) = "PeakNextInvSig"
fail_programs(19) = "PeakNextNullCurrent"
fail_programs(20) = "PeakNextNullCurrNextOffset"
fail_programs(21) = "PeakNextNullCurrPrevOffset"
fail_programs(22) = "PeakNextNullList"
fail_programs(23) = "PeakNextNullNext"
fail_programs(24) = "PeakPreviousInvSig"
fail_programs(25) = "PeakPreviousNullCurrent"
fail_programs(26) = "PeakPrevNullCurrNextOffset"
fail_programs(27) = "PeakPrevNullCurrPrevOffset"
fail_programs(28) = "PeakPreviousNullList"
fail_programs(29) = "PeakPreviousNullPrevious"
fail_programs(30) = "PutFirstInvSig"
fail_programs(31) = "PutFirstNullList"
fail_programs(32) = "PutFirstNullNewItem"
fail_programs(33) = "PutFirstNullNewNext"
fail_programs(34) = "PutFirstNullNewPrevious"
fail_programs(35) = "PutLastInvSig"
fail_programs(36) = "PutLastNullList"
fail_programs(37) = "PutLastNullNewItem"
fail_programs(38) = "PutLastNullNewNext"
fail_programs(39) = "PutLastNullNewPrevious"
fail_programs(40) = "RemoveInvSig"
fail_programs(41) = "RemoveNullItem"
fail_programs(42) = "RemoveNullNextOffset"
fail_programs(43) = "RemoveNullPrevOffset"
fail_programs(44) = "RemoveNullList"
fail_programs(45) = "RemoveAbsent"
fail_programs(46) = "ReplaceInvSig"
fail_programs(47) = "ReplaceNullList"
fail_programs(48) = "ReplaceNullNew"
fail_programs(49) = "ReplaceNullNewNext"
fail_programs(50) = "ReplaceNullNewPrevious"
fail_programs(51) = "ReplaceNullOld"
fail_programs(52) = "ResetInvSig"
fail_programs(53) = "ResetNullList"
fail_programs(54) = "RemoveWrongItem"

numTests = 71                 ' Sum of length of both arrays 
numFailed = 0

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

Set objShell = CreateObject("WScript.Shell")
verbose = False

prog_path = "Release"
Set objArgs = WScript.Arguments

' ***********************************************************************
'
' Start the program
'
' ***********************************************************************

For Each strArg in objArgs
   If Left(LCase(strArg), 6) = "--path" AND Len(strArg) > 6 Then
      prog_path = Right(strArg, Len(strArg)-7)
   end if
   If Left(LCase(strArg), 9) = "--dllpath" AND Len(strArg) > 9 Then
      dll_path = Right(strArg, Len(strArg)-10)
   end if
   if LCase(strArg) = "--verbose" then 
      verbose = True
   end if
Next 

dim WshEnv
Set WshEnv = objShell.Environment("Process") 
WshEnv("Path") = WshEnv("Path") & ";" & fso.GetAbsolutePathName(dll_path)

if Not consoleMode then
   wscript.echo "Be patient - running the tests in batch mode - click ok to start"
end if

For Each program in ok_programs
   exe_name = prog_path & "\" & program & ".exe"
   if consoleMode then 
      WScript.Echo "Running " & exe_name
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exe_name & Chr(34))
      status = objWshScriptExec.Status
      Do While objWshScriptExec.Status = 0
         WScript.Sleep 100
      Loop
      strOutput = objWshScriptExec.StdOut.ReadAll
      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & exe_name & Chr(34), 2, true)
   end if
   if rc <> 0 then
      failed_tests(numFailed) = program
      numFailed = numFailed + 1
   end if
Next

For Each program in fail_programs
   exe_name = prog_path & "\" & program & ".exe"
   if consoleMode then 
      WScript.Echo "Running " & exe_name
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exe_name & Chr(34))
      status = objWshScriptExec.Status
      Do While objWshScriptExec.Status = 0
         WScript.Sleep 100
      Loop
      strOutput = objWshScriptExec.StdOut.ReadAll
      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & exe_name & Chr(34), 2, true)
   end if
   if rc = 0 then
      failed_tests(numFailed) = program
      numFailed = numFailed + 1
   end if
Next

if consoleMode then
   WScript.StdOut.Write vbcrlf & "Total number of tests: " & numTests & vbcrlf
   WScript.StdOut.Write "Total number of failed tests: " & numFailed & vbcrlf & vbcrlf
   For i = 1 to numFailed 
      WScript.StdOut.Write "This test failed: " & failed_tests(i-1) & vbcrlf
   Next
   if verbose then
      WScript.StdOut.Write "Type return to continue"
      Dim dummy
      dummy = WScript.StdIn.Read(1)
   end if
else
   wscript.echo "Total number of tests: " & numTests & vbcrlf & _
      "Total number of failed tests: " & numFailed
end if

if numFailed > 0 then wscript.quit(1)
wscript.quit(0)

