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
Dim failed_tests(49)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(12)
Dim fail_programs(36)

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
ok_programs(3)  = "DestroyPass"
ok_programs(4)  = "EditPass"
ok_programs(5)  = "EditWrongLength"
ok_programs(6)  = "EditZeroLength"
ok_programs(7)  = "GetDefLengthPass"
ok_programs(8)  = "GetDefPass"
ok_programs(9)  = "OpenPass"
ok_programs(10) = "OpenWrongLength"
ok_programs(11) = "OpenZeroLength"
ok_programs(12) = "StatusPass"

fail_programs(0)  = "CloseNullContext"
fail_programs(1)  = "CloseNullItem"
fail_programs(2)  = "CreateNullContext"
fail_programs(3)  = "CreateNullDefinition"
fail_programs(4)  = "CreateNullFolder"
fail_programs(5)  = "CreateNullName"
fail_programs(6)  = "CreateWrongType"
fail_programs(7)  = "DestroyNullContext"
fail_programs(8)  = "DestroyNullFolder"
fail_programs(9)  = "DestroyNullName"
fail_programs(10) = "EditNullContext"
fail_programs(11) = "EditNullFolder"
fail_programs(12) = "EditNullItem"
fail_programs(13) = "EditNullName"
fail_programs(14) = "EditWrongType"
fail_programs(15) = "GetDefLengthNullContext"
fail_programs(16) = "GetDefLengthNullDataLen"
fail_programs(17) = "GetDefLengthNullFolder"
fail_programs(18) = "GetDefLengthNullKeyLen"
fail_programs(19) = "GetDefLengthNullName"
fail_programs(20) = "GetDefNullContext"
fail_programs(21) = "GetDefNullDataDef"
fail_programs(22) = "GetDefNullDataLength"
fail_programs(23) = "GetDefNullDef"
fail_programs(24) = "GetDefNullFolder"
fail_programs(25) = "GetDefNullKeyDef"
fail_programs(26) = "GetDefNullKeyLength"
fail_programs(27) = "GetDefNullName"
fail_programs(28) = "OpenNullContext"
fail_programs(29) = "OpenNullItem"
fail_programs(30) = "OpenNullFolder"
fail_programs(31) = "OpenNullName"
fail_programs(32) = "OpenWrongType"
fail_programs(33) = "StatusNullContext"
fail_programs(34) = "StatusNullFolder"
fail_programs(35) = "StatusNullName"
fail_programs(36) = "StatusNullStatus"

numTests = 50                 ' Sum of length of both arrays 
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

