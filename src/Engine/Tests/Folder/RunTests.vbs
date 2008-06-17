' ***********************************************************************
'
' Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
' 
' This file is part of vdsf (Virtual Data Space Framework).
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
Dim failed_tests(103)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(21)
Dim fail_programs(81)

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
ok_programs(0)  = "ClosePass"
ok_programs(1)  = "CreateInvalidNameLength"
ok_programs(2)  = "CreatePass"
ok_programs(3)  = "DeletePass"
ok_programs(4)  = "DestroyPass"
ok_programs(5)  = "FiniPass"
ok_programs(6)  = "GetFirstEmpty"
ok_programs(7)  = "GetFirstPass"
ok_programs(8)  = "GetNextPass"
ok_programs(9)  = "GetPass"
ok_programs(10) = "GetStatusPass"
ok_programs(11) = "InitPass"
ok_programs(12) = "InsertPass"
ok_programs(13) = "OpenPass"
ok_programs(14) = "ReleasePass"
ok_programs(15) = "RemovePass"
ok_programs(16) = "StatusPass"
ok_programs(17) = "Tests"
ok_programs(18) = "TopCreateInvalidNameLength"
ok_programs(19) = "TopCreatePass"
ok_programs(20) = "TopStatusPass"
ok_programs(21) = "ValidatePass"

fail_programs(0)  = "CloseNullContext"
fail_programs(1)  = "CloseNullItem"
fail_programs(2)  = "CreateNullContext"
fail_programs(3)  = "CreateNullDefinition"
fail_programs(4)  = "CreateNullFolder"
fail_programs(5)  = "CreateNullName"
fail_programs(6)  = "CreateWrongType"
fail_programs(7)  = "DeleteInvalidSig"
fail_programs(8)  = "DeleteNullContext"
fail_programs(9)  = "DeleteNullFolder"
fail_programs(10) = "DeleteNullName"
fail_programs(11) = "DeleteWrongObjType"
fail_programs(12) = "DeleteZeroNameLength"
fail_programs(13) = "DestroyNullContext"
fail_programs(14) = "DestroyNullFolder"
fail_programs(15) = "DestroyNullName"
fail_programs(16) = "FiniInvalidSig"
fail_programs(17) = "FiniNullContext"
fail_programs(18) = "FiniNullFolder"
fail_programs(19) = "FiniWrongObjType"
fail_programs(20) = "GetFirstNullContext"
fail_programs(21) = "GetFirstNullFolder"
fail_programs(22) = "GetFirstNullItem"
fail_programs(23) = "GetFirstWrongObjType"
fail_programs(24) = "GetInvalidSig"
fail_programs(25) = "GetNextItemNullHashItem"
fail_programs(26) = "GetNextItemNullOffset"
fail_programs(27) = "GetNextNullContext"
fail_programs(28) = "GetNextNullFolder"
fail_programs(29) = "GetNextNullItem"
fail_programs(30) = "GetNextWrongObjType"
fail_programs(31) = "GetNullContext"
fail_programs(32) = "GetNullItem"
fail_programs(33) = "GetNullFolder"
fail_programs(34) = "GetNullName"
fail_programs(35) = "GetStatusNullContext"
fail_programs(36) = "GetStatusNullFolder"
fail_programs(37) = "GetStatusNullName"
fail_programs(38) = "GetStatusNullStatus"
fail_programs(39) = "GetStatusZeroLength"
fail_programs(40) = "GetWrongObjType"
fail_programs(41) = "GetZeroNameLength"
fail_programs(42) = "InitNullContext"
fail_programs(43) = "InitNullFolder"
fail_programs(44) = "InitNullKeyOffset"
fail_programs(45) = "InitNullName"
fail_programs(46) = "InitNullParentOffset"
fail_programs(47) = "InitNullStatus"
fail_programs(48) = "InitZeroBlocks"
fail_programs(49) = "InitZeroNameLength"
fail_programs(50) = "InsertInvalidSig"
fail_programs(51) = "InsertNullContext"
fail_programs(52) = "InsertNullDefinition"
fail_programs(53) = "InsertNullFolder"
fail_programs(54) = "InsertNullName"
fail_programs(55) = "InsertNullOrigName"
fail_programs(56) = "InsertWrongObjType"
fail_programs(57) = "InsertZeroBlocks"
fail_programs(58) = "InsertZeroNameLength"
fail_programs(59) = "OpenNullContext"
fail_programs(60) = "OpenNullItem"
fail_programs(61) = "OpenNullFolder"
fail_programs(62) = "OpenNullName"
fail_programs(63) = "ReleaseNullContext"
fail_programs(64) = "ReleaseNullFolder"
fail_programs(65) = "ReleaseNullItem"
fail_programs(66) = "ReleaseWrongObjType"
fail_programs(67) = "RemoveNullContext"
fail_programs(68) = "RemoveNullFolder"
fail_programs(69) = "RemoveNullItem"
fail_programs(70) = "RemoveWrongObjType"
fail_programs(71) = "StatusNullFolder"
fail_programs(72) = "StatusNullStatus"
fail_programs(73) = "TopCreateNullContext"
fail_programs(74) = "TopCreateNullDefinition"
fail_programs(75) = "TopCreateNullFolder"
fail_programs(76) = "TopCreateNullName"
fail_programs(77) = "TopCreateWrongType"
fail_programs(78) = "TopStatusNullContext"
fail_programs(79) = "TopStatusNullFolder"
fail_programs(80) = "TopStatusNullName"
fail_programs(81) = "TopStatusNullStatus"

numTests = 104                 ' Sum of length of both arrays 
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

