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
Dim failed_tests(75)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(18)
Dim fail_programs(56)

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
ok_programs(0)  = "CommitAddPass"
ok_programs(1)  = "CommitRemovePass"
ok_programs(2)  = "DeletePass"
ok_programs(3)  = "FiniPass"
ok_programs(4)  = "GetFirstPass"
ok_programs(5)  = "GetFirstWrongKeyLength"
ok_programs(6)  = "GetFirstWrongLength"
ok_programs(7)  = "GetNextPass"
ok_programs(8)  = "GetNextWrongKeyLength"
ok_programs(9)  = "GetNextWrongLength"
ok_programs(10) = "GetPass"
ok_programs(11) = "GetWrongLength"
ok_programs(12) = "InitPass"
ok_programs(13) = "InsertPass"
ok_programs(14) = "ReleasePass"
ok_programs(15) = "ReplacePass"
ok_programs(16) = "RollbackAddPass"
ok_programs(17) = "RollbackRemovePass"
ok_programs(18) = "StatusPass"

fail_programs(0)  = "CommitAddNullContext"
fail_programs(1)  = "CommitAddNullHash"
fail_programs(2)  = "CommitAddNullItem"
fail_programs(3)  = "CommitRemoveNullContext"
fail_programs(4)  = "CommitRemoveNullHash"
fail_programs(5)  = "CommitRemoveNullItem"
fail_programs(6)  = "DeleteNullContext"
fail_programs(7)  = "DeleteNullHash"
fail_programs(8)  = "DeleteNullKey"
fail_programs(9)  = "DeleteZeroKeyLength"
fail_programs(10) = "FiniNullContext"
fail_programs(11) = "FiniNullHash"
fail_programs(12) = "GetFirstNullContext"
fail_programs(13) = "GetFirstNullHash"
fail_programs(14) = "GetFirstNullItem"
fail_programs(15) = "GetNextNullContext"
fail_programs(16) = "GetNextNullHash"
fail_programs(17) = "GetNextNullItem"
fail_programs(18) = "GetNullContext"
fail_programs(19) = "GetNullHash"
fail_programs(20) = "GetNullItem"
fail_programs(21) = "GetNullKey"
fail_programs(22) = "GetZeroKeyLength"
fail_programs(23) = "InitNullContext"
fail_programs(24) = "InitNullDataDef"
fail_programs(25) = "InitNullDefinition"
fail_programs(26) = "InitNullHash"
fail_programs(27) = "InitNullHashItemOffset"
fail_programs(28) = "InitNullKeyDef"
fail_programs(29) = "InitNullName"
fail_programs(30) = "InitNullParent"
fail_programs(31) = "InitNullStatus"
fail_programs(32) = "InitZeroBlocks"
fail_programs(33) = "InitZeroLength"
fail_programs(34) = "InsertNullContext"
fail_programs(35) = "InsertNullData"
fail_programs(36) = "InsertNullHash"
fail_programs(37) = "InsertNullKey"
fail_programs(38) = "InsertZeroLengthItem"
fail_programs(39) = "InsertZeroLengthKey"
fail_programs(40) = "ReleaseNullContext"
fail_programs(41) = "ReleaseNullHash"
fail_programs(42) = "ReleaseNullItem"
fail_programs(43) = "ReplaceNullContext"
fail_programs(44) = "ReplaceNullData"
fail_programs(45) = "ReplaceNullHash"
fail_programs(46) = "ReplaceNullKey"
fail_programs(47) = "ReplaceZeroLengthData"
fail_programs(48) = "ReplaceZeroLengthKey"
fail_programs(49) = "RollbackAddNullContext"
fail_programs(50) = "RollbackAddNullHash"
fail_programs(51) = "RollbackAddNullItem"
fail_programs(52) = "RollbackRemoveNullContext"
fail_programs(53) = "RollbackRemoveNullHash"
fail_programs(54) = "RollbackRemoveNullItem"
fail_programs(55) = "StatusNullHash"
fail_programs(56) = "StatusNullStatus"

numTests = 76                 ' Sum of length of both arrays 
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

