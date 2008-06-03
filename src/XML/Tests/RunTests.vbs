' ***********************************************************************
'
' Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
Dim objSocket

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failed_tests(11)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim ok_programs(11)

Dim exe_name, prog_path, program, wd_path, tmpDir, cmdFile, exeName
Dim consoleMode
Dim objArgs, strArg, i
dim strOutput

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
ok_programs(0)  = "dupFieldName.xml"
ok_programs(1)  = "invalidDataType.xml"
ok_programs(2)  = "invalidIntegerSize.xml"
ok_programs(3)  = "invalidObjType.xml"
ok_programs(4)  = "invalidPrecision.xml"
ok_programs(5)  = "invalidScale.xml"
ok_programs(6)  = "noLastField.xml"
ok_programs(7)  = "noMD.xml"
ok_programs(8)  = "twoLastField.xml"
ok_programs(9)  = "zeroIntegerSize.xml"
ok_programs(10) = "zeroPrecision.xml"
ok_programs(11) = "zeroStringLength.xml"

numTests = 12 
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

exeName = "xmllint --noout --schema ..\vdsf_md10.xml md\"

' Run all tests

For Each xml_test in ok_tests
   test_exec = exe_name & xml_test
   WScript.Echo test_exec
   if consoleMode then 
      WScript.Echo "Running " & test
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & test_exec & Chr(34))
      status = objWshScriptExec.Status
      Do While objWshScriptExec.Status = 0
         WScript.Sleep 100
      Loop
      strOutput = objWshScriptExec.StdOut.ReadAll
'      if verbose then 
         WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
'      end if
      rc = objWshScriptExec.ExitCode
   else
      rc = objShell.Run("%comspec% /c " & Chr(34) & test_exec & Chr(34), 2, true)
   end if
   if rc = 0 then   
   wscript.echo "rc = " & rc & " " & test
      failed_tests(numFailed) = test
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

