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
Dim failed_tests(130)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(30)
Dim fail_programs(99)

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
ok_programs(0)  = "CommitEditPass"
ok_programs(1)  = "CreateInvalidNameLength"
ok_programs(2)  = "CreatePass"
ok_programs(3)  = "DeletePass"
ok_programs(4)  = "DestroyPass"
ok_programs(5)  = "EditPass"
ok_programs(6)  = "FiniPass"
ok_programs(7)  = "GetFirstEmpty"
ok_programs(8)  = "GetFirstPass"
ok_programs(9)  = "GetNextPass"
ok_programs(10) = "GetPass"
ok_programs(11) = "GetStatusPass"
ok_programs(12) = "InitPass"
ok_programs(13) = "InsertPass"
ok_programs(14) = "ReleasePass"
ok_programs(15) = "RemovePass"
ok_programs(16) = "ResizePass"
ok_programs(17) = "StatusPass"
ok_programs(18) = "Tests"
ok_programs(19) = "TopClosePass"
ok_programs(20) = "TopCreateInvalidNameLength"
ok_programs(21) = "TopCreatePass"
ok_programs(22) = "TopDestroyPass"
ok_programs(23) = "TopEditPass"
ok_programs(24) = "TopEditWrongLength"
ok_programs(25) = "TopEditZeroLength"
ok_programs(26) = "TopOpenPass"
ok_programs(27) = "TopOpenWrongLength"
ok_programs(28) = "TopOpenZeroLength"
ok_programs(29) = "TopStatusPass"
ok_programs(30) = "ValidatePass"

fail_programs(0)   = "CommitEditNullContext"
fail_programs(1)   = "CommitEditNullFolder"
fail_programs(2)   = "CommitEditNullItem"
fail_programs(3)   = "CommitEditWrongObjType"
fail_programs(4)   = "CreateNullContext"
fail_programs(5)   = "CreateNullDefinition"
fail_programs(6)   = "CreateNullFolder"
fail_programs(7)   = "CreateNullName"
fail_programs(8)   = "CreateWrongType"
fail_programs(9)   = "DeleteNullContext"
fail_programs(10)  = "DeleteNullFolder"
fail_programs(11)  = "DeleteNullName"
fail_programs(12)  = "DeleteWrongObjType"
fail_programs(13)  = "DeleteZeroNameLength"
fail_programs(14)  = "DestroyNullContext"
fail_programs(15)  = "DestroyNullFolder"
fail_programs(16)  = "DestroyNullName"
fail_programs(17)  = "EditNullContext"
fail_programs(18)  = "EditNullFolder"
fail_programs(19)  = "EditNullItem"
fail_programs(20)  = "EditNullName"
fail_programs(21)  = "EditWrongObjectType"
fail_programs(22)  = "EditZeroNameLength"
fail_programs(23)  = "FiniNullContext"
fail_programs(24)  = "FiniNullFolder"
fail_programs(25)  = "FiniWrongObjType"
fail_programs(26)  = "GetFirstNullContext"
fail_programs(27)  = "GetFirstNullFolder"
fail_programs(28)  = "GetFirstNullItem"
fail_programs(29)  = "GetFirstWrongObjType"
fail_programs(30)  = "GetInvalidSig"
fail_programs(31)  = "GetNextItemNullHashItem"
fail_programs(32)  = "GetNextItemNullOffset"
fail_programs(33)  = "GetNextNullContext"
fail_programs(34)  = "GetNextNullFolder"
fail_programs(35)  = "GetNextNullItem"
fail_programs(36)  = "GetNextWrongObjType"
fail_programs(37)  = "GetNullContext"
fail_programs(38)  = "GetNullItem"
fail_programs(39)  = "GetNullFolder"
fail_programs(40)  = "GetNullName"
fail_programs(41)  = "GetStatusNullContext"
fail_programs(42)  = "GetStatusNullFolder"
fail_programs(43)  = "GetStatusNullName"
fail_programs(44)  = "GetStatusNullStatus"
fail_programs(45)  = "GetStatusZeroLength"
fail_programs(46)  = "GetWrongObjType"
fail_programs(47)  = "GetZeroNameLength"
fail_programs(48)  = "InitNullContext"
fail_programs(49)  = "InitNullFolder"
fail_programs(50)  = "InitNullHashItemOffset"
fail_programs(51)  = "InitNullName"
fail_programs(52)  = "InitNullParentOffset"
fail_programs(53)  = "InitNullStatus"
fail_programs(54)  = "InitZeroBlocks"
fail_programs(55)  = "InitZeroNameLength"
fail_programs(56)  = "InsertNullContext"
fail_programs(57)  = "InsertNullDefinition"
fail_programs(58)  = "InsertNullFolder"
fail_programs(59)  = "InsertNullName"
fail_programs(60)  = "InsertNullOrigName"
fail_programs(61)  = "InsertWrongObjType"
fail_programs(62)  = "InsertZeroBlocks"
fail_programs(63)  = "InsertZeroNameLength"
fail_programs(64)  = "ReleaseNullContext"
fail_programs(65)  = "ReleaseNullFolder"
fail_programs(66)  = "ReleaseNullItem"
fail_programs(67)  = "ReleaseWrongObjType"
fail_programs(68)  = "RemoveNullContext"
fail_programs(69)  = "RemoveNullFolder"
fail_programs(70)  = "RemoveNullItem"
fail_programs(71)  = "RemoveWrongObjType"
fail_programs(72)  = "ResizeNullContext"
fail_programs(73)  = "ResizeNullFolder"
fail_programs(74)  = "StatusNullFolder"
fail_programs(75)  = "StatusNullStatus"
fail_programs(76)   = "TopCloseNullContext"
fail_programs(77)   = "TopCloseNullItem"
fail_programs(78)  = "TopCreateNullContext"
fail_programs(79)  = "TopCreateNullDefinition"
fail_programs(80)  = "TopCreateNullFolder"
fail_programs(81)  = "TopCreateNullName"
fail_programs(82)  = "TopCreateWrongType"
fail_programs(83)  = "TopDestroyNullContext"
fail_programs(84)  = "TopDestroyNullFolder"
fail_programs(85)  = "TopDestroyNullName"
fail_programs(86)  = "TopEditNullContext"
fail_programs(87)  = "TopEditNullFolder"
fail_programs(88)  = "TopEditNullItem"
fail_programs(89)  = "TopEditNullName"
fail_programs(90)  = "TopEditWrongType"
fail_programs(91)  = "TopOpenNullContext"
fail_programs(92)  = "TopOpenNullItem"
fail_programs(93)  = "TopOpenNullFolder"
fail_programs(94)  = "TopOpenNullName"
fail_programs(95)  = "TopOpenWrongType"
fail_programs(96)  = "TopStatusNullContext"
fail_programs(97)  = "TopStatusNullFolder"
fail_programs(98)  = "TopStatusNullName"
fail_programs(99) = "TopStatusNullStatus"

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

