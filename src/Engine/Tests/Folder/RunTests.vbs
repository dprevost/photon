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
Dim failed_tests(130)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(29)
Dim fail_programs(100)

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
ok_programs(1)  = "CommitEditPass"
ok_programs(2)  = "CreateInvalidNameLength"
ok_programs(3)  = "CreatePass"
ok_programs(4)  = "DeletePass"
ok_programs(5)  = "DestroyPass"
ok_programs(6)  = "EditPass"
ok_programs(7)  = "FiniPass"
ok_programs(8)  = "GetFirstEmpty"
ok_programs(9)  = "GetFirstPass"
ok_programs(10) = "GetNextPass"
ok_programs(11) = "GetPass"
ok_programs(12) = "GetStatusPass"
ok_programs(13) = "InitPass"
ok_programs(14) = "InsertPass"
ok_programs(15) = "OpenPass"
ok_programs(16) = "OpenWrongLength"
ok_programs(17) = "OpenZeroLength"
ok_programs(18) = "ReleasePass"
ok_programs(19) = "RemovePass"
ok_programs(20) = "StatusPass"
ok_programs(21) = "Tests"
ok_programs(22) = "TopCreateInvalidNameLength"
ok_programs(23) = "TopCreatePass"
ok_programs(24) = "TopDestroyPass"
ok_programs(25) = "TopEditPass"
ok_programs(26) = "TopEditWrongLength"
ok_programs(27) = "TopEditZeroLength"
ok_programs(28) = "TopStatusPass"
ok_programs(29) = "ValidatePass"

fail_programs(0)   = "CloseNullContext"
fail_programs(1)   = "CloseNullItem"
fail_programs(2)   = "CommitEditNullContext"
fail_programs(3)   = "CommitEditNullFolder"
fail_programs(4)   = "CommitEditNullItem"
fail_programs(5)   = "CommitEditWrongObjType"
fail_programs(6)   = "CreateNullContext"
fail_programs(7)   = "CreateNullDefinition"
fail_programs(8)   = "CreateNullFolder"
fail_programs(9)   = "CreateNullName"
fail_programs(10)  = "CreateWrongType"
'fail_programs(11)  = "DeleteInvalidSig"
fail_programs(12)  = "DeleteNullContext"
fail_programs(13)  = "DeleteNullFolder"
fail_programs(14)  = "DeleteNullName"
fail_programs(15)  = "DeleteWrongObjType"
fail_programs(16)  = "DeleteZeroNameLength"
fail_programs(17)  = "DestroyNullContext"
fail_programs(18)  = "DestroyNullFolder"
fail_programs(19)  = "DestroyNullName"
fail_programs(20)  = "EditNullContext"
fail_programs(21)  = "EditNullFolder"
fail_programs(22)  = "EditNullItem"
fail_programs(23)  = "EditNullName"
fail_programs(24)  = "EditWrongObjectType"
fail_programs(25)  = "EditZeroNameLength"
'fail_programs(26)  = "FiniInvalidSig"
fail_programs(27)  = "FiniNullContext"
fail_programs(28)  = "FiniNullFolder"
fail_programs(29)  = "FiniWrongObjType"
fail_programs(30)  = "GetFirstNullContext"
fail_programs(31)  = "GetFirstNullFolder"
fail_programs(32)  = "GetFirstNullItem"
fail_programs(33)  = "GetFirstWrongObjType"
fail_programs(34)  = "GetInvalidSig"
fail_programs(35)  = "GetNextItemNullHashItem"
fail_programs(36)  = "GetNextItemNullOffset"
fail_programs(37)  = "GetNextNullContext"
fail_programs(38)  = "GetNextNullFolder"
fail_programs(39)  = "GetNextNullItem"
fail_programs(40)  = "GetNextWrongObjType"
fail_programs(41)  = "GetNullContext"
fail_programs(42)  = "GetNullItem"
fail_programs(43)  = "GetNullFolder"
fail_programs(44)  = "GetNullName"
fail_programs(45)  = "GetStatusNullContext"
fail_programs(46)  = "GetStatusNullFolder"
fail_programs(47)  = "GetStatusNullName"
fail_programs(48)  = "GetStatusNullStatus"
fail_programs(49)  = "GetStatusZeroLength"
fail_programs(50)  = "GetWrongObjType"
fail_programs(51)  = "GetZeroNameLength"
fail_programs(52)  = "InitNullContext"
fail_programs(53)  = "InitNullFolder"
fail_programs(54)  = "InitNullHashItemOffset"
fail_programs(55)  = "InitNullName"
fail_programs(56)  = "InitNullParentOffset"
fail_programs(57)  = "InitNullStatus"
fail_programs(58)  = "InitZeroBlocks"
fail_programs(59)  = "InitZeroNameLength"
'fail_programs(60)  = "InsertInvalidSig"
fail_programs(61)  = "InsertNullContext"
fail_programs(62)  = "InsertNullDefinition"
fail_programs(63)  = "InsertNullFolder"
fail_programs(64)  = "InsertNullName"
fail_programs(65)  = "InsertNullOrigName"
fail_programs(66)  = "InsertWrongObjType"
fail_programs(67)  = "InsertZeroBlocks"
fail_programs(68)  = "InsertZeroNameLength"
fail_programs(69)  = "OpenNullContext"
fail_programs(70)  = "OpenNullItem"
fail_programs(71)  = "OpenNullFolder"
fail_programs(72)  = "OpenNullName"
fail_programs(73)  = "OpenWrongType"
fail_programs(74)  = "ReleaseNullContext"
fail_programs(75)  = "ReleaseNullFolder"
fail_programs(76)  = "ReleaseNullItem"
fail_programs(77)  = "ReleaseWrongObjType"
fail_programs(78)  = "RemoveNullContext"
fail_programs(79)  = "RemoveNullFolder"
fail_programs(80)  = "RemoveNullItem"
fail_programs(81)  = "RemoveWrongObjType"
fail_programs(82)  = "StatusNullFolder"
fail_programs(83)  = "StatusNullStatus"
fail_programs(84)  = "TopCreateNullContext"
fail_programs(85)  = "TopCreateNullDefinition"
fail_programs(86)  = "TopCreateNullFolder"
fail_programs(87)  = "TopCreateNullName"
fail_programs(88)  = "TopCreateWrongType"
fail_programs(89)  = "TopDestroyNullContext"
fail_programs(90)  = "TopDestroyNullFolder"
fail_programs(91)  = "TopDestroyNullName"
fail_programs(92)  = "TopEditNullContext"
fail_programs(93)  = "TopEditNullFolder"
fail_programs(94)  = "TopEditNullItem"
fail_programs(95)  = "TopEditNullName"
fail_programs(96)  = "TopEditWrongType"
fail_programs(97)  = "TopStatusNullContext"
fail_programs(98)  = "TopStatusNullFolder"
fail_programs(99)  = "TopStatusNullName"
fail_programs(100) = "TopStatusNullStatus"

numTests = 131                 ' Sum of length of both arrays 
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

