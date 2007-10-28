' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' This script is provided both as an example and as a way to easily
' launch the watchdog for debugging purposes.
'
' The script will accept a single argument, the path where the watchdog
' is located. If not provided, the script assumes that the watchdog is
' located in "Watchdog".
'
' \todo Additional arguments (to control the location of the working
'       files, the size of the VDS, etc.) should be added eventually.
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Option Explicit

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Declaration of variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Dim rc
Dim objShell
Dim objWshScriptExec
Dim fso
Dim objArgs
Dim consoleMode
Dim tmpDir
Dim cmdFile
Dim exeName

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Initialization of these variables
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Set objArgs = WScript.Arguments
consoleMode = True
If Right(LCase(Wscript.FullName), 11) = "wscript.exe" Then
   consoleMode = False
End If

' Create the FileSystemObject
Set fso = CreateObject ("Scripting.FileSystemObject")

' Create an object shell
Set objShell = CreateObject("WScript.Shell")

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
'
' Start the program
'
' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Wscript.echo "This launch a new watchdog. The vds is removed upon termination."
if consoleMode then
   Wscript.echo "This launch a new watchdog. The vds is removed upon termination."
   Wscript.echo "You can specify the path to the watchdog executable as the first argument"
end if

tmpDir = objShell.Environment.item("TMP")
tmpDir = objShell.ExpandEnvironmentStrings(tmpDir)
tmpDir = tmpDir + "\vdsf_001"

if (fso.FolderExists(tmpDir)) Then
   fso.DeleteFolder(tmpDir)
end if

fso.CreateFolder(tmpDir)

Set cmdFile = fso.CreateTextFile(tmpDir + "\cfg.txt", True)
cmdFile.WriteLine("# VDSF Config file             ")
cmdFile.WriteLine("#                              ")
cmdFile.WriteLine("VDSLocation           " + tmpDir)
cmdFile.WriteLine("#MemorySize is in kbytes       ")
cmdFile.WriteLine("MemorySize            10000    ")
cmdFile.WriteLine("WatchdogAddress       10701    ")
cmdFile.WriteLine("LogTransaction        0        ")
cmdFile.WriteLine("FilePermissions       0660     ")
cmdFile.WriteLine("DirectoryPermissions  0770     ")

exeName = ".\debug\vdswd.exe -c " + tmpDir + "\cfg.txt"
'WScript.Echo "Running " & exeName
'if consoleMode then 
'   WScript.Echo "Running " & exeName
'   Set objWshScriptExec = objShell.Exec("%comspec% /c " & Chr(34) & exeName & Chr(34))
'   status = objWshScriptExec.Status
'   Do While objWshScriptExec.Status = 0
'      WScript.Sleep 100
'   Loop
'   strOutput = objWshScriptExec.StdOut.ReadAll
'   if verbose then 
'      WScript.Stdout.Write objWshScriptExec.StdErr.ReadAll
'   end if
'   rc = objWshScriptExec.ExitCode
'else
   rc = objShell.Run("%comspec% /c " & Chr(34) & exeName & Chr(34), 2, true)
'end if
'WScript.Echo  rc
Wscript.quit(0)

'rm -rf $BASE_DIR

' --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

