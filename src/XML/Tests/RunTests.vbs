' ***********************************************************************
'
' Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

Dim numTests, xmlTest, verbose, rc, numFailed, testExec, status
Dim objShell
Dim objWshScriptExec
Dim exeName
Dim consoleMode
Dim objArgs, strArg, i
Dim strOutput

' List of failed tests. We append to this list when an error is encountered
' while running the tests
Dim failedTests(11)

' Lists containing the names of the tests
' The "ok" lists are for programs which are expected to return zero (succeed)
' and the "fail" lists are for the other ones.
Dim okTests(11)

' ***********************************************************************
'
' Initialization of these variables
'
' ***********************************************************************

' Populate the program lists...
okTests(0)  = "dupFieldName.xml"
okTests(1)  = "invalidDataType.xml"
okTests(2)  = "invalidIntegerSize.xml"
okTests(3)  = "invalidObjType.xml"
okTests(4)  = "invalidPrecision.xml"
okTests(5)  = "invalidScale.xml"
okTests(6)  = "noLastField.xml"
okTests(7)  = "noMD.xml"
okTests(8)  = "twoLastField.xml"
okTests(9)  = "zeroIntegerSize.xml"
okTests(10) = "zeroPrecision.xml"
okTests(11) = "zeroStringLength.xml"

numTests = 12 
numFailed = 0

consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

Set objShell = CreateObject("WScript.Shell")
verbose = False

Set objArgs = WScript.Arguments

' ***********************************************************************
'
' Start the program
'
' ***********************************************************************

For Each strArg in objArgs
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

exeName = "xmllint --noout --schema ..\photon_meta10.xsd md\"

' Run all tests

For Each xmlTest in okTests
   testExec = exeName & xmlTest
   WScript.Echo testExec
   if consoleMode then 
      WScript.Echo "Running " & xmlTest
      Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & testExec & Chr(34))
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
      rc = objShell.Run("%comspec% /c " & Chr(34) & testExec & Chr(34), 2, true)
   end if
   if rc = 0 then   
   wscript.echo "rc = " & rc & " " & test
      failedTests(numFailed) = test
      numFailed = numFailed + 1
   end if
Next

if consoleMode then
   WScript.StdOut.Write vbcrlf & "Total number of tests: " & numTests & vbcrlf
   WScript.StdOut.Write "Total number of failed tests: " & numFailed & vbcrlf & vbcrlf
   For i = 1 to numFailed 
      WScript.StdOut.Write "This test failed: " & failedTests(i-1) & vbcrlf
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

