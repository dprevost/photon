' ***********************************************************************
'
' Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
' 
' This file is part of photon (photonsoftware.org).
' 
' This file may be distributed and/or modified under the terms of the
' GNU General Public License version 2 as published by the Free Software
' Foundation and appearing in the file COPYING included in the
' packaging of this library.
' 
' This file is distributed in the hope that it will be useful,
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
Dim failed_tests(62)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(13)
Dim fail_programs(48)

Dim exe_name, prog_path, program
Dim consoleMode
Dim objArgs, strArg, i
dim strOutput

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
ok_programs(0)  = "CopyPass"
ok_programs(1)  = "CopyPassSameLength"
ok_programs(2)  = "DeleteAtPass"
ok_programs(3)  = "DeletePass"
ok_programs(4)  = "EmptyPass"
ok_programs(5)  = "FiniPass"
ok_programs(6)  = "GetFirstPass"
ok_programs(7)  = "GetNextPass"
ok_programs(8)  = "GetPass"
ok_programs(9)  = "InitPass"
ok_programs(10) = "InsertAtPass"
ok_programs(11) = "InsertPass"
ok_programs(12) = "ResizePass"
ok_programs(13) = "UpdatePass"

fail_programs(0)  = "CopyNullContext"
fail_programs(1)  = "CopyNullNewHash"
fail_programs(2)  = "CopyNullOldHash"
fail_programs(3)  = "DeleteAtNullContext"
fail_programs(4)  = "DeleteAtNullHash"
fail_programs(5)  = "DeleteAtNullItem"
fail_programs(6)  = "DeleteNullContext"
fail_programs(7)  = "DeleteNullHash"
fail_programs(8)  = "DeleteNullKey"
fail_programs(9)  = "DeleteZeroLength"
fail_programs(10) = "EmptyNullContext"
fail_programs(11) = "EmptyNullHash"
fail_programs(12) = "FiniNullHash"
fail_programs(13) = "GetFirstNullHash"
fail_programs(14) = "GetFirstNullOffset"
fail_programs(15) = "GetNextNullHash"
fail_programs(16) = "GetNextNullNextOffset"
fail_programs(17) = "GetNextNullPrevOffset"
fail_programs(18) = "GetNullBucket"
fail_programs(19) = "GetNullContext"
fail_programs(20) = "GetNullHash"
fail_programs(21) = "GetNullItem"
fail_programs(22) = "GetNullKey"
fail_programs(23) = "GetZeroKeyLength"
fail_programs(24) = "InitNullContext"
fail_programs(25) = "InitNullHash"
fail_programs(26) = "InitNullOffset"
fail_programs(27) = "InsertAtNullContext"
fail_programs(28) = "InsertAtNullData"
fail_programs(29) = "InsertAtNullHash"
fail_programs(30) = "InsertAtNullKey"
fail_programs(31) = "InsertAtNullNewItem"
fail_programs(32) = "InsertAtZeroDataLength"
fail_programs(33) = "InsertAtZeroKeyLength"
fail_programs(34) = "InsertNullContext"
fail_programs(35) = "InsertNullData"
fail_programs(36) = "InsertNullHash"
fail_programs(37) = "InsertNullKey"
fail_programs(38) = "InsertNullNewItem"
fail_programs(39) = "InsertZeroDataLength"
fail_programs(40) = "InsertZeroKeyLength"
fail_programs(41) = "ResizeNullContext"
fail_programs(42) = "ResizeNullHash"
fail_programs(43) = "UpdateNullContext"
fail_programs(44) = "UpdateNullData"
fail_programs(45) = "UpdateNullHash"
fail_programs(46) = "UpdateNullKey"
fail_programs(47) = "UpdateZeroDataLength"
fail_programs(48) = "UpdateZeroKeyLength"

numTests = 63                 ' Sum of length of both arrays 
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
   if LCase(strArg) = "--verbose" then 
      verbose = True
   end if
Next 

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
